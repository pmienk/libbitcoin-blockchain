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

#ifndef LIBBITCOIN_BLOCKCHAIN_RED_BLACK_NODE_HPP
#define LIBBITCOIN_BLOCKCHAIN_RED_BLACK_NODE_HPP

#include <boost/utility.hpp>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace blockchain {

template <typename Key, typename Value>
class red_black_node : private boost::noncopyable
{
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef red_black_node<key_type, value_type> node_type;

    explicit red_black_node();

    key_type key;
    value_type value;
    bool is_red;
    node_type* left;
    node_type* right;
    node_type* parent;
};

} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/tree/red_black_node.ipp>

#endif
