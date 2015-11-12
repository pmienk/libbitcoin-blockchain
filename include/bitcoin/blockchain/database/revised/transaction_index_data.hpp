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
#ifndef LIBBITCOIN_TRANSACTION_INDEX_DATA_HPP
#define LIBBITCOIN_TRANSACTION_INDEX_DATA_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/index_data.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

class BCB_API transaction_index_data : public index_data
{
public:
    transaction_index_data();

    transaction_index_data(uint8_t chain_id,
        simple_allocator::position_type offset);

    transaction_index_data(const transaction_index_data& other);

    ~transaction_index_data();

    simple_allocator::position_type offset() const;

//    void offset(simple_allocator::position_type value);

    uint32_t input_count() const;

    void input_count(uint32_t count);

    uint32_t output_count() const;

    void output_count(uint32_t count);

    hash_digest spender(uint32_t output_index) const;

    bool spender(uint32_t output_index, const hash_digest& hash) const;

    bool operator>(const transaction_index_data& other) const;

private:
    simple_allocator::position_type offset_;
    uint32_t input_count_;
    uint32_t output_count_;
    hash_digest* output_spender_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
