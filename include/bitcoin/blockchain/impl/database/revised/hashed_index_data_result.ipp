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

#ifndef LIBBITCOIN_HASHED_INDEX_DATA_RESULT_IPP
#define LIBBITCOIN_HASHED_INDEX_DATA_RESULT_IPP

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

template<typename Index>
hashed_index_data_result<Index>::hashed_index_data_result(
    index_type& index, iterator_source source)
    : index_(index), source_(source)
{
}

template<typename Index>
hashed_index_data_result<Index>::~hashed_index_data_result()
{
}

template<typename Index>
hashed_index_data_result<Index>::operator bool() const
{
    return (index_.end() != source_);
}

template<typename Index>
const typename hashed_index_data_result<Index>::index_data_type&
    hashed_index_data_result<Index>::data() const
{
    return *source_;
}

template<typename Index>
typename hashed_index_data_result<Index>::iterator_source
    hashed_index_data_result<Index>::source() const
{
    return source_;
}

template<typename Index>
bool hashed_index_data_result<Index>::modify(data_modifier mod)
{
    data_modifier_functor temp { mod };
    return index_.template modify<data_modifier_functor>(source_, temp);
}

template<typename Index>
bool hashed_index_data_result<Index>::operator==(
    const iter_type& other) const
{
    return source_ == other.source_;
}

template<typename Index>
bool hashed_index_data_result<Index>::operator!=(
    const iter_type& other) const
{
    return !(*this == other);
}

template<typename Index>
typename hashed_index_data_result<Index>::iter_type&
    hashed_index_data_result<Index>::operator++()
{
    source_++;
    return *this;
}

template<typename Index>
typename hashed_index_data_result<Index>::iter_type
    hashed_index_data_result<Index>::operator++(int)
{
    auto it = *this;
    source_++;
    return it;
}

template<typename Index>
typename hashed_index_data_result<Index>::iter_type&
    hashed_index_data_result<Index>::operator--()
{
    source_--;
    return *this;
}

template<typename Index>
typename hashed_index_data_result<Index>::iter_type
    hashed_index_data_result<Index>::operator--(int)
{
    auto it = *this;
    source_--;
    return it;
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
