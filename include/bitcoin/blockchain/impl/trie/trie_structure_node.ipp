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

#ifndef LIBBITCOIN_BLOCKCHAIN_TRIE_STRUCTURE_NODE_IPP
#define LIBBITCOIN_BLOCKCHAIN_TRIE_STRUCTURE_NODE_IPP

namespace libbitcoin {
namespace blockchain {

// trie_node implenentation
template<typename Key, typename Node, typename Comparer>
trie_structure_node<Key, Node, Comparer>::trie_structure_node()
: label(), parent(nullptr), store(red_black_allocator_type())
{
    children_[0] = nullptr;
    children_[1] = nullptr;
}

template<typename Key, typename Node, typename Comparer>
trie_structure_node<Key, Node, Comparer>::trie_structure_node(
    red_black_allocator_type allocator)
    : label(), parent(nullptr), store(allocator)
{
    children_[0] = nullptr;
    children_[1] = nullptr;
}

template<typename Key, typename Node, typename Comparer>
bool trie_structure_node<Key, Node, Comparer>::has_children() const
{
    return (children_[0] != nullptr) || (children_[1] != nullptr);
}

template<typename Key, typename Node, typename Comparer>
bool trie_structure_node<Key, Node, Comparer>::has_children(const key_type& key) const
{
    bool result = false;

    if (children_[0] != nullptr)
        result = children_[0]->store.retrieve(key).second;

    if (!result && (children_[1] != nullptr))
        result = children_[1]->store.retrieve(key).second;

    return result;
}

template<typename Key, typename Node, typename Comparer>
bool trie_structure_node<Key, Node, Comparer>::has_value() const
{
    bool result = false;

    for (auto it = store.begin(); !result && (it != store.end()); ++it)
        result = (((*it).head_leftmost != nullptr) &&
            ((*it).head_leftmost->anchor == this));

    return result;
}

template<typename Key, typename Node, typename Comparer>
bool trie_structure_node<Key, Node, Comparer>::has_value(const key_type& key) const
{
    bool result = false;
    auto store_result = store.retrieve(key);

    if (store_result.second)
        result = ((*store_result.first).head_leftmost != nullptr) &&
            ((*store_result.first).head_leftmost->anchor == this);

    return result;
}

template<typename Key, typename Node, typename Comparer>
typename trie_structure_node<Key, Node, Comparer>::structure_node_type*
    trie_structure_node<Key, Node, Comparer>::get_child(
        bool matching_bit) const
{
    return matching_bit ? children_[1] : children_[0];
}

template<typename Key, typename Node, typename Comparer>
void trie_structure_node<Key, Node, Comparer>::set_child(
    bool matching_bit, structure_node_type* node)
{
    int index = matching_bit ? 1 : 0;
    children_[index] = node;
}

template<typename Key, typename Node, typename Comparer>
typename trie_structure_node<Key, Node, Comparer>::structure_node_type*
    trie_structure_node<Key, Node, Comparer>::get_first_child() const
{
    auto result = children_[0];

    if (result == nullptr)
    {
        result = children_[1];
    }

    return result;
}

template<typename Key, typename Node, typename Comparer>
typename trie_structure_node<Key, Node, Comparer>::structure_node_type*
    trie_structure_node<Key, Node, Comparer>::get_first_child(
        const key_type& key) const
{
    structure_node_type* result = children_[0];

    if ((result == nullptr) || !(result->store.retrieve(key).second))
    {
        result = children_[1];

        if ((result == nullptr) || !(result->store.retrieve(key).second))
            result = nullptr;
    }

    return result;
}

template<typename Key, typename Node, typename Comparer>
typename trie_structure_node<Key, Node, Comparer>::structure_node_type*
    trie_structure_node<Key, Node, Comparer>::get_last_child() const
{
    auto result = children_[1];

    if (result == nullptr)
    {
        result = children_[0];
    }

    return result;
}

template<typename Key, typename Node, typename Comparer>
typename trie_structure_node<Key, Node, Comparer>::structure_node_type*
    trie_structure_node<Key, Node, Comparer>::get_last_child(
        const key_type& key) const
{
    auto result = children_[1];

    if ((result == nullptr) || !(result->store.retrieve(key).second))
    {
        result = children_[0];

        if ((result == nullptr) || !(result->store.retrieve(key).second))
            result = nullptr;
    }

    return result;
}

template<typename Key, typename Node, typename Comparer>
bool trie_structure_node<Key, Node, Comparer>::remove_child(
    structure_node_type* child)
{
    bool removed = true;

    if (children_[0] == child)
        children_[0] = nullptr;
    else if (children_[1] == child)
        children_[1] = nullptr;
    else
        removed = false;

    return removed;
}

} // namespace blockchain
} // namespace libbitcoin

#endif
