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
#ifndef LIBBITCOIN_INDEX_DATA_HPP
#define LIBBITCOIN_INDEX_DATA_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/simple_allocator.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

class BCB_API index_data
{
public:
    index_data();

    index_data(uint8_t chain_id);

    ~index_data();

    uint8_t chain_id() const;

    void chain_id(uint8_t id);

private:
    uint8_t chain_id_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
