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

#ifndef LIBBITCOIN_BLOCKCHAIN_FIXED_SECONDARY_KEY_VALUE_ITERATOR_IPP
#define LIBBITCOIN_BLOCKCHAIN_FIXED_SECONDARY_KEY_VALUE_ITERATOR_IPP

namespace libbitcoin {
namespace blockchain {

// trie_structure_iterator implementation
template <typename K, typename V, typename P, typename R, typename C>
fixed_secondary_key_value_iterator<K, V, P, R, C>::fixed_secondary_key_value_iterator()
    : iterator_(nullptr), value_node_(nullptr)
{
}

template <typename K, typename V, typename P, typename R, typename C>
fixed_secondary_key_value_iterator<K, V, P, R, C>::fixed_secondary_key_value_iterator(
    structure_node_ptr node)
    : iterator_(node), value_node_(nullptr)
{
    auto key = iterator_.get_secondary_key();
    auto query = (*iterator_)->store.retrieve(key);

    if (query.second)
    {
        if (((*query.first).head_leftmost != nullptr) &&
            ((*query.first).head_leftmost->anchor == iterator_.trie_node_))
            value_node_ = (*query.first).head_leftmost;
    }
}

template <typename K, typename V, typename P, typename R, typename C>
fixed_secondary_key_value_iterator<K, V, P, R, C>::fixed_secondary_key_value_iterator(
    const key_type& key, structure_node_ptr node)
    : iterator_(key, node), value_node_(nullptr)
{
    auto query = (*iterator_)->store.retrieve(key);

    if (query.second)
    {
        if (((*query.first).head_leftmost != nullptr) &&
            ((*query.first).head_leftmost->anchor == iterator_.trie_node_))
            value_node_ = (*query.first).head_leftmost;
    }
}

template <typename K, typename V, typename P, typename R, typename C>
fixed_secondary_key_value_iterator<K, V, P, R, C>::fixed_secondary_key_value_iterator(
    const key_type& key, value_node_ptr value_node)
    : iterator_(key, (value_node != nullptr) ? value_node->anchor : nullptr),
      value_node_(value_node)
{
}

template <typename K, typename V, typename P, typename R, typename C>
fixed_secondary_key_value_iterator<K, V, P, R, C>::fixed_secondary_key_value_iterator(
    const iterator& it)
    : iterator_(it.iterator_), value_node_(it.value_node_)
{
}

template <typename K, typename V, typename P, typename R, typename C>
fixed_secondary_key_value_iterator<K, V, P, R, C>::operator bool() const
{
    return iterator_ && (value_node_ != nullptr);
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::reference
    fixed_secondary_key_value_iterator<K, V, P, R, C>::operator*() const
{
    return value_node_->value;
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::pointer
    fixed_secondary_key_value_iterator<K, V, P, R, C>::operator->() const
{
    return &(operator*());
}

template <typename K, typename V, typename P, typename R, typename C>
bool fixed_secondary_key_value_iterator<K, V, P, R, C>::operator==(
    const iter_type& other) const
{
    return (iterator_ == other.iterator_) &&
        (value_node_ == other.value_node_);
}

template <typename K, typename V, typename P, typename R, typename C>
bool fixed_secondary_key_value_iterator<K, V, P, R, C>::operator!=(
    const iter_type& other) const
{
    return !(*this == other);
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::iter_type&
fixed_secondary_key_value_iterator<K, V, P, R, C>::operator++()
{
    increment();
    return *this;
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::iter_type
    fixed_secondary_key_value_iterator<K, V, P, R, C>::operator++(int)
{
    auto it = *this;
    increment();
    return it;
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::iter_type&
fixed_secondary_key_value_iterator<K, V, P, R, C>::operator--()
{
    decrement();
    return *this;
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::iter_type
    fixed_secondary_key_value_iterator<K, V, P, R, C>::operator--(int)
{
    auto it = *this;
    decrement();
    return it;
}

template <typename K, typename V, typename P, typename R, typename C>
void fixed_secondary_key_value_iterator<K, V, P, R, C>::increment()
{
    if ((value_node_ != nullptr) && (value_node_->next != nullptr))
        value_node_ = value_node_->next;
    else
    {
        value_node_ = nullptr;
        ++iterator_;
        auto query = (*iterator_)->store.retrieve(get_secondary_key());

        if (query.second)
        {
            if (((*query.first).head_leftmost != nullptr) &&
                ((*query.first).head_leftmost->anchor == iterator_.trie_node_))
                    value_node_ = (*query.first).head_leftmost;
        }
    }
}

template <typename K, typename V, typename P, typename R, typename C>
void fixed_secondary_key_value_iterator<K, V, P, R, C>::decrement()
{
    if ((value_node_ != nullptr) && (value_node_->previous != nullptr))
        value_node_ = value_node_->previous;
    else
    {
        value_node_ = nullptr;
        --iterator_;
        auto query = (*iterator_)->store.retrieve(get_secondary_key());

        if (query.second)
        {
            if (((*query.first).head_leftmost != nullptr) &&
                ((*query.first).head_leftmost->anchor == iterator_.trie_node_))
                    value_node_ = (*query.first).tail_rightmost;
        }
    }
}

template <typename K, typename V, typename P, typename R, typename C>
binary_type fixed_secondary_key_value_iterator<K, V, P, R, C>::get_primary_key()
{
    return iterator_.get_primary_key();
}

template <typename K, typename V, typename P, typename R, typename C>
bool fixed_secondary_key_value_iterator<K, V, P, R, C>::first_value_with_primary_key() const
{
    bool result = false;
    auto query = (*iterator_)->store.retrieve(get_secondary_key());

    if (query.second)
        result = ((*query.first).head_leftmost == value_node_);

    return result;
}

template <typename K, typename V, typename P, typename R, typename C>
bool fixed_secondary_key_value_iterator<K, V, P, R, C>::last_value_with_primary_key() const
{
    bool result = false;
    auto query = (*iterator_)->store.retrieve(get_secondary_key());

    if (query.second)
        result = ((*query.first).tail_rightmost == value_node_);

    return result;
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::key_type
    fixed_secondary_key_value_iterator<K, V, P, R, C>::get_secondary_key() const
{
    return iterator_.get_secondary_key();
}

template <typename K, typename V, typename P, typename R, typename C>
typename fixed_secondary_key_value_iterator<K, V, P, R, C>::structure_iterator
    fixed_secondary_key_value_iterator<K, V, P, R, C>::get_structure_iterator() const
{
    return iterator_;
}

} // namespace blockchain
} // namespace libbitcoin

#endif
