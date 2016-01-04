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

template<typename Trie>
struct insert_status
{
    typename Trie::iterator pos;
    bool status;
};

template<std::size_t Size,
    typename Key,
    typename Value,
    typename Trie = modified_patricia_trie<
        Size,
        Key,
        Value,
        std::greater<Key>,
        std::greater<Value>,
        std::allocator<trie_structure_node<Key, Value, std::greater<Key>>>,
        std::allocator<trie_value_node<Value, trie_structure_node<Key, Value, std::greater<Key>>>>>>
static std::vector<insert_status<Trie>> insert_equal(Trie& tree,
    std::vector<std::tuple<Value, Key, binary_type>>& value_key_tuples)
{
    std::vector<insert_status<Trie>> result;

    // insert
    for (auto query : value_key_tuples)
    {
        auto insert_result = tree.insert_equal(std::get<2>(query),
            std::get<1>(query), std::get<0>(query));

        BOOST_REQUIRE(insert_result.second);
        BOOST_REQUIRE((*insert_result.first) == std::get<0>(query));

        result.push_back({ insert_result.first, insert_result.second });
    }

    return result;
}

template<std::size_t Size,
    typename Key,
    typename Value,
    typename Trie = modified_patricia_trie<
        Size,
        Key,
        Value,
        std::greater<Key>,
        std::greater<Value>,
        std::allocator<trie_structure_node<Key, Value, std::greater<Key>>>,
        std::allocator<trie_value_node<Value, trie_structure_node<Key, Value, std::greater<Key>>>>>>
static void test_insert_equal_with_unique_keys_succeeds(
    Trie& tree, std::vector<std::tuple<Value, Key, binary_type>>& value_key_tuples)
{
    insert_equal<Size, Key, Value, Trie>(tree, value_key_tuples);

    // find
    for (auto query : value_key_tuples)
    {
        auto find_result = tree.find_equal(std::get<2>(query), std::get<1>(query));

        BOOST_REQUIRE(find_result.first != find_result.second);
        BOOST_REQUIRE(*(find_result.first) == std::get<0>(query));
        BOOST_REQUIRE(find_result.first.get_primary_key() == std::get<2>(query));
        BOOST_REQUIRE(find_result.first.get_secondary_key() == std::get<1>(query));

        auto inc = ++find_result.first;
        BOOST_REQUIRE(inc == find_result.second);
    }
}

template<std::size_t Size,
    typename Key,
    typename Value,
    typename Trie = modified_patricia_trie<
        Size,
        Key,
        Value,
        std::greater<Key>,
        std::greater<Value>,
        std::allocator<trie_structure_node<Key, Value, std::greater<Key>>>,
        std::allocator<trie_value_node<Value, trie_structure_node<Key, Value, std::greater<Key>>>>>>
static void test_insert_unique_matches_expectation(Trie& tree,
    std::vector<std::tuple<bool, Value, Key, binary_type>>& query_tuples)
{
    // insert
    for (auto query : query_tuples)
    {
        auto insert_result = tree.insert_unique(std::get<3>(query),
            std::get<2>(query), std::get<1>(query));

        BOOST_REQUIRE(insert_result.second == std::get<0>(query));

        if (insert_result.second)
        {
            BOOST_REQUIRE(insert_result.first.get_primary_key() == std::get<3>(query));
            BOOST_REQUIRE(insert_result.first.get_secondary_key() == std::get<2>(query));
            BOOST_REQUIRE((*insert_result.first) == std::get<1>(query));
        }
    }

    // find
    for (auto query : query_tuples)
    {
        auto find_result = tree.find_equal(std::get<3>(query), std::get<2>(query));

        if (std::get<0>(query))
        {
            BOOST_REQUIRE(find_result.first != find_result.second);
            BOOST_REQUIRE(*(find_result.first) == std::get<1>(query));
            BOOST_REQUIRE(find_result.first.get_primary_key() == std::get<3>(query));
            BOOST_REQUIRE(find_result.first.get_secondary_key() == std::get<2>(query));
            BOOST_REQUIRE(++find_result.first == find_result.second);
        }
        else
        {
            if (std::get<3>(query).size() != 0)
            {
                BOOST_REQUIRE(find_result.first != find_result.second);
            }
            else
            {
                BOOST_REQUIRE(find_result.first == find_result.second);
            }
        }
    }
}

template<std::size_t Size,
    typename Key,
    typename Value,
    typename Trie = modified_patricia_trie<
        Size,
        Key,
        Value,
        std::greater<Key>,
        std::greater<Value>,
        std::allocator<trie_structure_node<Key, Value, std::greater<Key>>>,
        std::allocator<trie_value_node<Value, trie_structure_node<Key, Value, std::greater<Key>>>>>>
static void verify_iterator_range(std::vector<Value>& expected,
    typename Trie::iterator_range& range)
{
    auto expected_it = expected.begin();
    auto actual_it = range.first;

    while ((expected_it != expected.end()) && (actual_it != range.second))
    {
        BOOST_REQUIRE(*actual_it == *expected_it);

        ++expected_it;
        ++actual_it;
    }

    BOOST_REQUIRE(expected_it == expected.end());
    BOOST_REQUIRE(actual_it == range.second);
}

template<std::size_t Size,
    typename Key,
    typename Value,
    typename Trie = modified_patricia_trie<
        Size,
        Key,
        Value,
        std::greater<Key>,
        std::greater<Value>,
        std::allocator<trie_structure_node<Key, Value, std::greater<Key>>>,
        std::allocator<trie_value_node<Value, trie_structure_node<Key, Value, std::greater<Key>>>>>>
static void verify_iterator_range(
    std::vector<std::tuple<Value, Key, binary_type>>& expected,
    typename Trie::iterator_range& range)
{
    auto expected_it = expected.begin();
    auto actual_it = range.first;

    while ((expected_it != expected.end()) && (actual_it != range.second))
    {
//        if (*actual_it != (*expected_it).first)
//        {
//            std::cout << "actual: " << *actual_it << std::endl;
//            std::cout << "expect: " << (*expected_it).first << std::endl;
//        }

        BOOST_REQUIRE(*actual_it == std::get<0>(*expected_it));
        BOOST_REQUIRE(actual_it.get_secondary_key() == std::get<1>(*expected_it));
        BOOST_REQUIRE(actual_it.get_primary_key() == std::get<2>(*expected_it));

        ++expected_it;
        ++actual_it;
    }

    BOOST_REQUIRE(expected_it == expected.end());

    bool exhausted_actual_values = (actual_it == range.second);

//    int i = 0;
//    while ((actual_it != range.second) && (i < 5))
//    {
//        std::cout << "actual: " << *actual_it << std::endl;
//        ++actual_it;
//        i++;
//    }

    BOOST_REQUIRE(exhausted_actual_values);
}

template<typename Key, typename Value, typename Comparer = std::greater<Key>>
static std::pair<std::allocator<trie_structure_node<Key, Value, Comparer>>,
    std::allocator<trie_value_node<Value, trie_structure_node<Key, Value,
    Comparer>>>> get_allocators()
{
    std::allocator<trie_structure_node<Key, Value, Comparer>> structure_allocator;
    std::allocator<trie_value_node<Value, trie_structure_node<Key, Value, Comparer>>> value_allocator;

    return std::make_pair(structure_allocator, value_allocator);
}

BOOST_AUTO_TEST_SUITE(modified_patricia_trie_tests)

//
// constructor new/delete tests
//
BOOST_AUTO_TEST_CASE(constructor_allocator_args)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t>* tree= new modified_patricia_trie<32, uint32_t, uint32_t>(
        alloc_pair.first, alloc_pair.second);

    delete tree;

    BOOST_REQUIRE(true);
}

//
// insert_equal tests
//
BOOST_AUTO_TEST_CASE(insert_equal_zero_length_key_fails)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first,
        alloc_pair.second);

    binary_type primary(0, bc::data_chunk {});
    uint32_t secondary = 2;
    uint32_t value = 17;

    auto insert_result = tree.insert_equal(primary, secondary, value);

    BOOST_REQUIRE(!insert_result.second);

    auto find_result = tree.find_equal(primary, secondary);

    BOOST_REQUIRE(find_result.first == find_result.second);
}

BOOST_AUTO_TEST_CASE(insert_equal_key_length_exceeds_size_fails)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first,
        alloc_pair.second);

    binary_type primary(40, bc::data_chunk {
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1
    });

    uint32_t secondary = 2;
    uint32_t value = 17;

    auto insert_result = tree.insert_equal(primary, secondary, value);

    BOOST_REQUIRE(!insert_result.second);

    auto find_result = tree.find_equal(primary, secondary);

    BOOST_REQUIRE(find_result.first == find_result.second);
}

BOOST_AUTO_TEST_CASE(insert_equal_nonzero_length_key_succeeds)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    binary_type key(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });
    int value = 17;

    auto insert_result = tree.insert_equal(key, 1, value);

    BOOST_REQUIRE(insert_result.second);
    BOOST_REQUIRE((insert_result.first).get_primary_key() == key);
    BOOST_REQUIRE((*insert_result.first) == value);

    auto find_result = tree.find_equal(key, 1);

    BOOST_REQUIRE(find_result.first != find_result.second);

    BOOST_REQUIRE((find_result.first).get_primary_key() == key);
    BOOST_REQUIRE((*find_result.first) == value);
}

BOOST_AUTO_TEST_CASE(insert_equal_multiple_same_key_succeeds)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first,
        alloc_pair.second);

    binary_type primary(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });
    uint32_t secondary = 1;
    std::vector<uint32_t> values = { 17, 112, 34, 56 };

    for (auto value : values)
    {
        auto insert_result = tree.insert_equal(primary, secondary, value);

        BOOST_REQUIRE(insert_result.second);
        BOOST_REQUIRE(*(insert_result.first) == value);
    }

    auto find_result = tree.find_equal(primary, secondary);

    BOOST_REQUIRE(find_result.first != find_result.second);

    std::vector<int> expected = { 17, 34, 56, 112 };
    auto vit = expected.begin();
    auto rit = find_result.first;
    auto end = expected.end();

    for (; (rit != find_result.second) && (vit != end); ++rit, ++vit)
    {
        BOOST_CHECK(*vit == *rit);
    }

    BOOST_REQUIRE(vit == expected.end());
    BOOST_REQUIRE(rit == find_result.second);
}

BOOST_AUTO_TEST_CASE(insert_equal_fixed_secondary_key_add_leaf_node)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(17,  1,  binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_equal_with_unique_keys_succeeds<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_equal_fixed_secondary_key_add_intermediary_node)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(17,  1,  binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_equal_with_unique_keys_succeeds<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_equal_fixed_secondary_key_match_existing_intermediary_node)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(17,  1,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1,  binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_equal_with_unique_keys_succeeds<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_equal_fixed_secondary_key_multiple_unique_keys)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(17,  1,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1,  binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(34,  1,  binary_type(22, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(56,  1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(435, 1,  binary_type(15, bc::data_chunk { 0x0A, 0x5B, 0x00 })),
        std::make_tuple(234, 1,  binary_type(18, bc::data_chunk { 0x00, 0x02, 0x00 }))
    };

    test_insert_equal_with_unique_keys_succeeds<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_equal_multiple_unique_keys)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 2,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(17,  1,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  2,  binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(34,  1,  binary_type(22, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(56,  2,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(435, 1,  binary_type(15, bc::data_chunk { 0x0A, 0x5B, 0x00 })),
        std::make_tuple(234, 2,  binary_type(18, bc::data_chunk { 0x00, 0x02, 0x00 }))
    };

    test_insert_equal_with_unique_keys_succeeds<32>(tree, value_key_tuples);
}

//
// insert_unique tests
//
BOOST_AUTO_TEST_CASE(insert_unique_zero_length_key_fails)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    binary_type primary(0, bc::data_chunk {});
    uint32_t secondary = 10;
    uint32_t value = 17;

    auto insert_result = tree.insert_unique(primary, secondary, value);

    BOOST_REQUIRE(!insert_result.second);

    auto find_result = tree.find_equal(primary, secondary);

    BOOST_REQUIRE(find_result.first == find_result.second);
}

BOOST_AUTO_TEST_CASE(insert_unique_key_length_exceeds_size_fails)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first,
        alloc_pair.second);

    binary_type primary(40, bc::data_chunk {
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1
    });

    uint32_t secondary = 2;
    uint32_t value = 17;

    auto insert_result = tree.insert_unique(primary, secondary, value);

    BOOST_REQUIRE(!insert_result.second);

    auto find_result = tree.find_equal(primary, secondary);

    BOOST_REQUIRE(find_result.first == find_result.second);
}

BOOST_AUTO_TEST_CASE(insert_unique_collision_returns_error)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(true,   97,     1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(false,  112,    1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_unique_fixed_secondary_key_add_leaf_node)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(true,   97,     1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true,   112,    1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true,   17,     1,  binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_unique_fixed_secondary_key_add_intermediary_node)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(true,   97,     1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true,   17,     1,  binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true,   112,    1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation<32>(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_unique_fixed_secondary_key_match_existing_intermediary_node)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(true,   97,     1,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true,   112,    1,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(true,   17,     1,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true,   26,     1,  binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation<32>(tree, value_key_tuples);
}

//
// find_prefix tests
//
BOOST_AUTO_TEST_CASE(find_prefixed_matches_none)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    insert_equal<32>(tree, value_key_tuples);

    binary_type primary(8, bc::data_chunk { 0xBB });
    uint32_t secondary = 1;

    auto query_result = tree.find_prefixed(primary, secondary);

    BOOST_REQUIRE(query_result.first == query_result.second);
}

BOOST_AUTO_TEST_CASE(find_prefixed_matches_all)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    binary_type query(8, bc::data_chunk { 0xAA });

    std::vector<uint32_t> expected = { 97, 26, 112, 17 };

    insert_equal<32>(tree, value_key_tuples);

    auto query_result = tree.find_prefixed(query, 1);

    BOOST_REQUIRE(query_result.first != query_result.second);

    verify_iterator_range<32, uint32_t, uint32_t>(expected, query_result);
}

BOOST_AUTO_TEST_CASE(find_prefix_matches_returns_subset)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    binary_type query(10, bc::data_chunk { 0xAA, 0xC0 });

    std::vector<uint32_t> expected = { 26, 112 };

    insert_equal<32>(tree, value_key_tuples);

    auto query_result = tree.find_prefixed(query, 1);

    BOOST_REQUIRE(query_result.first != query_result.second);

    verify_iterator_range<32, uint32_t, uint32_t>(expected, query_result);
}

//
// remove_equal primary/secondary key tests
//
BOOST_AUTO_TEST_CASE(remove_equal_from_empty_tree)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    binary_type primary(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });
    uint32_t secondary = 1;

    bool success = tree.remove_equal(primary, secondary);

    BOOST_REQUIRE(!success);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_key_zero_length_returns_false)
{
    binary_type primary(0, bc::data_chunk { 0x00 });
    uint32_t secondary = 1;

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17, 1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(26, 1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(primary, secondary);

    BOOST_REQUIRE(!success);

    auto range = tree.find_secondary_key_bounds(secondary);

    verify_iterator_range<32, uint32_t, uint32_t>(value_key_tuples, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_key_not_present_returns_false)
{
    binary_type primary(4, bc::data_chunk { 0xFF });
    uint32_t secondary = 1;

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17, 1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(26, 1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(primary, secondary);

    BOOST_REQUIRE(!success);

    auto range = tree.find_secondary_key_bounds(secondary);

    verify_iterator_range<32, uint32_t, uint32_t>(value_key_tuples, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_leaf_of_root_with_single_value)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17, 1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(26, 1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(success);

    auto range = tree.find_secondary_key_bounds(std::get<1>(value_key_tuples[0]));

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected(
        ++(value_key_tuples.begin()), value_key_tuples.end());

    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_leaf_with_multiple_values)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17, 1, binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_tuple(523, 1, binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_tuple(26, 1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(success);

    auto range = tree.find_secondary_key_bounds(std::get<1>(value_key_tuples[0]));

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected(
        ++(++(value_key_tuples.begin())), value_key_tuples.end());

    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_unique_key)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(235, 1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(success);

    auto range = tree.find_secondary_key_bounds(std::get<1>(value_key_tuples[0]));

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected = {
        std::make_tuple(235, 1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_single_key_multiply_inserted)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(success);

    auto range = tree.find_secondary_key_bounds(std::get<1>(value_key_tuples[0]));

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected = {
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_multiple_secondary_keys_node_is_not_removed)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(97,  2, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  2, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(success);

    auto range_1 = tree.find_secondary_key_bounds(1);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected_1 = {
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected_1, range_1);

    auto range_2 = tree.find_secondary_key_bounds(2);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected_2 = {
        std::make_tuple(97,  2, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(17,  2, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected_2, range_2);
}

BOOST_AUTO_TEST_CASE(remove_equal_multiple_secondary_keys_node_removed)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(97,  2, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  2, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(59,  2, binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    bool success = tree.remove_equal(std::get<2>(value_key_tuples[3]),
        std::get<1>(value_key_tuples[3]));

    BOOST_REQUIRE(success);

    auto range_1 = tree.find_secondary_key_bounds(1);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected_1 = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected_1, range_1);

    auto range_2 = tree.find_secondary_key_bounds(2);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected_2 = {
        std::make_tuple(97,  2, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(59,  2, binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected_2, range_2);
}

//
// remove_equal secondary key tests
//
BOOST_AUTO_TEST_CASE(remove_equal2_from_empty_tree)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    uint32_t secondary = 1;

    BOOST_REQUIRE(!tree.remove_equal(secondary));
}

BOOST_AUTO_TEST_CASE(remove_equal2_from_nonempty_tree_empty_secondary_key)
{
    uint32_t secondary = 2;

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(59,  1, binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    BOOST_REQUIRE(!tree.remove_equal(secondary));

    auto range = tree.find_secondary_key_bounds(1);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected = {
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(59,  1, binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal2_from_nonempty_tree_single_secondary_key_empties_tree)
{
    uint32_t secondary = 1;

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_tuple(59,  1, binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    BOOST_REQUIRE(tree.remove_equal(secondary));

    auto range = tree.find_secondary_key_bounds(secondary);

    BOOST_REQUIRE(range.first == range.second);
}

BOOST_AUTO_TEST_CASE(remove_equal2_from_nonempty_tree_multiple_secondary_keys)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    uint32_t secondary = 1;

    BOOST_REQUIRE(!tree.remove_equal(secondary));
}

//BOOST_AUTO_TEST_CASE(remove_prefix_from_key_zero_length_returns_false)
//{
//    binary_type query(0, bc::data_chunk { 0x00 });
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(query);
//
//    BOOST_REQUIRE(!success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    verify_iterator_range<32, uint32_t, uint32_t><uint32_t, uint32_t, std::less<int>>(value_key_tuples, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_key_not_present_returns_false)
//{
//    binary_type query(4, bc::data_chunk { 0xFF });
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(query);
//
//    BOOST_REQUIRE(!success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    verify_iterator_range<32, uint32_t, uint32_t>(value_key_tuples, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_single_matching_key)
//{
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(value_key_tuples[0].second);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_single_matching_prefix)
//{
//    binary_type query(2, bc::data_chunk { 0x40 });
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(query);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_single_node_matching_key_with_multiplicity)
//{
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(value_key_tuples[0].second);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_single_node_matching_prefix_with_multiplicity)
//{
//    binary_type query(2, bc::data_chunk { 0x40 });
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(query);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_key)
//{
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
//        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
//        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
//        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(value_key_tuples[0].second);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_prefix)
//{
//    binary_type query(2, bc::data_chunk { 0x40 });
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
//        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
//        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
//        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(query);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_key_with_multiplicity)
//{
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
//        std::make_pair(27,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
//        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
//        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
//        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
//        std::make_pair(66,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
//        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
//        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(value_key_tuples[0].second);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
//        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
//        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}
//
//BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_prefix_with_multiplicity)
//{
//    binary_type query(2, bc::data_chunk { 0x40 });
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> value_key_tuples = {
//        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
//        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
//        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
//        std::make_pair(27,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
//        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
//        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
//        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
//        std::make_pair(66,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
//        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
//        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
//    };
//
//    auto alloc_pair = get_allocators<uint32_t, uint32_t>();
//
//    modified_patricia_trie<uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);
//
//    auto insert_results = insert_equal<32>(tree, value_key_tuples);
//
//    bool success = tree.remove_prefix(query);
//
//    BOOST_REQUIRE(success);
//
//    auto range = std::make_pair(tree.begin(), tree.end());
//
//    std::vector<std::pair<uint32_t, uint32_t, binary_type>> expected = {
//        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
//        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
//        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
//        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
//    };
//
//    verify_iterator_range<32, uint32_t, uint32_t>(expected, range);
//}

//
// remove_value tests
//
BOOST_AUTO_TEST_CASE(remove_value_from_empty_tree)
{
    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto range = tree.find_secondary_key_bounds(1);

    auto iterator = tree.remove_value(range.first);

    BOOST_REQUIRE(iterator == range.first);
}

BOOST_AUTO_TEST_CASE(remove_value_from_leaf_of_root_with_single_value)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17,  1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[0].pos);

    BOOST_REQUIRE(iterator != insert_results[0].pos);

    auto find_result = tree.find_prefixed(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(find_result.first == find_result.second);

    auto bounds = tree.find_secondary_key_bounds(1);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected(
            ++(value_key_tuples.begin()), value_key_tuples.end());

    verify_iterator_range<32, uint32_t, uint32_t>(expected, bounds);
}

BOOST_AUTO_TEST_CASE(remove_value_from_leaf_with_multiple_values_head)
{
    uint32_t secondary = 1;

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17,  secondary, binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_tuple(523, secondary, binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_tuple(26,  secondary, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[0].pos);

    BOOST_REQUIRE(iterator != insert_results[0].pos);

    auto bounds = tree.find_secondary_key_bounds(secondary);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected = {
        std::make_tuple(26,  secondary, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_tuple(523, secondary, binary_type(10, bc::data_chunk { 0xAB, 0xA0 }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected, bounds);
}

BOOST_AUTO_TEST_CASE(remove_value_from_leaf_with_multiple_values_tail)
{
    uint32_t secondary = 1;

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(17,  secondary, binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_tuple(523, secondary, binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_tuple(26,  secondary, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[1].pos);

    BOOST_REQUIRE(iterator != insert_results[1].pos);

    auto bounds = tree.find_secondary_key_bounds(secondary);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected = {
        std::make_tuple(26,  secondary, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_tuple(17,  secondary, binary_type(10, bc::data_chunk { 0xAB, 0xA0 }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected, bounds);
}

BOOST_AUTO_TEST_CASE(remove_value_from_unique_key)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[0].pos);

    BOOST_REQUIRE(iterator != insert_results[0].pos);

    auto find_result = tree.find_prefixed(std::get<2>(value_key_tuples[0]),
        std::get<1>(value_key_tuples[0]));

    BOOST_REQUIRE(find_result.first == find_result.second);

    auto bounds = tree.find_secondary_key_bounds(1);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected(
        ++(value_key_tuples.begin()), value_key_tuples.end());

    verify_iterator_range<32, uint32_t, uint32_t>(expected, bounds);
}

BOOST_AUTO_TEST_CASE(remove_value_from_single_key_multiply_inserted)
{
    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> value_key_tuples = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(97,  1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<uint32_t, uint32_t>();

    modified_patricia_trie<32, uint32_t, uint32_t> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal<32>(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[1].pos);

    BOOST_REQUIRE(iterator != insert_results[1].pos);
    BOOST_REQUIRE(iterator.get_primary_key() == std::get<2>(value_key_tuples[0]));
    BOOST_REQUIRE(iterator.get_secondary_key() == std::get<1>(value_key_tuples[0]));

    auto bounds = tree.find_secondary_key_bounds(1);

    std::vector<std::tuple<uint32_t, uint32_t, binary_type>> expected = {
        std::make_tuple(235, 1, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(17,  1, binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(26,  1, binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_tuple(112, 1, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 }))
    };

    verify_iterator_range<32, uint32_t, uint32_t>(expected, bounds);
}

BOOST_AUTO_TEST_SUITE_END()

