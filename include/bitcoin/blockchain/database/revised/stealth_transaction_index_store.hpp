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
#ifndef LIBBITCOIN_TRANSACTION_INDEX_STORE_HPP
#define LIBBITCOIN_TRANSACTION_INDEX_STORE_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/index_store.hpp>
#include <bitcoin/blockchain/database/revised/simple_transaction_index_data.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * Index of transactions.
 * Used to resolve offsets from hashes.
 */
class BCB_API stealth_transaction_index_store
    : public index_store<simple_transaction_index_data>
{
public:
    stealth_transaction_index_store();

    ~stealth_transaction_index_store();

    /**
     * Store transaction data in the database.
     */
    index_data_result_type store(const uint32_t& prefix,
        const secondary_key_type& height,
        uint8_t chain_id,
        simple_allocator::position_type offset);
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
