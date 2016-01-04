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
#ifndef LIBBITCOIN_INDEX_DATA_RESULT_HPP
#define LIBBITCOIN_INDEX_DATA_RESULT_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/trie/modified_patricia_trie.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

template<std::size_t Size, typename IndexData>
class prefix_queryable_index_store;

/**
 * Index of transactions.
 * Used to resolve offsets from hashes.
 */
template<std::size_t Size, typename IndexData>
class index_data_result
{
public:
    friend class prefix_queryable_index_store<Size, IndexData>;

    typedef IndexData index_data_type;

    typedef typename modified_patricia_trie<Size, uint32_t,
        index_data_type>::iterator iterator_source;

    typedef index_data_result<Size, index_data_type> iter_type;

public:
    index_data_result(iterator_source& source);

    ~index_data_result();

    operator bool() const;

    typename iterator_source::reference data() const;

    // iterator methods
    bool operator==(const iter_type& other) const;

    bool operator!=(const iter_type& other) const;

    iter_type& operator++();

    iter_type operator++(int);

    iter_type& operator--();

    iter_type operator--(int);

private:
    iterator_source source_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/database/revised/index_data_result.ipp>

#endif
