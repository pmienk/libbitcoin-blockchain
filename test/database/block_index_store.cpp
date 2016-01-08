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

BOOST_AUTO_TEST_SUITE(block_index_store_tests)

bool operator==(const block_index_store::entry& a, const block_index_store::entry& b)
{
    return (a.hash == b.hash)
        && (a.chain_id == b.chain_id)
        && (a.height == b.height)
        && (a.header_offset == b.header_offset)
        && (a.merkle_offset == b.merkle_offset);
}

//
// create() method tests
//
BOOST_AUTO_TEST_CASE(create_nop)
{
    block_index_store index;
    BOOST_CHECK_NO_THROW(index.create());
}

//
// start() method tests
//
BOOST_AUTO_TEST_CASE(start_nop)
{
    block_index_store index;
    BOOST_CHECK_NO_THROW(index.start());
}

//
// sync() method tests
//
BOOST_AUTO_TEST_CASE(sync_nop)
{
    block_index_store index;
    BOOST_CHECK_NO_THROW(index.sync());
}

//
// get(const chain_id_type) method tests
//
BOOST_AUTO_TEST_CASE(get_chain_id_missing)
{
    block_index_store index;
    block_index_store::chain_id_type chain_id = 0;
    auto result = index.get(chain_id);
    BOOST_REQUIRE(result == false);
}

BOOST_AUTO_TEST_CASE(get_chain_id_success)
{
    block_index_store index;
    block_index_store::entry entry = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result = index.store(entry);
    BOOST_REQUIRE(store_result);
    BOOST_REQUIRE(store_result.data() == entry);

    auto get_chain_result = index.get(entry.chain_id);
    BOOST_REQUIRE(get_chain_result);
    BOOST_REQUIRE(get_chain_result.data() == entry);
}

//
// get(const chain_id_type, const height_type) method tests
//
BOOST_AUTO_TEST_CASE(get_chain_id_height_missing)
{
    block_index_store index;
    block_index_store::chain_id_type chain_id = 0;
    block_index_store::height_type height = 0;

    auto result = index.get(chain_id, height);
    BOOST_REQUIRE(result == false);
}

BOOST_AUTO_TEST_CASE(get_chain_id_height_success)
{
    block_index_store index;
    block_index_store::entry entry = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result = index.store(entry);
    BOOST_REQUIRE(store_result);
    BOOST_REQUIRE(store_result.data() == entry);

    auto get_chain_height_result = index.get(entry.chain_id, entry.height);
    BOOST_REQUIRE(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result.data() == entry);
}

//
// remove_value(chain_height_iterated_index_data_result&) method tests
//
BOOST_AUTO_TEST_CASE(remove_value_chain_height_iterator_missing)
{
    block_index_store index;
    auto it = index.get(0, 0);
    auto result = index.remove_value(it);
    BOOST_REQUIRE(it == result);
}

BOOST_AUTO_TEST_CASE(remove_value_chain_height_iterator_single_chain_element)
{
    block_index_store index;
    block_index_store::entry entry = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result = index.store(entry);
    BOOST_REQUIRE(store_result);
    BOOST_REQUIRE(store_result.data() == entry);

    auto get_chain_height_result = index.get(entry.chain_id, entry.height);
    BOOST_REQUIRE(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result.data() == entry);

    auto remove_result = index.remove_value(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result != remove_result);
    BOOST_REQUIRE(!remove_result);

    auto get_hash_result = index.get(entry.hash);
    BOOST_REQUIRE(!get_hash_result);
}

BOOST_AUTO_TEST_CASE(remove_value_chain_height_iterator_remove_non_top_chain_element)
{
    block_index_store index;
    block_index_store::entry entry1 = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result1 = index.store(entry1);
    BOOST_REQUIRE(store_result1);
    BOOST_REQUIRE(store_result1.data() == entry1);

    block_index_store::entry entry2 = {
        hash_literal("aaedfda2e31431655245723522367345745233574ad5347ef4f3123643feabbb"),
        5,
        16,
        26475,
        75434
    };

    auto store_result2 = index.store(entry2);
    BOOST_REQUIRE(store_result2);
    BOOST_REQUIRE(store_result2.data() == entry2);

    auto get_chain_height_result = index.get(entry1.chain_id, entry1.height);
    BOOST_REQUIRE(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result.data() == entry1);

    auto remove_result = index.remove_value(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result != remove_result);
//    BOOST_REQUIRE(remove_result);
//    BOOST_REQUIRE(remove_result.data() == entry2);

    auto get_hash_result = index.get(entry1.hash);
    BOOST_REQUIRE(!get_hash_result);

    auto get_chain_result = index.get(entry1.chain_id);
    BOOST_REQUIRE(get_chain_result);
    BOOST_REQUIRE(get_chain_result.data() == entry2);
}

BOOST_AUTO_TEST_CASE(remove_value_chain_height_iterator_remove_top_chain_element)
{
    block_index_store index;
    block_index_store::entry entry1 = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result1 = index.store(entry1);
    BOOST_REQUIRE(store_result1);
    BOOST_REQUIRE(store_result1.data() == entry1);

    block_index_store::entry entry2 = {
        hash_literal("aaedfda2e31431655245723522367345745233574ad5347ef4f3123643feabbb"),
        5,
        16,
        26475,
        75434
    };

    auto store_result2 = index.store(entry2);
    BOOST_REQUIRE(store_result2);
    BOOST_REQUIRE(store_result2.data() == entry2);

    auto get_chain_height_result = index.get(entry2.chain_id, entry2.height);
    BOOST_REQUIRE(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result.data() == entry2);

    auto remove_result = index.remove_value(get_chain_height_result);
    BOOST_REQUIRE(get_chain_height_result != remove_result);
//    BOOST_REQUIRE(!remove_result);

    auto get_hash_result = index.get(entry2.hash);
    BOOST_REQUIRE(!get_hash_result);

    auto get_chain_result = index.get(entry2.chain_id);
    BOOST_REQUIRE(get_chain_result);
    BOOST_REQUIRE(get_chain_result.data() == entry1);
}

//
// get(const hash_digest&) method tests
//
BOOST_AUTO_TEST_CASE(get_hash_missing)
{
    block_index_store index;
    hash_digest hash = null_hash;
    auto result = index.get(hash);
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(get_hash_success)
{
    block_index_store index;
    block_index_store::entry entry = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result = index.store(entry);
    BOOST_REQUIRE(store_result);
    BOOST_REQUIRE(store_result.data() == entry);

    auto get_hash_result = index.get(entry.hash);
    BOOST_REQUIRE(get_hash_result);
    BOOST_REQUIRE(get_hash_result.data() == entry);
}

//
// remove_value(hash_iterated_index_data_result&) method tests
//
BOOST_AUTO_TEST_CASE(remove_value_hash_iterator_missing)
{
    block_index_store index;
    auto it = index.get(null_hash);
    auto result = index.remove_value(it);
    BOOST_REQUIRE(it == result);
}

BOOST_AUTO_TEST_CASE(remove_value_hash_iterator_single_chain_element)
{
    block_index_store index;
    block_index_store::entry entry = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result = index.store(entry);
    BOOST_REQUIRE(store_result);
    BOOST_REQUIRE(store_result.data() == entry);

    auto remove_result = index.remove_value(store_result);
    BOOST_REQUIRE(store_result != remove_result);
//    BOOST_REQUIRE(!remove_result);

    auto get_hash_result = index.get(entry.hash);
    BOOST_REQUIRE(!get_hash_result);
}

BOOST_AUTO_TEST_CASE(remove_value_hash_iterator_remove_non_top_chain_element)
{
    block_index_store index;
    block_index_store::entry entry1 = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result1 = index.store(entry1);
    BOOST_REQUIRE(store_result1);
    BOOST_REQUIRE(store_result1.data() == entry1);

    block_index_store::entry entry2 = {
        hash_literal("aaedfda2e31431655245723522367345745233574ad5347ef4f3123643feabbb"),
        5,
        16,
        26475,
        75434
    };

    auto store_result2 = index.store(entry2);
    BOOST_REQUIRE(store_result2);
    BOOST_REQUIRE(store_result2.data() == entry2);

    auto remove_result = index.remove_value(store_result1);
    BOOST_REQUIRE(store_result1 != remove_result);
//    BOOST_REQUIRE(remove_result);
//    BOOST_REQUIRE(remove_result.data() == entry2);

    auto get_hash_result = index.get(entry1.hash);
    BOOST_REQUIRE(!get_hash_result);

    auto get_chain_result = index.get(entry1.chain_id);
    BOOST_REQUIRE(get_chain_result);
    BOOST_REQUIRE(get_chain_result.data() == entry2);
}

BOOST_AUTO_TEST_CASE(remove_value_hash_iterator_remove_top_chain_element)
{
    block_index_store index;
    block_index_store::entry entry1 = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result1 = index.store(entry1);
    BOOST_REQUIRE(store_result1);
    BOOST_REQUIRE(store_result1.data() == entry1);

    block_index_store::entry entry2 = {
        hash_literal("aaedfda2e31431655245723522367345745233574ad5347ef4f3123643feabbb"),
        5,
        16,
        26475,
        75434
    };

    auto store_result2 = index.store(entry2);
    BOOST_REQUIRE(store_result2);
    BOOST_REQUIRE(store_result2.data() == entry2);

    auto remove_result = index.remove_value(store_result2);
    BOOST_REQUIRE(store_result2 != remove_result);
//    BOOST_REQUIRE(!remove_result);

    auto get_hash_result = index.get(entry2.hash);
    BOOST_REQUIRE(!get_hash_result);

    auto get_chain_result = index.get(entry2.chain_id);
    BOOST_REQUIRE(get_chain_result);
    BOOST_REQUIRE(get_chain_result.data() == entry1);
}

//
// store method tests
//
BOOST_AUTO_TEST_CASE(store_empty_success)
{
    block_index_store index;
    block_index_store::entry entry = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result = index.store(entry);
    BOOST_REQUIRE(store_result);
    BOOST_REQUIRE(store_result.data() == entry);
}

BOOST_AUTO_TEST_CASE(store_twice_same_chain_id_success)
{
    block_index_store index;
    block_index_store::entry entry1 = {
        hash_literal("eefa5d23968584be9d8d064bcf99c24666e4d53b8e10e5097bd6f7b5059d7c53"),
        5,
        15,
        12341,
        43214
    };

    auto store_result1 = index.store(entry1);
    BOOST_REQUIRE(store_result1);
    BOOST_REQUIRE(store_result1.data() == entry1);

    block_index_store::entry entry2 = {
        hash_literal("aaedfda2e31431655245723522367345745233574ad5347ef4f3123643feabbb"),
        5,
        16,
        26475,
        75434
    };

    auto store_result2 = index.store(entry2);
    BOOST_REQUIRE(store_result2);
    BOOST_REQUIRE(store_result2.data() == entry2);
}

BOOST_AUTO_TEST_SUITE_END()
