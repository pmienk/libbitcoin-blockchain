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

#include <bitcoin/blockchain/database/revised/header_result.hpp>
#include <bitcoin/blockchain/pointer_array_source.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

header_result::header_result(const simple_allocator::bytes_type data)
    : data_(data)
{
}

header_result::operator bool() const
{
    return (data_ != nullptr);
}

chain::header header_result::header() const
{
    boost::iostreams::stream<byte_pointer_array_source> source((data_ + 1), size());
    chain::header result;
    result.from_data(source);
    return result;
}

bool header_result::marked() const
{
    return (data_[0] != 0xFF);
}

void header_result::mark()
{
    data_[0] = 0xFF;
}

uint64_t header_result::size() const
{
    return (*this) ? 80 + 1 : 0;
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
