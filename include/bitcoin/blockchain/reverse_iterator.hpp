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

#ifndef LIBBITCOIN_BLOCKCHAIN_REVERSE_ITERATOR_HPP
#define LIBBITCOIN_BLOCKCHAIN_REVERSE_ITERATOR_HPP

#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace blockchain {

template<typename Iterator>
class reverse_iterator
{
public:
    // iterator_traits required typedefs
    typedef typename std::iterator_traits<Iterator>::difference_type
        difference_type;
    typedef typename std::iterator_traits<Iterator>::iterator_category
        iterator_category;
    typedef typename std::iterator_traits<Iterator>::pointer pointer;
    typedef typename std::iterator_traits<Iterator>::reference reference;
    typedef typename std::iterator_traits<Iterator>::value_type value_type;

    typedef Iterator base_iter_type;
    typedef reverse_iterator<base_iter_type> iter_type;

public:

    // constructors
    reverse_iterator();

    explicit reverse_iterator(base_iter_type it);

    reverse_iterator(const iter_type& other);

    template<typename Iter>
    reverse_iterator(const reverse_iterator<Iter> &other);

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

protected:

    base_iter_type base_iterator_;
};

} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/reverse_iterator.ipp>

#endif
