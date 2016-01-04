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

#ifndef LIBBITCOIN_INDEX_DATA_RESULT_IPP
#define LIBBITCOIN_INDEX_DATA_RESULT_IPP

namespace libbitcoin {
namespace blockchain {
namespace revised_database {

template<std::size_t Size, typename IndexData>
index_data_result<Size, IndexData>::index_data_result(
    iterator_source& source)
    : source_(source)
{
}

template<std::size_t Size, typename IndexData>
index_data_result<Size, IndexData>::~index_data_result()
{
}

template<std::size_t Size, typename IndexData>
index_data_result<Size, IndexData>::operator bool() const
{
    return source_;
}

template<std::size_t Size, typename IndexData>
typename index_data_result<Size, IndexData>::iterator_source::reference
    index_data_result<Size, IndexData>::data() const
{
    return *source_;
}

//template<typename IndexData>
//typename index_data_result<IndexData>::iterator_source&
//    index_data_result<IndexData>::source() const
//{
//    return source_;
//}

template<std::size_t Size, typename IndexData>
bool index_data_result<Size, IndexData>::operator==(
    const iter_type& other) const
{
    return source_ == other.source_;
}

template<std::size_t Size, typename IndexData>
bool index_data_result<Size, IndexData>::operator!=(
    const iter_type& other) const
{
    return !(*this == other);
}

template<std::size_t Size, typename IndexData>
typename index_data_result<Size, IndexData>::iter_type&
    index_data_result<Size, IndexData>::operator++()
{
    source_++;
    return *this;
}

template<std::size_t Size, typename IndexData>
typename index_data_result<Size, IndexData>::iter_type
    index_data_result<Size, IndexData>::operator++(int)
{
    auto it = *this;
    source_++;
    return it;
}

template<std::size_t Size, typename IndexData>
typename index_data_result<Size, IndexData>::iter_type&
    index_data_result<Size, IndexData>::operator--()
{
    source_--;
    return *this;
}

template<std::size_t Size, typename IndexData>
typename index_data_result<Size, IndexData>::iter_type
    index_data_result<Size, IndexData>::operator--(int)
{
    auto it = *this;
    source_--;
    return it;
}

} // namespace revised_database
} // namespace blockchain
} // namespace libbitcoin

#endif
