/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_BLOCKCHAIN_BLOCK_LOCATION_HPP
#define LIBBITCOIN_BLOCKCHAIN_BLOCK_LOCATION_HPP

#include <vector>
#include <boost/optional/optional.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/block_identity.hpp>

namespace libbitcoin {
namespace blockchain {

enum class location_type
{
    NONE = 1,
    BLOCK = 2,
    MERKLE = 3
};

struct BCB_API block_location
{
    boost::optional<block_identity> identity;
    std::vector<hash_digest> branch;
};

} // namespace blockchain
} // namespace libbitcoin

#endif

