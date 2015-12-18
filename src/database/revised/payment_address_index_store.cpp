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

#include <bitcoin/blockchain/database/revised/payment_address_index_store.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

payment_address_index_store::payment_address_index_store()
    : prefix_queryable_index_store()
{
}

payment_address_index_store::~payment_address_index_store()
{
}

payment_address_index_store::index_prefix_query_result_type
    payment_address_index_store::get(const short_hash& address) const
{
    binary_type prefix(short_hash_size * byte_bits, address);

    return get_prefix(prefix);
}

payment_address_index_store::index_data_result_type
    payment_address_index_store::store(const short_hash& address,
        const secondary_key_type& height,
        uint8_t chain_id,
        simple_allocator::position_type offset)
{
    binary_type primary_key(short_hash_size * byte_bits, address);

    auto result = trie_.insert_equal(primary_key, height,
        simple_transaction_index_data(chain_id, offset));

    return (payment_address_index_store::index_data_result_type)(result.first);
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
