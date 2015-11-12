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

#ifndef LIBBITCOIN_INDEX_PREFIX_QUERY_RESULT_IPP
#define LIBBITCOIN_INDEX_PREFIX_QUERY_RESULT_IPP

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

template<typename IndexData>
index_prefix_query_result<IndexData>::index_prefix_query_result(
    const pair_query_result_bool& result)
    : result_(result.first), is_valid_(result.second)
{
}

template<typename IndexData>
index_prefix_query_result<IndexData>::index_prefix_query_result(
    const query_result& result, bool is_valid)
    : result_(result), is_valid_(is_valid)
{
}

template<typename IndexData>
index_prefix_query_result<IndexData>::~index_prefix_query_result()
{
}

template<typename IndexData>
index_prefix_query_result<IndexData>::operator bool() const
{
    return is_valid_;
}

template<typename IndexData>
typename index_prefix_query_result<IndexData>::data_range
    index_prefix_query_result<IndexData>::get_exact(
        const secondary_key_type& key) const
{
    auto bounds = result_.get_exact(key);

    return std::make_pair((index_data_result_type)(bounds.first),
        (index_data_result_type)(bounds.second));
}

template<typename IndexData>
typename index_prefix_query_result<IndexData>::data_range
    index_prefix_query_result<IndexData>::get_exact_greater_equal(
        const secondary_key_type& key) const
{
    auto bounds = result_.get_exact_greater_equal(key);

    return std::make_pair((index_data_result_type)(bounds.first),
        (index_data_result_type)(bounds.second));
}

template<typename IndexData>
typename index_prefix_query_result<IndexData>::data_range
    index_prefix_query_result<IndexData>::get_prefixed(
        const secondary_key_type& key) const
{
    auto bounds = result_.get_prefixed(key);

    return std::make_pair((index_data_result_type)(bounds.first),
        (index_data_result_type)(bounds.second));
}

template<typename IndexData>
typename index_prefix_query_result<IndexData>::data_range
    index_prefix_query_result<IndexData>::get_prefixed_greater_equal(
        const secondary_key_type& key) const
{
    auto bounds = result_.get_prefixed_greater_equal(key);

    return std::make_pair((index_data_result_type)(bounds.first),
        (index_data_result_type)(bounds.second));
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
