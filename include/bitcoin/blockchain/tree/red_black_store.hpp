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

#ifndef LIBBITCOIN_BLOCKCHAIN_RED_BLACK_STORE_HPP
#define LIBBITCOIN_BLOCKCHAIN_RED_BLACK_STORE_HPP

#include <boost/utility.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/tree/key_value_store.hpp>
#include <bitcoin/blockchain/tree/red_black_tree.hpp>

namespace libbitcoin {
namespace blockchain {

template<typename Key,
    typename Value,
    typename Comparer = std::less<Key>,
    typename Allocator = std::allocator<red_black_node<Key, Value>>>
class red_black_store : public key_value_store<Key, Value, Comparer>,
    private boost::noncopyable
{
public:

    typedef Key key_type;
    typedef Value value_type;
    typedef Allocator node_allocator;
    typedef red_black_tree<Key, Value, Comparer, Allocator> tree_type;
    typedef std::pair<value_type, bool> pair_value_bool;

public:

    red_black_store(node_allocator allocator);
    red_black_store(std::shared_ptr<tree_type> tree);
    ~red_black_store();

    bool add(key_type key, value_type value, bool replace = false);
    bool remove(key_type key);
    pair_value_bool retrieve(key_type key);

private:

    std::shared_ptr<tree_type> tree_;
};

} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/tree/red_black_store.ipp>

#endif
