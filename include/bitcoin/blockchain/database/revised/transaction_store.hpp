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
#ifndef LIBBITCOIN_TRANSACTION_STORE_HPP
#define LIBBITCOIN_TRANSACTION_STORE_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/simple_allocator.hpp>
#include <bitcoin/blockchain/database/revised/transaction_result.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * Stores transactions.
 * Lookup possible by file offset.
 */
class BCB_API transaction_store
{
public:
    transaction_store(const boost::filesystem::path& filename);

    /**
     * Initialize a new transaction database.
     */
    void create();

    /**
     * You must call start() before using the database.
     */
    void start();

    /**
     * Fetch transaction by offset.
     */
    transaction_result get(const simple_allocator::position_type offset) const;

    /**
     * Store a transaction in the database.
     */
    simple_allocator::position_type store(const chain::transaction& transaction);

    /**
     * Synchronize storage with disk so things are consistent.
     * Should be done at the end of every write, including those result actions
     * which modify data.
     */
    void sync();

private:
    mmfile file_;
    simple_allocator allocator_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
