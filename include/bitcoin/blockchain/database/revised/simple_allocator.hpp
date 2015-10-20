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
#ifndef LIBBITCOIN_BLOCKCHAIN_SIMPLE_ALLOCATOR_HPP
#define LIBBITCOIN_BLOCKCHAIN_SIMPLE_ALLOCATOR_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
//#include <bitcoin/blockchain/database/disk_array.hpp>
#include <bitcoin/blockchain/database/mmfile.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * The simple allocator provides a basic mapping between identifiers
 *
 */
class BCB_API simple_allocator
{
public:
    typedef uint8_t* bytes_type;
    typedef uint64_t position_type;

    simple_allocator(mmfile& file);

    /**
      * Create simple allocator.
      */
    void create();

    /**
     * Prepare allocator for usage.
     */
    void start();

    /**
     * Synchronise allocator to disk.
     */
    void sync();

    /**
     * Allocate contiguous bytes of the provided size.
     * Call sync() after writing the record.
     */
    position_type allocate(uint64_t bytes_needed);

    /**
     * Return bytes from byte-wise position relative to start.
     */
    bytes_type get(position_type position) const;

    /**
     * Return distance from reference to eof providing a read boundary.
     */
    position_type to_eof(bytes_type location) const;

private:
    /// File data access, by byte-wise position relative to start.
    bytes_type data(const position_type offset) const;

    /// Ensure bytes for a new record are available.
    void reserve(uint64_t bytes_needed);

    /// Read the size of the data from the file.
    void read_size();

    /// Write the size of the data from the file.
    void write_size();

    mmfile& file_;
    position_type size_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
