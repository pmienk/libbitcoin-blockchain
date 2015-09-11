/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBBITCOIN_BLOCKCHAIN_RED_BLACK_TREE_IPP
#define LIBBITCOIN_BLOCKCHAIN_RED_BLACK_TREE_IPP

namespace libbitcoin {
namespace blockchain {

template<typename Key, typename Value, typename Comparer, typename Allocator>
red_black_tree<Key, Value, Comparer, Allocator>::red_black_tree(
    node_allocator allocator)
    : allocator_(allocator), nil_(create_node()), root_(nil_)
{
    nil_->left = nil_;
    nil_->parent = nil_;
    nil_->right = nil_;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
red_black_tree<Key, Value, Comparer, Allocator>::~red_black_tree()
{
    destroy();
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
const typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::root() const
{
    return root_;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
const typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::nil() const
{
    return nil_;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::destroy()
{
    if (root_ != nil_)
    {
        destroy_subtree(root_);
        destroy_node(root_);
    }

    root_ = nullptr;
    destroy_node(nil_);
    nil_ = nullptr;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::create_node()
{
    auto node = allocator_.allocate(1);
    if (node == nullptr)
        throw std::bad_alloc();

    new (node)node_type();
    return node;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::create_node(
        const key_type& key, const value_type& value)
{
    auto node = create_node();
    node->key = key;
    node->value = value;
    node->is_red = true;
    node->parent = nil_;
    node->right = nil_;
    node->left = nil_;
    return node;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
bool red_black_tree<Key, Value, Comparer, Allocator>::destroy_node(
    node_type* node)
{
    auto result = false;
    if (node != nullptr)
    {
        allocator_.destroy(node);
        allocator_.deallocate(node, 1);
        result = true;
    }

    return result;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::destroy_subtree(
    node_type* node)
{
    if ((node != nullptr) && (node != nil_))
    {
        auto current = leftmost_leaf(node);

        while (current != node)
        {
            auto removable = current;

            if ((current == current->parent->left) &&
                (current->parent->right != nil_))
            {
                current = leftmost_leaf(current->parent->right);
            }
            else
                current = current->parent;

            destroy_node(removable);
        }
    }
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::add(
    node_type* node)
{
    node_type* parent = nil_;
    node_type* current = root_;

    while (current != nil_)
    {
        parent = current;

        if (comparer_(node->key, current->key))
            current = current->right;
        else
            current = current->left;
    }

    node->parent = parent;

    if (parent == nil_)
        root_ = node;
    else if (comparer_(node->key, parent->key))
        parent->right = node;
    else
        parent->left = node;

    balance_add(node);
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::remove(
    node_type* node)
{
    auto potential_violator = node;
    bool potential_violator_was_red = potential_violator->is_red;

    node_type* replacement = nil_;

    if (node->left == nil_)
    {
        replacement = node->right;
        transplant(node, replacement);
    }
    else if (node->right == nil_)
    {
        replacement = node->left;
        transplant(node, replacement);
    }
    else
    {
        potential_violator = tree_minimum(node->right);
        potential_violator_was_red = potential_violator->is_red;
        replacement = potential_violator->right;

        if (potential_violator->parent == node)
        {
            replacement->parent = potential_violator;
        }
        else
        {
            transplant(potential_violator, potential_violator->right);
            potential_violator->right = node->right;
            potential_violator->right->parent = potential_violator;
        }

        transplant(node, potential_violator);
        potential_violator->left = node->left;
        potential_violator->left->parent = potential_violator;
        potential_violator->is_red = node->is_red;
    }

    if (!potential_violator_was_red)
        balance_remove(replacement);
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::retrieve(
        key_type key)
{
    node_type* current = root_;

    while (current != nil_)
    {
        bool a_compare_b = comparer_(key, current->key);
        bool b_compare_a = comparer_(current->key, key);
        bool equal = (a_compare_b && b_compare_a) || (!a_compare_b && !b_compare_a);

        if (equal)
            break;
        else if (a_compare_b)
            current = current->right;
        else
            current = current->left;
    }

    return current;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::retrieve_greater_equal(
        key_type key)
{
    node_type* last = nil_;
    node_type* current = root_;

    while (current != nil_)
    {
        bool a_compare_b = comparer_(key, current->key);
        bool b_compare_a = comparer_(current->key, key);
        bool equal = (a_compare_b && b_compare_a) || (!a_compare_b && !b_compare_a);

        if (equal)
            break;
        else if (a_compare_b)
            current = current->right;
        else
        {
            last = current;
            current = current->left;
        }
    }

    if (current == nil_)
        current = last;

    return current;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::balance_add(
    node_type* node)
{
    while (node->parent->is_red)
    {
        bool is_parent_left_sibling = (node->parent == node->parent->parent->left);

        node_type* parent_sibling = is_parent_left_sibling ?
            node->parent->parent->right : node->parent->parent->left;

        if (parent_sibling->is_red)
        {
            node->parent->is_red = false;
            parent_sibling->is_red = false;
            node->parent->parent->is_red = true;
            node = node->parent->parent;
        }
        else
        {
            if ((is_parent_left_sibling && (node == node->parent->right)) ||
                (!is_parent_left_sibling && (node == node->parent->left)))
            {
                node = node->parent;

                if (is_parent_left_sibling)
                    rotate_left(node);
                else
                    rotate_right(node);
            }

            node->parent->is_red = false;
            node->parent->parent->is_red = true;

            if (is_parent_left_sibling)
                rotate_right(node->parent->parent);
            else
                rotate_left(node->parent->parent);
        }
    }

    root_->is_red = false;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::balance_remove(
    node_type* node)
{
    auto current = node;

    while ((current != root_) && !current->is_red)
    {
        bool is_current_left_sibling = (current == current->parent->left);

        auto sibling = is_current_left_sibling ?
            current->parent->right : current->parent->left;

        if (sibling->is_red)
        {
            sibling->is_red = false;
            current->parent->is_red = true;

            if (is_current_left_sibling)
            {
                rotate_left(current->parent);
                sibling = current->parent->right;
            }
            else
            {
                rotate_right(current->parent);
                sibling = current->parent->left;
            }
        }

        if (!sibling->left->is_red && !sibling->right->is_red)
        {
            sibling->is_red = true;
            current = current->parent;
        }
        else
        {
            if ((is_current_left_sibling && !sibling->right->is_red) ||
                (!is_current_left_sibling && !sibling->left->is_red))
            {
                sibling->is_red = true;

                if (is_current_left_sibling)
                {
                    sibling->left->is_red = false;
                    rotate_right(sibling);
                    sibling = current->parent->right;
                }
                else
                {
                    sibling->right->is_red = false;
                    rotate_left(sibling);
                    sibling = current->parent->left;
                }
            }

            sibling->is_red = current->parent->is_red;
            current->parent->is_red = false;

            if (is_current_left_sibling)
            {
                sibling->right->is_red = false;
                rotate_left(current->parent);
            }
            else
            {
                sibling->left->is_red = false;
                rotate_right(current->parent);
            }

            current = root_;
        }
    }

    current->is_red = false;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::leftmost_leaf(
        node_type* node)
{
    auto last = nil_;
    auto current = node;

    if (current != nil_)
    {
        while (current != nil_)
        {
            last = current;
            current = current->left != nil_ ? current->left : current->right;
        }
    }

    return last;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::transplant(
    node_type* alpha, node_type* beta)
{
    if (alpha == root_)
        root_ = beta;
    else if (alpha == alpha->parent->left)
        alpha->parent->left = beta;
    else
        alpha->parent->right = beta;

    beta->parent = alpha->parent;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_tree<Key, Value, Comparer, Allocator>::node_type*
    red_black_tree<Key, Value, Comparer, Allocator>::tree_minimum(
        node_type* node)
{
    auto current = node;

    while (current->left != nil_)
        current = current->left;

    return current;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::rotate_left(
    node_type* alpha)
{
    auto beta = alpha->right;
    alpha->right = beta->left;

    if (beta->left != nil_)
        beta->left->parent = alpha;

    transplant(alpha, beta);

    beta->left = alpha;
    alpha->parent = beta;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
void red_black_tree<Key, Value, Comparer, Allocator>::rotate_right(
    node_type* alpha)
{
    auto beta = alpha->left;
    alpha->left = beta->right;

    if (beta->right != nil_)
        beta->right->parent = alpha;

    transplant(alpha, beta);

    beta->right = alpha;
    alpha->parent = beta;
}

} // namespace blockchain
} // namespace libbitcoin

#endif
