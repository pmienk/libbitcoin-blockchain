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

#include <bitcoin/blockchain/database/revised/block_index_store.hpp>
//#include <bitcoin/blockchain/pointer_array_sink.hpp>

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

block_index_store::block_index_store()
    : chain_top_height_(),
      index_(),
      chain_height_store_(index_.get<by_chain_height>()),
      hash_store_(index_.get<by_hash>())
{
}

block_index_store::~block_index_store()
{
}

void block_index_store::create()
{
}

void block_index_store::start()
{
}

block_index_store::chain_height_iterated_index_data_result block_index_store::get(
    const chain_id_type& chain_id)
{
    auto height_result = chain_top_height_.find(chain_id);

    if (height_result == chain_top_height_.end())
        return chain_height_iterated_index_data_result(chain_height_store_,
            chain_height_store_.end());

    return get(chain_id, height_result->second);
}

block_index_store::chain_height_iterated_index_data_result block_index_store::get(
    const chain_id_type& chain_id, const height_type& height)
{
    auto result = chain_height_store_.find(boost::make_tuple(height, chain_id));
    return chain_height_iterated_index_data_result(chain_height_store_, result);
}

block_index_store::chain_height_iterated_index_data_result block_index_store::remove_value(
    chain_height_iterated_index_data_result& it)
{
    if (it)
    {
        repeal(it.data());
        auto next = chain_height_store_.erase(it.source());
        return chain_height_iterated_index_data_result(chain_height_store_, next);
    }

    return it;
}

block_index_store::hash_iterated_index_data_result block_index_store::get(
    const hash_digest& hash)
{
    auto result = hash_store_.find(hash);
    return hash_iterated_index_data_result(hash_store_, result);
}

block_index_store::hash_iterated_index_data_result block_index_store::remove_value(
    hash_iterated_index_data_result& it)
{
    if (it)
    {
        repeal(it.data());
        auto next = hash_store_.erase(it.source());
        return hash_iterated_index_data_result(hash_store_, next);
    }

    return it;
}

block_index_store::hash_iterated_index_data_result block_index_store::store(
    const entry& entry)
{
    auto result = index_.insert(entry);

    if (result.second)
    {
        auto height_result = chain_top_height_.find(entry.chain_id);

        if (height_result == chain_top_height_.end())
            chain_top_height_.emplace(entry.chain_id, entry.height);
        else if (height_result->second < entry.height)
            height_result->second = entry.height;
    }

    return hash_iterated_index_data_result(hash_store_, result.first);
}

void block_index_store::sync()
{
}

void block_index_store::repeal(const entry& entry)
{
    auto top = chain_top_height_.find(entry.chain_id);

    if ((top != chain_top_height_.end()) && (top->second == entry.height))
    {
        bool found_replacement = false;
        height_type height = 0;

        if (top->second > 0)
        {
            for (height_type h = top->second - 1; (h > 0); h--)
            {
                auto result = chain_height_store_.find(boost::make_tuple(
                    h, entry.chain_id));

                if (result != chain_height_store_.end())
                {
                    found_replacement =  true;
                    height = h;
                    break;
                }
            }
        }

        if (found_replacement)
            top->second = height;
        else
            chain_top_height_.erase(top);
    }
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin
