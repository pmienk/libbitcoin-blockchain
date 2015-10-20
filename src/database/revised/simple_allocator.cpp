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
#include <bitcoin/blockchain/database/revised/simple_allocator.hpp>

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/database/mmfile.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

simple_allocator::simple_allocator(mmfile& file)
  : file_(file), size_(0)
{
}

// This writes the byte size of the allocated space to the file.
void simple_allocator::create()
{
    size_ = sizeof(position_type);
    sync();
}

void simple_allocator::start()
{
    read_size();
}

void simple_allocator::sync()
{
    write_size();
}

simple_allocator::position_type simple_allocator::allocate(
    uint64_t bytes_needed)
{
    BITCOIN_ASSERT_MSG(size_ > 0, "simple_allocator::start() wasn't called.");
    const auto position = size_;
    reserve(bytes_needed);
    return position;
}

// logical slab access
simple_allocator::bytes_type simple_allocator::get(
    position_type position) const
{
    BITCOIN_ASSERT_MSG(size_ > 0, "simple_allocator::start() wasn't called.");
    BITCOIN_ASSERT(position < size_);
    return data(position);
}


// retrieve eof/memory boundary
simple_allocator::position_type simple_allocator::to_eof(
    bytes_type location) const
{
    uint64_t result = 0;

    if (location > file_.data())
    {
        uint64_t offset_position = (location - file_.data());

        if (offset_position < size_)
            result = size_ - offset_position;
    }

    return result;
}

// File data access, by byte-wise position relative to start.
simple_allocator::bytes_type simple_allocator::data(
    const position_type offset) const
{
    BITCOIN_ASSERT(sizeof(position_type) + offset <= file_.size());
    return file_.data() + sizeof(position_type) + offset;
}

void simple_allocator::reserve(uint64_t bytes_needed)
{
    // See comment in hsdb_shard::reserve()
    const uint64_t required_size = sizeof(position_type) + size_ + bytes_needed;
    DEBUG_ONLY(const auto success =) file_.reserve(required_size);
    BITCOIN_ASSERT(success);
    size_ += bytes_needed;
}

// Read the size value from the first chunk of the file.
// Since the write is not atomic this must be read before write is enabled.
void simple_allocator::read_size()
{
    BITCOIN_ASSERT(file_.size() >= sizeof(size_));
    size_ = from_little_endian_unsafe<position_type>(file_.data());
}

// Write the size value to the first chunk of the file.
// This write is not atomic and therefore assumes there are no readers.
void simple_allocator::write_size()
{
    BITCOIN_ASSERT(size_ <= file_.size());
    auto serial = make_serializer(file_.data());
    serial.write_little_endian(size_);
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
