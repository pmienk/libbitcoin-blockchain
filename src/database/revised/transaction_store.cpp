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

#include <bitcoin/blockchain/database/revised/transaction_store.hpp>
#include <bitcoin/blockchain/pointer_array_sink.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

transaction_store::transaction_store(const boost::filesystem::path& filename)
    : file_(filename), allocator_(file_)
{
}

void transaction_store::create()
{
    file_.resize(0);
    allocator_.create();
}

void transaction_store::start()
{
    allocator_.start();
}

transaction_result transaction_store::get(
    const simple_allocator::position_type offset) const
{
    auto data = allocator_.get(offset);
    auto bytes_to_eof = allocator_.to_eof(data);
    return transaction_result(data, bytes_to_eof);
}

simple_allocator::position_type transaction_store::store(
    const chain::transaction& transaction)
{
    uint64_t transaction_size = transaction.serialized_size();
    if (transaction_size > max_uint32)
        throw std::runtime_error("The transaction size exceeds maximum encodable (uint32_t).");

    uint64_t size = 1 + 4 + transaction_size;
    auto offset = allocator_.allocate(size);
    auto data = allocator_.get(offset);
    boost::iostreams::stream<byte_pointer_array_sink> sink(data, size);
    ostream_writer writer(sink);
    writer.write_byte(0x00);
    writer.write_4_bytes_little_endian(transaction_size);
    transaction.to_data(writer);
    return offset;
}

void transaction_store::sync()
{
    allocator_.sync();
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
