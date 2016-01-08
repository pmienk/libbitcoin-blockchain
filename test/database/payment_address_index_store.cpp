/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <new>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/blockchain.hpp>

using namespace bc;
using namespace bc::blockchain;
using namespace bc::blockchain::revised_database;

BOOST_AUTO_TEST_SUITE(payment_address_index_store_tests)

//
// create() method tests
//
BOOST_AUTO_TEST_CASE(create_nop)
{
    payment_address_index_store index;
    BOOST_CHECK_NO_THROW(index.create());
}

//
// start() method tests
//
BOOST_AUTO_TEST_CASE(start_nop)
{
    payment_address_index_store index;
    BOOST_CHECK_NO_THROW(index.start());
}

//
// sync() method tests
//
BOOST_AUTO_TEST_CASE(sync_nop)
{
    payment_address_index_store index;
    BOOST_CHECK_NO_THROW(index.sync());
}

//
// get(const short_hash&) method tests
//
BOOST_AUTO_TEST_CASE(get_hash_missing)
{
    payment_address_index_store index;
    auto get_result = index.get(null_short_hash);
    BOOST_REQUIRE(!get_result);
}

BOOST_AUTO_TEST_CASE(get_hash_success)
{
    payment_address_index_store index;
    auto store_result = index.store(null_short_hash, 2, 3, 4);
    BOOST_REQUIRE(store_result);

    auto get_result = index.get(null_short_hash);
    BOOST_REQUIRE(get_result);

    auto range = get_result.get_exact(2);
    BOOST_REQUIRE(range.first != range.second);

    auto& data = (range.first).data();
    BOOST_REQUIRE(data.chain_id() == 3);
    BOOST_REQUIRE(data.offset() == 4);

    BOOST_REQUIRE(++(range.first) == range.second);
}

//
// get_prefix(const binary_type&) method tests
//
BOOST_AUTO_TEST_CASE(get_binary_missing)
{
    payment_address_index_store index;
    binary_type primary(hash_size, null_hash);
    auto get_result = index.get_prefix(primary);
    BOOST_REQUIRE(!get_result);
}

BOOST_AUTO_TEST_CASE(get_binary_success)
{
    payment_address_index_store index;
    auto primary = null_short_hash;
    binary_type primary_binary(short_hash_size * byte_bits, primary);

    auto store_result = index.store(primary, 2, 3, 4);
    BOOST_REQUIRE(store_result);

    auto get_result = index.get_prefix(primary_binary);
    BOOST_REQUIRE(get_result);

    auto range = get_result.get_exact(2);
    BOOST_REQUIRE(range.first != range.second);

    auto& data = (range.first).data();
    BOOST_REQUIRE(data.chain_id() == 3);
    BOOST_REQUIRE(data.offset() == 4);

    BOOST_REQUIRE(++(range.first) == range.second);
}

//
// remove_equal(const binary_type&, const secondary_key_type&) method tests
//
BOOST_AUTO_TEST_CASE(remove_equal_primary_secondary_missing)
{
    payment_address_index_store index;
    binary_type primary(hash_size, null_hash);
    BOOST_REQUIRE(index.remove_equal(primary, 0) == false);
}

BOOST_AUTO_TEST_CASE(remove_equal_primary_secondary_success)
{
    payment_address_index_store index;
    auto primary = null_short_hash;
    binary_type primary_binary(short_hash_size * byte_bits, primary);
    transaction_index_store::secondary_key_type secondary = 1;

    auto store_result = index.store(primary, secondary, 2, 5);
    BOOST_REQUIRE(store_result);

    bool result = index.remove_equal(primary_binary, secondary);
    BOOST_REQUIRE(result);
}

//
// remove_equal(const secondary_key_type&) method tests
//
BOOST_AUTO_TEST_CASE(remove_equal_secondary_missing)
{
    payment_address_index_store index;
    BOOST_REQUIRE(index.remove_equal(0) == false);
}

BOOST_AUTO_TEST_CASE(remove_equal_secondary_success)
{
    payment_address_index_store index;
    auto store_result = index.store(null_short_hash, 2, 3, 4);
    BOOST_REQUIRE(store_result);

    BOOST_REQUIRE(index.remove_equal(2) == true);
}

//
// remove_value(index_data_result_type) method tests
//
BOOST_AUTO_TEST_CASE(remove_value_missing)
{
    payment_address_index_store index;
    auto get_result = index.get(null_short_hash);
    auto range = get_result.get_exact(0);
    BOOST_REQUIRE(range.first == range.second);
    BOOST_REQUIRE(!range.first);

    auto remove_result = index.remove_value(range.first);
    BOOST_REQUIRE(remove_result == range.first);
    BOOST_REQUIRE(!remove_result);
}

BOOST_AUTO_TEST_CASE(remove_value_success)
{
    payment_address_index_store index;
    auto store_result = index.store(null_short_hash, 2, 3, 4);
    BOOST_REQUIRE(store_result);

    auto remove_result = index.remove_value(store_result);
    BOOST_REQUIRE(remove_result != store_result);
}

//
// store() method tests
//
BOOST_AUTO_TEST_CASE(store_unique_success)
{
    payment_address_index_store index;
    auto result = index.store(null_short_hash, 2, 3, 4);
    BOOST_REQUIRE(result);

    auto& data = result.data();
    BOOST_REQUIRE(data.chain_id() == 3);
    BOOST_REQUIRE(data.offset() == 4);
}

BOOST_AUTO_TEST_CASE(store_append_success)
{
    payment_address_index_store index;
    auto result1 = index.store(null_short_hash, 2, 3, 4);
    BOOST_REQUIRE(result1);

    auto& data1 = result1.data();
    BOOST_REQUIRE(data1.chain_id() == 3);
    BOOST_REQUIRE(data1.offset() == 4);

    auto result2 = index.store(null_short_hash, 12, 13, 0);
    BOOST_REQUIRE(result2);

    auto& data2 = result2.data();
    BOOST_REQUIRE(data2.chain_id() == 13);
    BOOST_REQUIRE(data2.offset() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
