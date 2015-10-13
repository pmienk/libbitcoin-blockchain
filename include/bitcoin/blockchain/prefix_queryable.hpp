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
#ifndef LIBBITCOIN_BLOCKCHAIN_PREFIX_QUERYABLE_HPP
#define LIBBITCOIN_BLOCKCHAIN_PREFIX_QUERYABLE_HPP

#include <cstdint>
#include <functional>
#include <vector>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/block_identity.hpp>
#include <bitcoin/blockchain/block_located.hpp>
#include <bitcoin/blockchain/query_filter.hpp>
#include <bitcoin/blockchain/unspent_transaction_outputs.hpp>

namespace libbitcoin {
namespace blockchain {

typedef block_located<hash_digest> transaction_hash_result;

typedef block_located<chain::transaction> tx_result;

typedef block_located<unspent_transaction_outputs> utxo_result;

/**
 * An interface to a prefix queryable backend.
 */
class prefix_queryable
{
public:

    template <typename Message>
    using fetch_handler = std::function<void (
        const std::error_code&,
        const boost::optional<block_identity>,
        const boost::optional<block_identity>,
        const Message&)>;

    typedef fetch_handler<chain::header::list> fetch_handler_header;

    typedef fetch_handler<chain::block::list> fetch_handler_block;

    typedef fetch_handler<transaction_hash_result::list>
        fetch_handler_transaction_hashes;

    typedef fetch_handler<tx_result::list> fetch_handler_transactions;

    typedef fetch_handler<utxo_result::list>
        fetch_handler_unspent_transaction_outputs;

    typedef std::function<void (const std::error_code&, const block_identity)>
        fetch_handler_last_block_identity;

    virtual ~prefix_queryable() {}

    /**
     * Fetches block headers.
     *
     * @param[in]   handler             Completion handler for fetch operation.
     * @param[in]   start               Block identity for the first result.
     * @param[in]   results_per_page    Number of results to return.
     * @code
     *  void handle_fetch(
     *      const std::error_code& code,                // Status of operation
     *      const boost::optional<block_identity> next, // Indicates continuation
     *      const boost::optional<block_identity> top,  // Indicates end of chain
     *      const chain::header::list headers           // Block headers
     *  );
     * @endcode
     */
    virtual void fetch_block_headers(
        fetch_handler_header handler, block_identity start,
        uint32_t results_per_page) = 0;

    /**
     * Fetches blocks.
     *
     * @param[in]   handler             Completion handler for fetch operation.
     * @param[in]   start               Block identity for the first result.
     * @param[in]   results_per_page    Number of results to return.
     * @code
     *  void handle_fetch(
     *      const std::error_code& code,                // Status of operation
     *      const boost::optional<block_identity> next, // Indicates continuation
     *      const boost::optional<block_identity> top,  // Indicates end of chain
     *      const chain::block::list blocks             // Blocks
     *  );
     * @endcode
     */
    virtual void fetch_blocks(
        fetch_handler_block handler, block_identity start,
        uint32_t results_per_page) = 0;

    /**
     * Fetches transaction hashes.
     *
     * @param[in]   handler             Completion handler for fetch operation.
     * @param[in]   start               Block identity for the first result.
     * @param[in]   results_per_page    Number of results to return.
     * @param[in]   queries             Collection of zero or more filters.
     * @param[in]   location            The block_location quality.
     * @code
     *  void handle_fetch(
     *      const std::error_code& code,                // Status of operation
     *      const boost::optional<block_identity> next, // Indicates continuation
     *      const boost::optional<block_identity> top,  // Indicates end of chain
     *      const transaction_hash_result::list hashes  // Transaction hash results
     *  );
     * @endcode
     */
    virtual void fetch_transaction_hashes(
        fetch_handler_transaction_hashes handler, block_identity start,
        uint32_t results_per_page, query_filter::list queries,
        location_type location = location_type::NONE) = 0;

    /**
     * Fetches transactions.
     *
     * @param[in]   handler             Completion handler for fetch operation.
     * @param[in]   start               Block identity for the first result.
     * @param[in]   results_per_page    Number of results to return.
     * @param[in]   queries             Collection of zero or more filters.
     * @param[in]   location            The block_location quality.
     * @code
     *  void handle_fetch(
     *      const std::error_code& code,                // Status of operation
     *      const boost::optional<block_identity> next, // Indicates continuation
     *      const boost::optional<block_identity> top,  // Indicates end of chain
     *      const tx_result::list transactions          // Transaction results
     *  );
     * @endcode
     */
    virtual void fetch_transactions(
        fetch_handler_transactions handler, block_identity start,
        uint32_t results_per_page, query_filter::list queries,
        location_type location = location_type::NONE) = 0;

    /**
     * Fetches unspent transaction outputs (utxos).
     *
     * @param[in]   handler             Completion handler for fetch operation.
     * @param[in]   start               Block identity for the first result.
     * @param[in]   results_per_page    Number of results to return.
     * @param[in]   queries             Collection of zero or more filters.
     * @param[in]   location            The block_location quality.
     * @code
     *  void handle_fetch(
     *      const std::error_code& code,                // Status of operation
     *      const boost::optional<block_identity> next, // Indicates continuation
     *      const boost::optional<block_identity> top,  // Indicates end of chain
     *      const utxo_result_list utxos                // utxo results
     *  );
     * @endcode
     */
    virtual void fetch_unspent_transaction_outputs(
        fetch_handler_unspent_transaction_outputs handler,
        block_identity start, uint32_t results_per_page,
        query_filter::list queries,
        location_type location = location_type::NONE) = 0;

    /**
     * Fetches the identity of the last block in the known blockchain.
     *
     * @param[in]   handler         Completion handler for fetch operation.
     * @code
     *  void handle_fetch(
     *      const std::error_code& code,    // Status of operation
     *      const block_identity id         // Identity of the last block
     *  );
     * @endcode
     */
    virtual void fetch_last_block_identity(
        fetch_handler_last_block_identity handler) = 0;
};

} // namespace blockchain
} // namespace libbitcoin

#endif
