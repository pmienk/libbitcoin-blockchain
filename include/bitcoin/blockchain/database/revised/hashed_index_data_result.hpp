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
#ifndef LIBBITCOIN_HASHED_INDEX_DATA_RESULT_HPP
#define LIBBITCOIN_HASHED_INDEX_DATA_RESULT_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * Index of transactions.
 * Used to resolve offsets from hashes.
 */
template<typename Index>
class hashed_index_data_result
{
public:
    typedef Index index_type;

    typedef typename index_type::value_type index_data_type;

    typedef typename index_type::iterator iterator_source;

    typedef hashed_index_data_result<index_type> iter_type;

    typedef std::function<void (index_data_type&)> data_modifier;

public:
    hashed_index_data_result(index_type& index, iterator_source source);

    ~hashed_index_data_result();

    operator bool() const;

    const index_data_type& data() const;

    iterator_source source() const;

    bool modify(data_modifier mod);

    // iterator methods
    bool operator==(const iter_type& other) const;

    bool operator!=(const iter_type& other) const;

    iter_type& operator++();

    iter_type operator++(int);

    iter_type& operator--();

    iter_type operator--(int);

private:
    struct data_modifier_functor
    {
        data_modifier modifier;

        void operator()(index_data_type& x)const
        {
            modifier(x);
        }
    };

    index_type& index_;
    iterator_source source_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/database/revised/hashed_index_data_result.ipp>

#endif
