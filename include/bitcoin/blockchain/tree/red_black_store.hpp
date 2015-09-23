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
#include <bitcoin/blockchain/tree/red_black_iterator.hpp>
#include <bitcoin/blockchain/tree/red_black_reverse_iterator.hpp>
#include <bitcoin/blockchain/tree/red_black_tree.hpp>

namespace libbitcoin {
namespace blockchain {

template<typename Key,
    typename Value,
    typename Comparer = std::greater<Key>,
    typename Allocator = std::allocator<red_black_node<Key, Value>>>
class red_black_store : private boost::noncopyable
{
public:

    typedef Key key_type;
    typedef Value value_type;
    typedef Allocator node_allocator;
    typedef red_black_tree<Key, Value, Comparer, Allocator> tree_type;

    typedef red_black_iterator<Key, Value, Value&, Value*> iterator;
    typedef typename iterator::const_iterator const_iterator;
    typedef red_black_reverse_iterator<key_type, iterator> reverse_iterator;
    typedef red_black_reverse_iterator<key_type, const_iterator> const_reverse_iterator;

    typedef std::pair<iterator, bool> pair_iterator_bool;

public:

    red_black_store(node_allocator allocator);
    red_black_store(tree_type* tree);
    ~red_black_store();

    pair_iterator_bool add(const key_type& key, value_type value = value_type(),
        bool replace = false);
    bool remove(const key_type& key);
    pair_iterator_bool retrieve(const key_type& key) const;
    pair_iterator_bool retrieve_greater_equal(const key_type& key) const;

    // iterators
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

private:

    tree_type* tree_;
};

} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/tree/red_black_store.ipp>

#endif
