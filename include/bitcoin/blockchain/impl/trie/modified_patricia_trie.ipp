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

#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMARY_PREFIXED_SECONDARY_ORDERED_PATRICIA_TRIE_IPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMARY_PREFIXED_SECONDARY_ORDERED_PATRICIA_TRIE_IPP

#include <new>

namespace libbitcoin {
namespace blockchain {

// modified_patricia_trie implementation
template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::modified_patricia_trie(
    structure_node_allocator structure_allocator,
    value_node_allocator value_allocator)
    : structure_allocator_(structure_allocator),
      value_allocator_(value_allocator), root_(create_structure_node()),
      end_(secondary_key_type(), root_)
{
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::~modified_patricia_trie()
{
    destroy();
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::destroy()
{
    auto current = get_leftmost_leaf(root_);

    while ((current != root_))
    {
        auto last = current;
        auto parent = current->parent;

        if (current != parent->get_last_child())
            current = get_leftmost_leaf(parent->get_last_child());
        else
            current = parent;

        parent->remove_child(last);
        destroy_structure_node(last);
    }

    destroy_structure_node(root_);

    root_ = nullptr;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::create_structure_node()
{
    auto node = structure_allocator_.allocate(1);
    if (node == nullptr)
        throw std::bad_alloc();

    new (node)structure_node_type();
    return node;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::create_structure_node(
        const binary_type& key)
{
    auto node = create_structure_node();
    node->label = key;
    return node;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::create_structure_node(
        const binary_type& primary, const secondary_key_type& secondary,
        const value_type& value)
{
    auto node = create_structure_node(primary);
    auto value_node = create_value_node(value);
    append_value(node, secondary, value_node);
    return node;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
bool modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::destroy_structure_node(
    structure_node_type* node)
{
    auto result = false;
    if (node != nullptr)
    {
        erase_values(node);
        structure_allocator_.destroy(node);
        structure_allocator_.deallocate(node, 1);
        result = true;
    }

    return result;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::erase_values(
    structure_node_type* node)
{
    if (node != nullptr)
    {
        for (auto it = node->store.begin(); it != node->store.end(); ++it)
        {
            bool immutable = false;
            auto value = (*it).head_leftmost;

            // Restrict to removing only anchored values, rely on the fact
            // that there cannot be a mix of anchored and unanchored values.
            if ((value != nullptr) && (value->anchor != node))
            {
                immutable = true;
                value = nullptr;
            }

            while (value != nullptr)
            {
                auto next = value->next;
                destroy_value_node(value);
                value = next;
            }

            if (!immutable)
            {
                (*it).head_leftmost = nullptr;
                (*it).tail_rightmost = nullptr;
            }
        }
    }
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::value_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::disconnect_values(
        structure_node_type* node, const secondary_key_type& key)
{
    value_node_type* head = nullptr;

    if (node != nullptr)
    {
        auto bounds = node->store.retrieve(key);

        if (bounds.second)
        {
            bool immutable = false;
            auto value = (*bounds.first).head_leftmost;

            // Restrict to removing only anchored values, rely on the fact
            // that there cannot be a mix of anchored and unanchored values.
            if ((value != nullptr) && (value->anchor != node))
            {
                immutable = true;
                value = nullptr;
            }

            head = value;

            if (!immutable)
            {
                (*bounds.first).head_leftmost = nullptr;
                (*bounds.first).tail_rightmost = nullptr;
            }
        }
    }

    return head;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::erase_disconnected_list(value_node_type* head)
{
    if (head != nullptr)
    {
        auto value = head;

        while (value != nullptr)
        {
            auto next = value->next;
            destroy_value_node(value);
            value = next;
        }
    }
}

//template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
//void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::erase_values(
//    structure_node_type* node, const secondary_key_type& key)
//{
//    if (node != nullptr)
//    {
//        auto bounds = node->store.retrieve(key);
//
//        if (bounds.second)
//        {
//            auto value = (*bounds.first).head_leftmost;
//
//            // Restrict to removing only anchored values, rely on the fact
//            // that there cannot be a mix of anchored and unanchored values.
//            if ((value != nullptr) && (value->anchor != node))
//                value = nullptr;
//
//            while (value != nullptr)
//            {
//                auto next = value->next;
//                destroy_value_node(value);
//                value = next;
//            }
//
//            (*bounds.first).head_leftmost = nullptr;
//            (*bounds.first).tail_rightmost = nullptr;
//        }
//    }
//}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::compress_branch(
        structure_node_type* node, const secondary_key_type& key)
{
    if (node != nullptr)
    {
        auto update = true;

        // while not the trie root (designated by null parent), having no value
        // and having no children, delete leaf and replace reference with its parent
        while ((node->parent != nullptr) && !node->has_value())
        {
            auto parent = node->parent;
            if (node->has_children())
            {
                // Can we collapse this node out of the trie before termination?
                if (node->get_first_child() == node->get_last_child())
                {
                    auto replacement = node->get_last_child();
                    replacement->label.prepend(node->label);
                    attach_child(parent, replacement);
                    destroy_structure_node(node);
                    node = replacement;
                    update = false; // was triggered by attach_child
                }

                break;
            }

            // remove child reference within parent, delete leaf
            parent->set_child(node->label[0], nullptr);
            destroy_structure_node(node);
            node = parent;
        }

        if (update)
            update_left_and_right_branch(node, key);
    }

    return node;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::value_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::create_value_node(
        const value_type& value)
{
    auto* node = value_allocator_.allocate(1);
    if (node == nullptr)
        throw std::bad_alloc();

    new (node)value_node_type(value);
    return node;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
bool modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::destroy_value_node(
    value_node_type* node)
{
    auto result = false;
    if (node != nullptr)
    {
        value_allocator_.destroy(node);
        value_allocator_.deallocate(node, 1);
        result = true;
    }

    return result;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::value_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::append_value(
        structure_node_type* node, const secondary_key_type& key,
        value_node_type* value_node)
{
    auto update = false;
    auto bounds_query = node->store.retrieve(key);

    if (!bounds_query.second)
        bounds_query = node->store.add(key, { nullptr, nullptr }, true);

    value_node->anchor = node;
    auto* next = (*bounds_query.first).head_leftmost;
    auto* previous = (*bounds_query.first).tail_rightmost;

    while ((previous != nullptr) &&
        !value_comparer_(value_node->value, previous->value))
        previous = previous->previous;

    if (previous != nullptr)
    {
        next = previous->next;
        previous->next = value_node;
        value_node->previous = previous;
    }

    if (previous == (*bounds_query.first).tail_rightmost)
    {
        update = true;
        (*bounds_query.first).tail_rightmost = value_node;
    }

    if (previous == nullptr)
    {
        update = true;
        (*bounds_query.first).head_leftmost = value_node;
    }

    value_node->next = next;

    if (next != nullptr)
        next->previous = value_node;

    // fixup left/right pointers
    if (update)
        update_left_and_right_branch(node, key);

    return value_node;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::value_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::append_value(
        structure_node_type* node, const secondary_key_type& key,
        const value_type& value)
{
    auto* value_node = create_value_node(value);
    return append_value(node, key, value_node);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::get_leftmost_leaf(
        structure_node_type* origin) const
{
    auto* current = origin;
    while (current->has_children())
        current = current->get_first_child();

    return current;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::update_left_and_right(
    structure_node_type* node, const secondary_key_type& key)
{
    if (!node->has_children(key))
    {
        auto result = node->store.retrieve(key);

        if (result.second)
        {
//            (*result.first).leftmost = (*result.first).head;
//            (*result.first).rightmost = (*result.first).tail;

            // Assumes reachable values anchored to other nodes are not deallocated.
            if (((*result.first).head_leftmost == nullptr) ||
                ((*result.first).head_leftmost->anchor != node))
                node->store.remove(key);
        }
    }
    else
    {
        auto key_bounds = node->store.retrieve(key);

        if (!key_bounds.second)
            key_bounds = node->store.add(key,
                typename structure_node_type::value_boundaries());

        // Note: assumes no catestrophic failure in node->store.add as no check
        // to key_bounds.second is performed or exception thrown.
        auto first_child_bounds = node->get_first_child(key)->store.retrieve(key);
        (*key_bounds.first).head_leftmost = (*first_child_bounds.first).head_leftmost;

        auto last_child_bounds = node->get_last_child(key)->store.retrieve(key);
        (*key_bounds.first).tail_rightmost = (*last_child_bounds.first).tail_rightmost;
    }
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::update_left_and_right_branch(
    structure_node_type* node, const secondary_key_type& key)
{
    if (node != nullptr)
    {
        // fixup left/right pointers
        auto temp = node;
        while (temp != nullptr)
        {
            update_left_and_right(temp, key);
            temp = temp->parent;
        }
    }
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::attach_child(
    const secondary_key_type& key, structure_node_type* parent,
    structure_node_type* child)
{
    // note: method trusts that the parent's child is safely overwriteable
    child->parent = parent;
    parent->set_child(child->label[0], child);
    if (child->has_value(key))
        update_left_and_right_branch(child, key);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
void modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::attach_child(
    structure_node_type* parent, structure_node_type* child)
{
    // note: method trusts that the parent's child is safely overwriteable
    child->parent = parent;
    parent->set_child(child->label[0], child);

    for (auto it = child->store.begin(); it != child->store.end(); ++it)
        update_left_and_right_branch(child, it.get_key());
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::insert_at(
        structure_node_type* current, const binary_type& key)
{
    auto host = create_structure_node(key);
    attach_child(current, host);
    return host;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::structure_node_type*
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::insert_at(
        structure_node_type* current, const binary_type& primary,
        const secondary_key_type& secondary, const value_type& value)
{
    auto host = create_structure_node(primary, secondary, value);
    attach_child(secondary, current, host);
    return host;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::pair_iterator_bool
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::insert(
        structure_node_type*& current, const binary_type& primary,
        const secondary_key_type& secondary, const value_type& value)
{
    binary_type::size_type key_offset = 0;
    for (; key_offset < primary.size();)
    {
        auto initial_match = current->get_child(primary[key_offset]);
        if (initial_match == nullptr)
        {
            auto subkey = primary.substring(key_offset,
                primary.size() - key_offset);
            auto node = insert_at(current, subkey, secondary, value);
            auto value_bounds = node->store.retrieve(secondary);

            return std::make_pair(
                iterator(secondary, (*(value_bounds.first)).head_leftmost),
                true);
        }

        // scan for substring mismatch with label, note scan begins at
        // second character as first character was redundantly encoded
        // and checked during child choice for the loop's examination
        bool matches_label = true;
        bool initial_match_label_exceeds_key = false;
        binary_type::size_type label_offset = 1;

        for (; (label_offset < initial_match->label.size()); ++label_offset)
        {
            if ((key_offset + label_offset) >= primary.size())
            {
                initial_match_label_exceeds_key = true;
                break;
            }

            if (initial_match->label[label_offset] !=
                primary[key_offset + label_offset])
            {
                matches_label = false;
                break;
            }
        }

        if (!matches_label || initial_match_label_exceeds_key
            || (label_offset != initial_match->label.size()))
        {
            // if there is a disagreement, introduce intermediary node
            // and insert the new branch
            auto intermediary_key = initial_match->label.substring(0,
                label_offset);

            auto trailing_initial_key = initial_match->label.substring(
                label_offset);

            // unlink/remove the initial_match from the tree
            initial_match->parent = nullptr;

            // add intermediary to tree
            auto intermediary = insert_at(current, intermediary_key);

            // add back initial_match with reduced label
            initial_match->label = trailing_initial_key;
            attach_child(intermediary, initial_match);

            if (primary.size() > (key_offset + label_offset))
            {
                // if the key has remaining length, insert a sibling
                auto remaining_key = primary.substring(
                    key_offset + label_offset);

                return std::make_pair(iterator(secondary, insert_at(
                    intermediary, remaining_key, secondary, value)), true);
            }
            else
            {
                // Note: Due to the introduction of universal Size restrictions
                // on primary keys, this code should be unreachable.

                // otherwise intermediary label must be key, so add value to
                // the intermediary which was uniquely added and link_node
                auto* inserted = append_value(intermediary, secondary, value);
                current = intermediary;
                return std::make_pair(iterator(secondary, inserted), true);
            }
        }
        else
        {
            // otherwise, loop decending the tree
            current = initial_match;
            key_offset += label_offset;
        }
    }

    return std::make_pair(iterator(secondary, current), false);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::pair_iterator_bool
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::insert_equal(
        const binary_type& primary, const secondary_key_type& secondary,
        const value_type& value)
{
    auto current = root_;

    // prevent insertion with primary key longer than size template parameter
    if (primary.size() != S)
        return std::make_pair(iterator(secondary, current), false);

    auto result = insert(current, primary, secondary, value);

    // if we haven't created a node yet, but exhausted our key
    // we must have matched an existing node, we can append here
    // though we aren't guaranteed to be unique
    if (!result.second && (current != root_))
    {
        auto inserted = append_value(current, secondary, value);
        return std::make_pair(iterator(secondary, inserted), true);
    }

    return result;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::pair_iterator_bool
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::insert_unique(
        const binary_type& primary, const secondary_key_type& secondary,
        const value_type& value)
{
    auto current = root_;

    // prevent insertion with primary key longer than size template parameter
    if (primary.size() != S)
        return std::make_pair(iterator(secondary, current), false);

    auto result = insert(current, primary, secondary, value);

    // Note: Due to the introduction of the Size template parameter as a
    // universal key length constraint, it becomes impossible to have such a
    // matching intermediary node.

    // if we haven't created a node yet, but exhausted our key
    // we must have matched an existing node, if it has no value
    // we can introduce one here
    if (!result.second && !current->has_value(secondary) && (current != root_))
    {
        auto inserted = append_value(current, secondary, value);
        return std::make_pair(iterator(secondary, inserted), true);
    }

    return result;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::pair_node_size
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::find_closest_subkey_matching_node(
        structure_node_type* start, const binary_type& key) const
{
    auto match = true;
    auto current = start;
    binary_type::size_type key_offset = 0;
    for (; match && (current != nullptr) && (key_offset < key.size());)
    {
        auto initial_match = current->get_child(key[key_offset]);
        if (initial_match != nullptr)
        {
            for (binary_type::size_type label_offset = 0;
                (label_offset < initial_match->label.size()) &&
                    (key_offset + label_offset < key.size()); ++label_offset)
            {
                if (key[key_offset + label_offset]
                    != initial_match->label[label_offset])
                {
                    match = false;
                    break;
                }
            }

            key_offset += initial_match->label.size();
        }

        current = initial_match;
    }

    if (!match)
    {
        current = nullptr;
    }

    return std::make_pair(current, key_offset);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::pair_query_result_bool
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::find_equal(
        const binary_type& primary) const
{
    auto find_pair = find_closest_subkey_matching_node(root_, primary);
    bool failure =  (find_pair.second == 0) || (find_pair.first == nullptr) ||
        (find_pair.second != primary.size());

    return std::make_pair(query_result(find_pair.first), !failure);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::iterator_range
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::find_equal(
        const binary_type& primary, const secondary_key_type& secondary) const
{
    auto primary_key_query_result = find_equal(primary);

    if (primary_key_query_result.second)
        return primary_key_query_result.first.get_exact(secondary);

    return std::make_pair(end_, end_);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::pair_query_result_bool
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::find_prefixed(
        const binary_type& primary) const
{
    auto find_pair = find_closest_subkey_matching_node(root_, primary);
    bool failure =  (find_pair.second == 0) || (find_pair.first == nullptr);

    return std::make_pair(query_result(find_pair.first), !failure);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::iterator_range
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::find_prefixed(
        const binary_type& primary, const secondary_key_type& secondary) const
{
    auto primary_key_query_result = find_prefixed(primary);

    if (primary_key_query_result.second)
        return primary_key_query_result.first.get_prefixed(secondary);

    return std::make_pair(end_, end_);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::iterator_range
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::find_secondary_key_bounds(
        const secondary_key_type& key) const
{
    auto result = query_result(root_);
    return result.get_prefixed(key);
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
bool modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::remove_equal(
    const binary_type& primary, const secondary_key_type& secondary)
{
    auto find_pair = find_closest_subkey_matching_node(root_, primary);
    auto nonremovable = ((find_pair.first == nullptr) ||
        (find_pair.second != primary.size()) || (find_pair.second == 0));

    if (!nonremovable)
    {
        auto node = find_pair.first;
        auto disconnected = disconnect_values(node, secondary);
        compress_branch(node, secondary);
        erase_disconnected_list(disconnected);
    }

    return !nonremovable;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
bool modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::remove_equal(
    const secondary_key_type& secondary)
{
    bool removed = false;
    query_result prefix_query(root_);
    auto value_bounds = prefix_query.get_prefixed(secondary);
    auto begin = value_bounds.first.iterator_;
    auto end = value_bounds.second.iterator_;

    while (begin != end)
    {
        auto node = begin.trie_node_;
        ++begin;
        auto disconnected = disconnect_values(node, secondary);
        compress_branch(node, secondary);
        erase_disconnected_list(disconnected);
        removed = true;
    }

    return removed;
}

template <std::size_t S, typename K, typename V, typename KC, typename VC, typename SNA, typename VNA>
typename modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::iterator
    modified_patricia_trie<S, K, V, KC, VC, SNA, VNA>::remove_value(iterator it)
{
    auto value_node = it.value_node_;
    if (value_node == nullptr)
        return it;

    auto secondary_key = it.get_secondary_key();
    auto anchor = value_node->anchor;
    auto anchor_key_bounds = anchor->store.retrieve(secondary_key);

    auto struct_iter = it.get_structure_iterator();
    ++struct_iter;

    if ((*anchor_key_bounds.first).head_leftmost == (*anchor_key_bounds.first).tail_rightmost)
    {
        auto next = struct_iter.trie_node_;

        // otherwise, remove all values and attempt to remove the node
        auto disconnected = disconnect_values(anchor, secondary_key);
        compress_branch(anchor, secondary_key);
        erase_disconnected_list(disconnected);

        return (iterator)(next);
    }

    // if the value can be removed without removing emptying the node
    auto next = value_node->next;
    auto previous = value_node->previous;

    if (next != nullptr)
        next->previous = previous;

    if (previous != nullptr)
        previous->next = next;

    auto update = false;
    value_node->next = nullptr;
    value_node->previous = nullptr;

    if ((*anchor_key_bounds.first).head_leftmost == value_node)
    {
        (*anchor_key_bounds.first).head_leftmost = next;
        update = true;
    }

    if ((*anchor_key_bounds.first).tail_rightmost == value_node)
    {
        (*anchor_key_bounds.first).tail_rightmost = previous;
        update = true;
    }

    destroy_value_node(value_node);

    if (update)
        update_left_and_right_branch(anchor, secondary_key);

    return (next == nullptr) ?
        iterator(secondary_key, struct_iter.trie_node_) : iterator(secondary_key, next);
}

} // namespace blockchain
} // namespace libbitcoin

#endif
