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

#include <bitcoin/blockchain/database/revised/simple_transaction_index_data.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

simple_transaction_index_data::simple_transaction_index_data()
    : index_data(), offset_(0)
{
}

simple_transaction_index_data::simple_transaction_index_data(uint8_t chain_id,
    simple_allocator::position_type offset)
    : index_data(chain_id), offset_(offset)
{
}

//simple_transaction_index_data::simple_transaction_index_data(
//    const simple_transaction_index_data& other)
//    : index_data(other), offset_(other.offset_)
//{
//}

simple_transaction_index_data::~simple_transaction_index_data()
{
}

simple_allocator::position_type simple_transaction_index_data::offset() const
{
    return offset_;
}

//void simple_transaction_index_data::offset(
//    simple_allocator::position_type value)
//{
//    offset_ = value;
//}

bool simple_transaction_index_data::operator>(
    const simple_transaction_index_data& other) const
{
    return (offset() > other.offset());
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
