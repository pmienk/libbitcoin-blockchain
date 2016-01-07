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

#ifndef LIBBITCOIN_BLOCKCHAIN_PREFIX_QUERY_RESULT_IPP
#define LIBBITCOIN_BLOCKCHAIN_PREFIX_QUERY_RESULT_IPP

namespace libbitcoin {
namespace blockchain {

// prefix_query_result implementation
template <typename Key, typename Value, typename Comparer>
prefix_query_result<Key, Value, Comparer>::prefix_query_result()
    : node_(nullptr), terminal_()
{
}

template <typename Key, typename Value, typename Comparer>
prefix_query_result<Key, Value, Comparer>::prefix_query_result(
    structure_node_type* node)
    : node_(node), terminal_()
{
}

template <typename Key, typename Value, typename Comparer>
typename prefix_query_result<Key, Value, Comparer>::iterator_range
    prefix_query_result<Key, Value, Comparer>::get_exact(
        const secondary_key_type& key) const
{
    if (node_ == nullptr)
        return std::make_pair(terminal_, terminal_);

    auto result = node_->store.retrieve(key);

    if (!result.second)
        return std::make_pair(terminal_, terminal_);

    auto head = (*result.first).head_leftmost;
    auto tail = (*result.first).tail_rightmost;

    if ((head != nullptr) && (head->anchor != node_))
    {
        head = nullptr;
        tail = nullptr;
    }

    auto begin = iterator(key, head);
    auto end = iterator(key, tail);
    if (tail != nullptr)
        ++end;

    return std::make_pair(begin, end);
}

template <typename Key, typename Value, typename Comparer>
typename prefix_query_result<Key, Value, Comparer>::iterator_range
    prefix_query_result<Key, Value, Comparer>::get_exact_greater_equal(
        const secondary_key_type& key) const
{
    if (node_ == nullptr)
        return std::make_pair(terminal_, terminal_);

    auto result = node_->store.retrieve_greater_equal(key);

    if (!result.second)
        return std::make_pair(terminal_, terminal_);

    auto head = (*result.first).head_leftmost;
    auto tail = (*result.first).tail_rightmost;

    if ((head != nullptr) && (head->anchor != node_))
    {
        head = nullptr;
        tail = nullptr;
    }

    auto begin = iterator(key, head);
    auto end = iterator(key, tail);
    if (tail != nullptr)
        ++end;

    return std::make_pair(begin, end);
}

template <typename Key, typename Value, typename Comparer>
typename prefix_query_result<Key, Value, Comparer>::iterator_range
    prefix_query_result<Key, Value, Comparer>::get_prefixed(
        const secondary_key_type& key) const
{
    if (node_ == nullptr)
        return std::make_pair(terminal_, terminal_);

    auto result = node_->store.retrieve(key);

    if (!result.second)
        return std::make_pair(terminal_, terminal_);

    auto begin = iterator(key, (*result.first).head_leftmost);
    auto end = iterator(key, (*result.first).tail_rightmost);

    if ((*result.first).tail_rightmost != nullptr)
        ++end;

    return std::make_pair(begin, end);
}

template <typename Key, typename Value, typename Comparer>
typename prefix_query_result<Key, Value, Comparer>::iterator_range
    prefix_query_result<Key, Value, Comparer>::get_prefixed_greater_equal(
        const secondary_key_type& key) const
{
    if (node_ == nullptr)
        return std::make_pair(terminal_, terminal_);

    auto result = node_->store.retrieve_greater_equal(key);

    if (!result.second)
        return std::make_pair(terminal_, terminal_);

    auto begin = iterator(key, (*result.first).head_leftmost);
    auto end = iterator(key, (*result.first).tail_rightmost);

    if ((*result.first).tail_rightmost != nullptr)
        ++end;

    return std::make_pair(begin, end);
}

} // namespace blockchain
} // namespace libbitcoin

#endif
