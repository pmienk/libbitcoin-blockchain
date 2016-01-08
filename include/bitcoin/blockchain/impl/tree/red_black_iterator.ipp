/**
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef LIBBITCOIN_BLOCKCHAIN_RED_BLACK_ITERATOR_IPP
#define LIBBITCOIN_BLOCKCHAIN_RED_BLACK_ITERATOR_IPP

namespace libbitcoin {
namespace blockchain {

// secondary_key_fixed_iterator implementation
template< typename Key, typename Value, typename Reference, typename Pointer>
red_black_iterator<Key, Value, Reference, Pointer>::red_black_iterator()
    : nil_(nullptr), root_(nullptr), node_(nullptr)
{
}

//template< typename Key, typename Value, typename Reference, typename Pointer>
//red_black_iterator<Key, Value, Reference, Pointer>::red_black_iterator(
//    node_type* nil, node_type* root, node_type* node)
//    : nil_(nil), root_(root), node_(node)
//{
//}

template< typename Key, typename Value, typename Reference, typename Pointer>
template<typename Comparer, typename Allocator>
red_black_iterator<Key, Value, Reference, Pointer>::red_black_iterator(
    red_black_tree<Key, Value, Comparer, Allocator>* tree, node_type* node)
    : nil_(tree->nil()), root_(tree->root()), node_(node)
{
}

template< typename Key, typename Value, typename Reference, typename Pointer>
red_black_iterator<Key, Value, Reference, Pointer>::red_black_iterator(
    const iterator& it)
    : nil_(it.nil_), root_(it.root_), node_(it.node_)
{
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::reference
    red_black_iterator<Key, Value, Reference, Pointer>::operator*() const
{
    return node_->value;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::pointer
    red_black_iterator<Key, Value, Reference, Pointer>::operator->() const
{
    return &(operator*());
}

template< typename Key, typename Value, typename Reference, typename Pointer>
bool red_black_iterator<Key, Value, Reference, Pointer>::operator==(
    const iter_type& other) const
{
    return (node_ == other.node_);
}

template< typename Key, typename Value, typename Reference, typename Pointer>
bool red_black_iterator<Key, Value, Reference, Pointer>::operator!=(
    const iter_type& other) const
{
    return !(*this == other);
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::iter_type&
    red_black_iterator<Key, Value, Reference, Pointer>::operator++()
{
    increment();
    return *this;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::iter_type
    red_black_iterator<Key, Value, Reference, Pointer>::operator++(int)
{
    auto it = *this;
    increment();
    return it;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::iter_type&
    red_black_iterator<Key, Value, Reference, Pointer>::operator--()
{
    decrement();
    return *this;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::iter_type
    red_black_iterator<Key, Value, Reference, Pointer>::operator--(int)
{
    auto it = *this;
    decrement();
    return it;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
void red_black_iterator<Key, Value, Reference, Pointer>::increment()
{
    auto current = node_;

    if (current != nil_)
    {
        bool allow_right_child_descent = true;

        while (current != nil_)
        {
            if (allow_right_child_descent && (current->right != nil_))
            {
                current = get_leftmost_child(current->right);
                break;
            }

            allow_right_child_descent = (current->parent->left == current);
            current = current->parent;

            if (allow_right_child_descent)
                break;
        }
    }
    else if (root_ != nil_)
        current = get_leftmost_child(root_);

    node_ = current;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
void red_black_iterator<Key, Value, Reference, Pointer>::decrement()
{
    auto current = node_;

    if (current != nil_)
    {
        bool allow_left_child_descent = true;

        while (current != nil_)
        {
            if (allow_left_child_descent && (current->left != nil_))
            {
                current = get_rightmost_child(current->left);
                break;
            }

            allow_left_child_descent = (current->parent->right == current);
            current = current->parent;

            if (allow_left_child_descent)
                break;
        }
    }
    else if (root_ != nil_)
        current = get_rightmost_child(root_);

    node_ = current;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::node_type*
    red_black_iterator<Key, Value, Reference, Pointer>::get_leftmost_child(
        node_type* node) const
{
    auto current = node;

    if ((current != nil_) && (current != nullptr))
    {
        while (current->left != nil_)
            current = current->left;
    }

    return current;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::node_type*
    red_black_iterator<Key, Value, Reference, Pointer>::get_rightmost_child(
        node_type* node) const
{
    auto current = node;

    if ((current != nil_) && (current != nullptr))
    {
        while (current->right != nil_)
            current = current->right;
    }

    return current;
}

template< typename Key, typename Value, typename Reference, typename Pointer>
typename red_black_iterator<Key, Value, Reference, Pointer>::key_type
    red_black_iterator<Key, Value, Reference, Pointer>::get_key()
{
    key_type result;

    if (node_ != nullptr)
        result = node_->key;

    return result;
}

} // namespace blockchain
} // namespace libbitcoin

#endif