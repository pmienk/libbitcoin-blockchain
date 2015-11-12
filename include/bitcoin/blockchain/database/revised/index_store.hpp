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
#ifndef LIBBITCOIN_INDEX_STORE_HPP
#define LIBBITCOIN_INDEX_STORE_HPP

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
template<typename IndexData>
class index_store
{
public:
    typedef IndexData index_data_type;

    typedef typename modified_patricia_trie<uint32_t,
        index_data_type>::secondary_key_type secondary_key_type;

    typedef index_prefix_query_result<index_data_type> index_prefix_query_result_type;

    typedef index_data_result<index_data_type> index_data_result_type;

public:
    index_store();

    ~index_store();

    void create();

    void start();

    index_prefix_query_result_type get(const hash_digest& hash) const;

    index_prefix_query_result_type get(const binary_type& prefix) const;

    void sync();

protected:
    modified_patricia_trie<uint32_t, index_data_type> trie_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/database/revised/index_store.ipp>

#endif
