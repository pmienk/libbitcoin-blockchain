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
#ifndef LIBBITCOIN_PREFIX_QUERYABLE_INDEX_STORE_HPP
#define LIBBITCOIN_PREFIX_QUERYABLE_INDEX_STORE_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/index_data_result.hpp>
#include <bitcoin/blockchain/database/revised/index_prefix_query_result.hpp>
#include <bitcoin/blockchain/trie/modified_patricia_trie.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * Index of transactions.
 * Used to resolve offsets from hashes.
 */
template<std::size_t Size, typename IndexData>
class prefix_queryable_index_store
{
public:
    typedef IndexData index_data_type;

    typedef typename modified_patricia_trie<Size, uint32_t,
        index_data_type>::secondary_key_type secondary_key_type;

    typedef index_prefix_query_result<Size, index_data_type> index_prefix_query_result_type;

    typedef index_data_result<Size, index_data_type> index_data_result_type;

public:
    prefix_queryable_index_store();

    ~prefix_queryable_index_store();

    void create();

    void start();

    index_prefix_query_result_type get_prefix(const binary_type& prefix) const;

    bool remove_equal(const binary_type& primary,
        const secondary_key_type& secondary);

    bool remove_equal(const secondary_key_type& secondary);

    index_data_result_type remove_value(index_data_result_type& it);

    void sync();

protected:
    modified_patricia_trie<Size, uint32_t, index_data_type> trie_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/database/revised/prefix_queryable_index_store.ipp>

#endif
