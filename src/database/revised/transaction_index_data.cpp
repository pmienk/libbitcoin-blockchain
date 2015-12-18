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

#include <bitcoin/blockchain/database/revised/transaction_index_data.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

transaction_index_data::transaction_index_data()
    : index_data(), offset_(0), input_count_(0), output_count_(0),
      output_spender_(nullptr)
{
}

transaction_index_data::transaction_index_data(uint8_t chain_id,
    simple_allocator::position_type offset)
    : index_data(chain_id), offset_(offset), input_count_(0), output_count_(0),
      output_spender_(nullptr)
{
}

transaction_index_data::transaction_index_data(
    const transaction_index_data& other)
    : index_data(other), offset_(other.offset_),
      input_count_(other.input_count_), output_count_(other.output_count_),
      output_spender_(nullptr)
{
    if (output_count_ > 0)
        output_spender_ = new hash_digest[output_count_];

    for (uint32_t i = 0; i < output_count_; i++)
    {
        if (other.output_spender_ != nullptr)
            output_spender_[i] = other.output_spender_[i];
        else
            output_spender_[i] = null_hash;
    }
}

transaction_index_data::~transaction_index_data()
{
    if (output_spender_ != nullptr)
        delete [] output_spender_;

    output_spender_ = nullptr;
}

simple_allocator::position_type transaction_index_data::offset() const
{
    return offset_;
}

//void transaction_index_data::offset(simple_allocator::position_type value)
//{
//    offset_ = value;
//}

uint32_t transaction_index_data::input_count() const
{
    return input_count_;
}

void transaction_index_data::input_count(uint32_t count)
{
    input_count_ = count;
}

uint32_t transaction_index_data::output_count() const
{
    return output_count_;
}

void transaction_index_data::output_count(uint32_t count)
{
    if (output_count_ != count)
    {
        hash_digest* previous = output_spender_;

        if (count > 0)
        {
            output_spender_ = new hash_digest[count];

            for (uint32_t i = 0; i < count; i++)
                output_spender_[i] = (i < output_count_) ? previous[i] : null_hash;
        }

        output_count_ = count;

        if (previous != nullptr)
            delete [] previous;
    }
}

hash_digest transaction_index_data::spender(uint32_t output_index) const
{
    if (output_index < output_count_)
        return output_spender_[output_index];

    return null_hash;
}

bool transaction_index_data::spender(uint32_t output_index,
    const hash_digest& hash) const
{
    bool result = false;

    if (output_index < output_count_)
    {
        output_spender_[output_index] = hash;
        result = true;
    }

    return result;
}

bool transaction_index_data::operator>(const transaction_index_data& other) const
{
    return (offset() > other.offset());
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
