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

#include <bitcoin/blockchain/database/revised/index_data.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

index_data::index_data()
    :  chain_id_(0)
{
}

index_data::index_data(uint8_t chain_id)
    : chain_id_(chain_id)
{
}

index_data::~index_data()
{
}

uint8_t index_data::chain_id() const
{
    return chain_id_;
}

void index_data::chain_id(uint8_t id)
{
    chain_id_ = id;
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
