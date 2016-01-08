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

#include <bitcoin/blockchain/database/revised/header_store.hpp>
#include <bitcoin/blockchain/pointer_array_sink.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

header_store::header_store(const boost::filesystem::path& filename)
    : file_(filename), allocator_(file_)
{
}

void header_store::create()
{
    file_.resize(0);
    allocator_.create();
}

void header_store::start()
{
    allocator_.start();
}

header_result header_store::get(
    const simple_allocator::position_type offset) const
{
    auto data = allocator_.get(offset);
    return header_result(data);
}

simple_allocator::position_type header_store::store(
    const chain::header& header)
{
    auto size = header.serialized_size(false) + 1;
    auto offset = allocator_.allocate(size);
    auto data = allocator_.get(offset);
    boost::iostreams::stream<byte_pointer_array_sink> sink(data, size);
    ostream_writer writer(sink);
    writer.write_byte(0x00);
    header.to_data(writer, false);
    return offset;
}

void header_store::sync()
{
    allocator_.sync();
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
