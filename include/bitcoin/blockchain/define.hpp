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
#ifndef LIBBITCOIN_BLOCKCHAIN_DEFINE_HPP
#define LIBBITCOIN_BLOCKCHAIN_DEFINE_HPP

#include <bitcoin/bitcoin.hpp>

// We use the generic helper definitions in libbitcoin to define BCB_API
// and BCB_INTERNAL. BCB_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) BCB_INTERNAL is
// used for non-api symbols.

#if defined BCC_STATIC
#define BCB_API
#define BCB_INTERNAL
#elif defined BCC_DLL
#define BCB_API      BC_HELPER_DLL_EXPORT
#define BCB_INTERNAL BC_HELPER_DLL_LOCAL
#else
#define BCB_API      BC_HELPER_DLL_IMPORT
#define BCB_INTERNAL BC_HELPER_DLL_LOCAL
#endif

#endif

