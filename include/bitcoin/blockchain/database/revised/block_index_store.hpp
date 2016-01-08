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
#ifndef LIBBITCOIN_BLOCK_INDEX_STORE_HPP
#define LIBBITCOIN_BLOCK_INDEX_STORE_HPP

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/define.hpp>
#include <bitcoin/blockchain/database/revised/hashed_index_data_result.hpp>
#include <bitcoin/blockchain/database/revised/simple_allocator.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/member.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

/**
 * Index of block data.
 * Used to resolve offsets to header and merkle data from hashes/heights/chains.
 */
class BCB_API block_index_store
{
public:
    typedef uint8_t chain_id_type;

    typedef uint32_t height_type;

    struct entry
    {
        hash_digest hash;
        chain_id_type chain_id;
        height_type height;
        simple_allocator::position_type header_offset;
        simple_allocator::position_type merkle_offset;
    };

    struct by_hash {};

    struct by_chain_height {};

    struct hash_digest_hash
    {
        std::size_t operator()(const hash_digest& key)const
        {
            //return boost::hash<uint32_t>()(key.k1) + boost::hash<uint32_t>()(key.k2);
            hash_number temp;
            temp.set_hash(key);
            return temp.compact();
        }
    };

    typedef boost::multi_index_container<entry, boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<boost::multi_index::tag<by_hash>,
            boost::multi_index::member<entry, hash_digest, &entry::hash>, hash_digest_hash>,
        boost::multi_index::hashed_non_unique<
            boost::multi_index::tag<by_chain_height>,
            boost::multi_index::composite_key<entry,
                boost::multi_index::member<entry, height_type, &entry::height>,
                boost::multi_index::member<entry, chain_id_type, &entry::chain_id>>>>> entry_set;

    typedef hashed_index_data_result<entry_set::index<by_hash>::type>
        hash_iterated_index_data_result;

    typedef hashed_index_data_result<entry_set::index<by_chain_height>::type>
        chain_height_iterated_index_data_result;

//    typedef hashed_index_data_result<boost::multi_index::index<entry_set, by_chain_height>::type>
//        chain_height_iterated_index_data_result;

public:
    block_index_store();

    ~block_index_store();

    void create();

    void start();

    chain_height_iterated_index_data_result get(
        const chain_id_type& chain_id);

    chain_height_iterated_index_data_result get(const chain_id_type& chain_id,
        const height_type& height);

    chain_height_iterated_index_data_result remove_value(
        chain_height_iterated_index_data_result& it);

    hash_iterated_index_data_result get(const hash_digest& hash);

    hash_iterated_index_data_result remove_value(
        hash_iterated_index_data_result& it);

    hash_iterated_index_data_result store(const entry& entry);

    void sync();

//    /**
//     * Store block data in the database.
//     */
//    hash_iterated_index_data_result store(const entry& entry);

private:

    void repeal(const entry& top);

    std::map<chain_id_type, height_type> chain_top_height_;
    entry_set index_;
    entry_set::index<by_chain_height>::type& chain_height_store_;
    entry_set::index<by_hash>::type& hash_store_;
};

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
