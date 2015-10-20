/*
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
#ifndef LIBBITCOIN_BLOCKCHAIN_POINTER_ARRAY_SINK_HPP
#define LIBBITCOIN_BLOCKCHAIN_POINTER_ARRAY_SINK_HPP

#include <algorithm>
#include <cstdint>
#include <boost/iostreams/categories.hpp>
#include <boost/static_assert.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace blockchain {

template<typename SinkType, typename CharType>
class pointer_array_sink
{
public:
    typedef CharType char_type;
    typedef boost::iostreams::sink_tag category;

    pointer_array_sink(SinkType* begin, uint64_t size)
      : size_(size), position_(0), begin_(begin)
    {
        static_assert(sizeof(SinkType) == sizeof(CharType), "oops!");
    }

    std::streamsize write(const char_type* buffer, std::streamsize size)
    {
        const auto safe_sink = reinterpret_cast<const SinkType*>(buffer);
        auto copy_size = (size_ - position_);
        if (size < copy_size)
            copy_size = size;

        if (copy_size > 0)
            memcpy((begin_ + position_), safe_sink, copy_size);

        return copy_size;
    }

private:
    uint64_t size_;
    uint64_t position_;
    SinkType* begin_;
};

typedef pointer_array_sink<uint8_t, char> byte_pointer_array_sink;

} // namespace blockchain
} // namespace libbitcoin

#endif
