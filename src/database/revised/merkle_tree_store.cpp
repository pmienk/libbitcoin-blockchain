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

#include <bitcoin/blockchain/database/revised/merkle_tree_store.hpp>
#include <bitcoin/blockchain/pointer_array_sink.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

merkle_tree_store::merkle_tree_store(const boost::filesystem::path& filename)
    : file_(filename), allocator_(file_)
{
}

void merkle_tree_store::create()
{
    file_.resize(0);
    allocator_.create();
}

void merkle_tree_store::start()
{
    allocator_.start();
}

merkle_tree_result merkle_tree_store::get(
    const simple_allocator::position_type offset) const
{
    auto data = allocator_.get(offset);
    auto bytes_to_eof = allocator_.to_eof(data);
    return merkle_tree_result(data, bytes_to_eof);
}

simple_allocator::position_type merkle_tree_store::store(
    const hash_digest& header_hash, const hash_list transaction_hashes)
{
    uint32_t hash_count = (1 + transaction_hashes.size());
    auto size = 1 + 4 + (sizeof(hash_digest) * hash_count);
    auto offset = allocator_.allocate(size);
    auto data = allocator_.get(offset);
    boost::iostreams::stream<byte_pointer_array_sink> sink(data, size);
    ostream_writer writer(sink);
    writer.write_byte(0x00);
    writer.write_4_bytes_little_endian(hash_count);
    writer.write_hash(header_hash);

    for (auto& hash : transaction_hashes)
        writer.write_hash(hash);

    return offset;
}

void merkle_tree_store::sync()
{
    allocator_.sync();
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
