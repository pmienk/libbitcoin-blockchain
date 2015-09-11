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

#ifndef LIBBITCOIN_BLOCKCHAIN_RED_BLACK_TREE_HPP
#define LIBBITCOIN_BLOCKCHAIN_RED_BLACK_TREE_HPP

#include <boost/utility.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/tree/red_black_node.hpp>

namespace libbitcoin {
namespace blockchain {

template<typename Key,
    typename Value,
    typename Comparer = std::greater<Key>,
    typename Allocator = std::allocator<red_black_node<Key, Value>>>
class red_black_tree : private boost::noncopyable
{
public:

    typedef Key key_type;
    typedef Value value_type;
    typedef Comparer key_type_comparer;
    typedef Allocator node_allocator;
    typedef red_black_node<key_type, value_type> node_type;
    typedef std::pair<value_type, bool> pair_value_bool;

public:

    // constructor/destructors
    red_black_tree(node_allocator allocator);

    ~red_black_tree();

    // node variants of exposed functions
    void add(node_type* node);
    void remove(node_type* node);
    node_type* retrieve(key_type key);
    node_type* retrieve_greater_equal(key_type key);

    const node_type* root() const;
    const node_type* nil() const;

    node_type* create_node(const key_type& key, const value_type& value);
    bool destroy_node(node_type* node);

private:

    void destroy();

    // node management
    node_type* create_node();
    void destroy_subtree(node_type* node);

    // tree manipulation
    void balance_add(node_type* node);
    void balance_remove(node_type* node);
    node_type* leftmost_leaf(node_type* node);
    void transplant(node_type* alpha, node_type* beta);
    node_type* tree_minimum(node_type* node);
    void rotate_left(node_type* alpha);
    void rotate_right(node_type* alpha);

    node_allocator allocator_;
    key_type_comparer comparer_;
    node_type* nil_;
    node_type* root_;
};

} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/tree/red_black_tree.ipp>

#endif
