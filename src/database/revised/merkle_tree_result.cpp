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

#include <bitcoin/blockchain/database/revised/merkle_tree_result.hpp>
#include <bitcoin/blockchain/pointer_array_source.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

merkle_tree_result::merkle_tree_result(simple_allocator::bytes_type data,
    uint64_t size)
    : data_(data), size_limit_(size)
{
}

merkle_tree_result::operator bool() const
{
    return (data_ != nullptr);
}

hash_digest merkle_tree_result::header_hash() const
{
    hash_digest result;
    boost::iostreams::stream<byte_pointer_array_source> source(data_, size_limit_);
    istream_reader reader(source);
    reader.read_byte();
    auto count = reader.read_4_bytes_little_endian();
    if (count > 0)
        result = reader.read_hash();

    return result;
}

hash_list merkle_tree_result::transaction_hashes() const
{
    hash_list result;
    boost::iostreams::stream<byte_pointer_array_source> source(data_, size_limit_);
    istream_reader reader(source);
    reader.read_byte();
    auto count = reader.read_4_bytes_little_endian();
    if (count > 0)
        reader.read_hash();

    for (uint32_t i = 1; i < count; i++)
        result.push_back(reader.read_hash());

    return result;
}

uint32_t merkle_tree_result::transaction_count() const
{
    boost::iostreams::stream<byte_pointer_array_source> source(data_, size_limit_);
    istream_reader reader(source);
    reader.read_byte();
    auto result = reader.read_4_bytes_little_endian();
    return result - 1;
}

bool merkle_tree_result::marked() const
{
    return (data_[0] != 0xFF);
}

void merkle_tree_result::mark()
{
    data_[0] = 0xFF;
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
