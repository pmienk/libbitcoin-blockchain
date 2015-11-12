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
#ifndef LIBBITCOIN_INDEX_PREFIX_QUERY_RESULT_HPP
#define LIBBITCOIN_INDEX_PREFIX_QUERY_RESULT_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/index_data_result.hpp>
#include <bitcoin/blockchain/trie/modified_patricia_trie.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * Index of transactions.
 * Used to resolve offsets from hashes.
 */
template<typename IndexData>
class index_prefix_query_result
{
public:
    typedef IndexData index_data_type;

    typedef typename modified_patricia_trie<uint32_t,
        index_data_type>::pair_query_result_bool pair_query_result_bool;

    typedef typename modified_patricia_trie<uint32_t,
        index_data_type>::secondary_key_type secondary_key_type;

    typedef typename modified_patricia_trie<uint32_t,
        index_data_type>::query_result query_result;

    typedef index_data_result<index_data_type> index_data_result_type;

    typedef std::pair<index_data_result_type,
        index_data_result_type> data_range;

public:
    index_prefix_query_result(
        const pair_query_result_bool& result);

    index_prefix_query_result(const query_result& result,
        bool is_valid);

    ~index_prefix_query_result();

    operator bool() const;

    // accessors
    data_range get_exact(const secondary_key_type& key) const;

    data_range get_exact_greater_equal(const secondary_key_type& key) const;

    data_range get_prefixed(const secondary_key_type& key) const;

    data_range get_prefixed_greater_equal(const secondary_key_type& key) const;

private:
    query_result result_;
    bool is_valid_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#include <bitcoin/blockchain/impl/database/revised/index_prefix_query_result.ipp>

#endif
