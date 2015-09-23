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

#ifndef LIBBITCOIN_BLOCKCHAIN_RED_BLACK_STORE_IPP
#define LIBBITCOIN_BLOCKCHAIN_RED_BLACK_STORE_IPP

namespace libbitcoin {
namespace blockchain {

template<typename Key, typename Value, typename Comparer, typename Allocator>
red_black_store<Key, Value, Comparer, Allocator>::red_black_store(
    node_allocator allocator)
    : tree_(new tree_type(allocator))
{
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
red_black_store<Key, Value, Comparer, Allocator>::red_black_store(
    tree_type* tree)
    : tree_(tree)
{
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
red_black_store<Key, Value, Comparer, Allocator>::~red_black_store()
{
    if (tree_ != nullptr)
        delete tree_;

    tree_ = nullptr;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::pair_iterator_bool
    red_black_store<Key, Value, Comparer, Allocator>::add(const key_type& key,
        value_type value, bool replace)
{
    auto node = tree_->retrieve(key);

    if (node != tree_->nil())
    {
        if (replace)
            node->value = value;

        return std::make_pair(iterator(tree_, node), replace);
    }

    // created as red node with no children
    node = tree_->create_node(key, value);

    tree_->add(node);

    return std::make_pair(iterator(tree_, node), true);
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
bool red_black_store<Key, Value, Comparer, Allocator>::remove(
    const key_type& key)
{
    bool result = false;
    auto node = tree_->retrieve(key);

    if (node != tree_->nil())
    {
        tree_->remove(node);
        tree_->destroy_node(node);
        result = true;
    }

    return result;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::pair_iterator_bool
    red_black_store<Key, Value, Comparer, Allocator>::retrieve(
        const key_type& key) const
{
    auto node = tree_->retrieve(key);
    return std::make_pair(iterator(tree_, node), (node != tree_->nil()));
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::pair_iterator_bool
    red_black_store<Key, Value, Comparer, Allocator>::retrieve_greater_equal(
        const key_type& key) const
{
    auto node = tree_->retrieve_greater_equal(key);
    return std::make_pair(iterator(tree_, node), (node != tree_->nil()));
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::iterator
    red_black_store<Key, Value, Comparer, Allocator>::begin()
{
    return iterator(tree_, tree_->minimum());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::iterator
    red_black_store<Key, Value, Comparer, Allocator>::end()
{
    return iterator(tree_, tree_->nil());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_iterator
    red_black_store<Key, Value, Comparer, Allocator>::begin() const
{
    return const_iterator(tree_, tree_->minimum());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_iterator
    red_black_store<Key, Value, Comparer, Allocator>::end() const
{
    return const_iterator(tree_, tree_->nil());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_iterator
    red_black_store<Key, Value, Comparer, Allocator>::cbegin() const
{
    return begin();
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_iterator
    red_black_store<Key, Value, Comparer, Allocator>::cend() const
{
    return end();
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::reverse_iterator
    red_black_store<Key, Value, Comparer, Allocator>::rbegin()
{
    return static_cast<reverse_iterator>(end());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::reverse_iterator
    red_black_store<Key, Value, Comparer, Allocator>::rend()
{
    return static_cast<reverse_iterator>(begin());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_reverse_iterator
    red_black_store<Key, Value, Comparer, Allocator>::rbegin() const
{
    return static_cast<const_reverse_iterator>(end());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_reverse_iterator
    red_black_store<Key, Value, Comparer, Allocator>::rend() const
{
    return static_cast<const_reverse_iterator>(begin());
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_reverse_iterator
    red_black_store<Key, Value, Comparer, Allocator>::crbegin() const
{
    return rbegin();
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::const_reverse_iterator
    red_black_store<Key, Value, Comparer, Allocator>::crend() const
{
    return rend();
}

} // namespace blockchain
} // namespace libbitcoin

#endif
