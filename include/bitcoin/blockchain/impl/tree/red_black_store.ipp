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
    std::shared_ptr<tree_type> tree)
    : tree_(tree)
{
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
red_black_store<Key, Value, Comparer, Allocator>::~red_black_store()
{
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
bool red_black_store<Key, Value, Comparer, Allocator>::add(key_type key,
    value_type value, bool replace)
{
    auto node = tree_->retrieve(key);

    if (node != tree_->nil())
    {
        if (replace)
            node->value = value;

        return replace;
    }

    // created as red node with no children
    node = tree_->create_node(key, value);

    tree_->add(node);

    return true;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
bool red_black_store<Key, Value, Comparer, Allocator>::remove(key_type key)
{
    bool result = false;
    auto node = tree_->retrieve(key);

    if (node != tree_->nil())
    {
        tree_->remove(node);
        tree_->destroy(node);
        result = true;
    }

    return result;
}

template<typename Key, typename Value, typename Comparer, typename Allocator>
typename red_black_store<Key, Value, Comparer, Allocator>::pair_value_bool
    red_black_store<Key, Value, Comparer, Allocator>::retrieve(key_type key)
{
    auto node = tree_->retrieve(key);

    return std::make_pair((node != tree_->nil()) ? node->key : key_type(),
        (node != tree_->nil()));
}

} // namespace blockchain
} // namespace libbitcoin

#endif
