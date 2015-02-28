/*
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

#ifndef LIBBITCOIN_BLOCKCHAIN_BINARY_TRIE_HPP
#define LIBBITCOIN_BLOCKCHAIN_BINARY_TRIE_HPP

#include <bitcoin/bitcoin.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/utility.hpp>
#include <memory>

namespace libbitcoin {
namespace chain {

template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
class binary_trie;

template <typename Value>
struct trie_node;


template <typename Value>
struct value_list_node : protected boost::noncopyable
{
public:

    typedef Value value_type;
    typedef trie_node<value_type> trie_node_type;
    typedef value_list_node<value_type> node_type;

    trie_node_type* anchor;
    node_type* next;
    node_type* previous;
    value_type value;

public:

    explicit value_list_node();

    explicit value_list_node(const value_type& x);
};


template <typename Value>
struct trie_node : private boost::noncopyable
{
public:

    typedef Value value_type;
    typedef trie_node<value_type> node_type;
    typedef value_list_node<value_type> value_list_type;

    node_type* parent;
    node_type* children[2];

    value_list_type* value_head;
    value_list_type* value_tail;

    value_list_type* value_leftmost;
    value_list_type* value_rightmost;

    node_type* previous;
    node_type* next;

    binary_type label;

public:

    explicit trie_node();

    node_type* get_first_child() const;

    node_type* get_last_child() const;

    bool has_children() const;

    bool has_value() const;
};


template <typename Value, typename Reference, typename Pointer>
struct trie_iterator
{
    template<class V, class AllocN, class AllocV> friend class binary_trie;

public:
    // iterator_traits required typedefs
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Value value_type;

    typedef trie_iterator<Value, Reference, Pointer> iter_type;
    typedef trie_iterator<Value, Value&, Value*> iterator;
    typedef trie_iterator<Value, const Value&, const Value*> const_iterator;
    typedef trie_node<Value> trie_node_type;
    typedef value_list_node<Value> value_node_type;

public:

    // constructors
    explicit trie_iterator();

    trie_iterator(trie_node_type* node);

    trie_iterator(value_node_type* value);

    trie_iterator(trie_node_type* node, value_node_type* value);

    trie_iterator(const iterator& it);

    binary_type get_key();

    // iterator methods
    reference operator*() const;

    pointer operator->() const;

    bool operator==(const trie_iterator& other) const;

    bool operator!=(const trie_iterator& other) const;

    iter_type& operator++();

    iter_type operator++(int);

    iter_type& operator--();

    iter_type operator--(int);

protected:
    // increment/decrement implementation
    void trie_node_increment();

    void trie_node_decrement();

    void increment();

    void decrement();

private:

    trie_node_type* trie_node_;
    value_node_type* value_node_;
};


template <typename TrieIterator>
struct trie_reverse_iterator
{
public:
    // iterator_traits required typedefs
    typedef typename std::iterator_traits<TrieIterator>::difference_type differcent_type;
    typedef typename std::iterator_traits<TrieIterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<TrieIterator>::pointer pointer;
    typedef typename std::iterator_traits<TrieIterator>::reference reference;
    typedef typename std::iterator_traits<TrieIterator>::value_type value_type;

    typedef TrieIterator base_iter_type;
    typedef trie_reverse_iterator<base_iter_type> iter_type;

public:

    // constructors
    trie_reverse_iterator();

    explicit trie_reverse_iterator(base_iter_type it);

    trie_reverse_iterator(const iter_type& other);

    template <typename Iter>
    trie_reverse_iterator(const trie_reverse_iterator<Iter> &other);

    // iterator methods
    reference operator*() const;

    pointer operator->() const;

    bool operator==(const iter_type& other) const;

    bool operator!=(const iter_type& other) const;

    iter_type& operator++();

    iter_type operator++(int);

    iter_type& operator--();

    iter_type operator--(int);

    // accessors
    base_iter_type base() const;

    binary_type get_key();

private:

    base_iter_type base_iterator_;
};


template<typename Value, typename NodeAlloc, typename ValueNodeAlloc>
class binary_trie: protected boost::noncopyable
{
public:

    typedef Value value_type;
    typedef NodeAlloc trie_node_allocator;
    typedef ValueNodeAlloc value_list_node_allocator;
    typedef binary_trie<value_type, trie_node_allocator, value_list_node_allocator> binary_trie_type;
    typedef trie_node<value_type> trie_node_type;
    typedef value_list_node<value_type> value_node_type;

    typedef trie_iterator<Value, Value&, Value*> iterator;
    typedef typename iterator::const_iterator const_iterator;
    typedef trie_reverse_iterator<iterator> reverse_iterator;
    typedef trie_reverse_iterator<const_iterator> const_reverse_iterator;

    typedef std::pair<iterator, bool> pair_iterator_bool;
    typedef std::pair<iterator, iterator> iterator_range;

    typedef std::pair<trie_node_type*, binary_type::size_type> pair_node_size;

public:

    // constructor/destructors
    binary_trie(trie_node_allocator trie_allocator,
        value_list_node_allocator value_allocator);

    ~binary_trie();

    // trie iterators
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

    // insertion
    pair_iterator_bool insert_equal(const binary_type& key,
        const value_type& value);

    pair_iterator_bool insert_unique(const binary_type& key,
        const value_type& value);

    // lookup
    // iterator range will share a key
    iterator_range find_equal(const binary_type& key);

    // iterator range will span keys with matching prefix
    iterator_range find_prefix(const binary_type& key);

    // deletion
    bool remove_equal(const binary_type& key);

    bool remove_prefix(const binary_type& key);

    iterator remove_value(iterator it);

protected:

    void destroy();

    // trie node management
    trie_node_type* create_trie_node();

    trie_node_type* create_trie_node(const binary_type& key);

    trie_node_type* create_trie_node(const binary_type& key,
        const value_node_type* value_node);

    trie_node_type* create_trie_node(const binary_type& key,
        const value_type& value);

    bool destroy_trie_node(trie_node_type* node);

    // value node management
    value_node_type* create_value_node(const value_type& value);

    bool destroy_value_node(value_node_type* node);

    // seek methods
    trie_node_type* get_leftmost_leaf(trie_node_type* origin) const;

    trie_node_type* get_rightmost_leaf(trie_node_type* origin) const;

    trie_node_type* get_leftmost_node(trie_node_type* origin) const;

    trie_node_type* get_rightmost_node(trie_node_type* origin) const;

    // next/previous/leftmost/rightmost pointer updates
    void update_left_and_right(trie_node_type* node);

    void update_left_and_right_branch(trie_node_type* node);

    trie_node_type* next_node_with_value(trie_node_type* node);

    void link_node(trie_node_type* node);

    void unlink_node(trie_node_type* node);

    // cleanup/teardown
    void erase_values(trie_node_type* node);

    void erase_subtree(trie_node_type* node);

    trie_node_type* compress_branch(trie_node_type* node);

    //
    void append_value(trie_node_type* node, value_node_type* value_node);

    void append_value(trie_node_type* node, const value_type& value);

    void attach_child(trie_node_type* parent, trie_node_type* child);

    trie_node_type* insert_at(trie_node_type* current, const binary_type& key);

    trie_node_type* insert_at(trie_node_type* current, const binary_type& key,
        const value_type& value);

    pair_iterator_bool insert(trie_node_type*& current, const binary_type& key,
        const value_type& value);

    pair_node_size find_closest_subkey_matching_node(trie_node_type* start,
        const binary_type& key);

private:

    trie_node_allocator node_allocator_;
    value_list_node_allocator value_allocator_;
    trie_node_type* root_;
};

}
}

#include <bitcoin/blockchain/impl/binary_trie.ipp>

#endif
