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

template <typename Comparer>
static red_black_tree<int, int, Comparer, std::allocator<red_black_node<int, int>>>* get_tree()
{
    std::allocator<red_black_node<int, int>> allocator;
    return new red_black_tree<int, int, Comparer>(allocator);
}

template <typename Comparer>
static red_black_tree<int, int, Comparer, std::allocator<red_black_node<int, int>>>* get_populated_tree(
    std::vector<std::pair<int, int>> values = {
        std::make_pair(15, 1),
        std::make_pair(20, 2),
        std::make_pair(25, 3),
        std::make_pair(30, 4),
        std::make_pair(35, 5),
        std::make_pair(40, 6),
        std::make_pair(45, 7)
    })
{
    auto tree = get_tree<Comparer>();

    for (auto it : values)
        tree->add(tree->create_node(it.first, it.second));

    return tree;
}

BOOST_AUTO_TEST_SUITE(red_black_store_tests)

//
// constructor new/delete tests
//
BOOST_AUTO_TEST_CASE(constructor_allocator_arg)
{
    std::allocator<red_black_node<int, int>> allocator;
    auto store = new red_black_store<int, int>(allocator);

    delete store;

    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_CASE(constructor_tree_arg)
{
    auto store = new red_black_store<int, int, std::greater<int>>(get_tree<std::greater<int>>());

    delete store;

    BOOST_REQUIRE(true);
}

//
// add tests
//
BOOST_AUTO_TEST_CASE(add_new_key_success)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    auto result = store->add(23, 17, false);

    BOOST_REQUIRE_EQUAL(true, result.second);
    BOOST_REQUIRE_EQUAL(23, result.first.get_key());
    BOOST_REQUIRE_EQUAL(17, (*result.first));
}

BOOST_AUTO_TEST_CASE(add_existing_key_no_replace_failure)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    auto result = store->add(25, 100, false);

    BOOST_REQUIRE_EQUAL(false, result.second);
}

BOOST_AUTO_TEST_CASE(add_existing_key_replace_success)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    auto result = store->add(25, 123, true);

    BOOST_REQUIRE_EQUAL(true, result.second);
    BOOST_REQUIRE_EQUAL(25, result.first.get_key());
    BOOST_REQUIRE_EQUAL(123, (*result.first));
}

//
// remove tests
//
BOOST_AUTO_TEST_CASE(remove_nonexistent_key_failure)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    BOOST_REQUIRE_EQUAL(false, store->remove(22));
}

BOOST_AUTO_TEST_CASE(remove_existing_key_success)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    BOOST_REQUIRE_EQUAL(true, store->remove(25));
}

//
// retrieve tests
//
BOOST_AUTO_TEST_CASE(retrieve_nonexisting_key_failure)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    auto result = store->retrieve(22);

    BOOST_REQUIRE_EQUAL(false, result.second);
}

BOOST_AUTO_TEST_CASE(retrieve_existing_key_success)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    BOOST_REQUIRE_EQUAL(true, store->add(25, 87545, true).second);

    auto result = store->retrieve(25);

    BOOST_REQUIRE_EQUAL(true, result.second);
    BOOST_REQUIRE_EQUAL(87545, (*result.first));
    BOOST_REQUIRE_EQUAL(25, result.first.get_key());
}

//
// retrieve_greater_equal tests
//
BOOST_AUTO_TEST_CASE(retrieve_greater_equal_empty_tree_failure)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    auto result = store->retrieve_greater_equal(22);

    BOOST_REQUIRE_EQUAL(false, result.second);
}

BOOST_AUTO_TEST_CASE(retrieve_greater_equal_greater_than_existing_key_failure)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    BOOST_REQUIRE_EQUAL(true, store->add(10, 87545, true).second);

    auto result = store->retrieve_greater_equal(22);

    BOOST_REQUIRE_EQUAL(false, result.second);
}

BOOST_AUTO_TEST_CASE(retrieve_greater_equal_existing_key_success)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>());

    BOOST_REQUIRE_EQUAL(true, store->add(25, 87545, true).second);

    auto result = store->retrieve_greater_equal(25);

    BOOST_REQUIRE_EQUAL(true, result.second);
    BOOST_REQUIRE_EQUAL(87545, (*result.first));
    BOOST_REQUIRE_EQUAL(25, result.first.get_key());
}

BOOST_AUTO_TEST_CASE(retrieve_greater_equal_less_than_existing_key_success)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    BOOST_REQUIRE_EQUAL(true, store->add(25, 87545, true).second);

    auto result = store->retrieve_greater_equal(10);

    BOOST_REQUIRE_EQUAL(true, result.second);
    BOOST_REQUIRE_EQUAL(87545, (*result.first));
    BOOST_REQUIRE_EQUAL(25, result.first.get_key());
}

//
// begin_end tests
//
BOOST_AUTO_TEST_CASE(begin_end_empty_tree_returns_begin_equals_end)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    auto begin = store->begin();
    auto end = store->end();

    BOOST_REQUIRE(begin == end);
}

BOOST_AUTO_TEST_CASE(begin_end_nonempty_tree_returns_expected)
{
    std::vector<std::pair<int, int>> ordered_insertion = {
        std::make_pair(15, 212),
        std::make_pair(19, 12),
        std::make_pair(20, 45),
        std::make_pair(31, 64),
        std::make_pair(46, 235),
        std::make_pair(69, 111)
    };

    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>(ordered_insertion));

    auto begin = store->begin();
    auto end = store->end();

    BOOST_REQUIRE(begin != end);

    for (auto& pair : ordered_insertion)
    {
        BOOST_REQUIRE(begin.get_key() == pair.first);
        BOOST_REQUIRE((*begin) == pair.second);
        begin++;
    }

    BOOST_REQUIRE(begin == end);
}

//
// const begin_end tests
//
BOOST_AUTO_TEST_CASE(const_begin_end_empty_tree_returns_begin_equals_end)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    auto begin = store->cbegin();
    auto end = store->cend();

    BOOST_REQUIRE(begin == end);
}

BOOST_AUTO_TEST_CASE(const_begin_end_nonempty_tree_returns_expected)
{
    std::vector<std::pair<int, int>> ordered_insertion = {
        std::make_pair(15, 212),
        std::make_pair(19, 12),
        std::make_pair(20, 45),
        std::make_pair(31, 64),
        std::make_pair(46, 235),
        std::make_pair(69, 111)
    };

    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>(ordered_insertion));

    auto begin = store->cbegin();
    auto end = store->cend();

    BOOST_REQUIRE(begin != end);

    for (auto& pair : ordered_insertion)
    {
        BOOST_REQUIRE(begin.get_key() == pair.first);
        BOOST_REQUIRE((*begin) == pair.second);
        begin++;
    }

    BOOST_REQUIRE(begin == end);
}

//
// reverse begin/end tests
//
BOOST_AUTO_TEST_CASE(reverse_begin_end_empty_tree_returns_begin_equals_end)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    auto begin = store->rbegin();
    auto end = store->rend();

    BOOST_REQUIRE(begin == end);
}

BOOST_AUTO_TEST_CASE(reverse_begin_end_nonempty_tree_returns_expected)
{
    std::vector<std::pair<int, int>> ordered_insertion = {
        std::make_pair(15, 212),
        std::make_pair(19, 12),
        std::make_pair(20, 45),
        std::make_pair(31, 64),
        std::make_pair(46, 235),
        std::make_pair(69, 111)
    };

    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>(ordered_insertion));

    auto begin = store->rbegin();
    auto end = store->rend();

    BOOST_REQUIRE(begin != end);

    for (auto it = ordered_insertion.rbegin(); it != ordered_insertion.rend(); ++it)
    {
        BOOST_REQUIRE(begin.get_key() == (*it).first);
        BOOST_REQUIRE((*begin) == (*it).second);
        ++begin;
    }

    BOOST_REQUIRE(begin == end);
}

//
// reverse const begin/end tests
//
BOOST_AUTO_TEST_CASE(const_reverse_begin_end_empty_tree_returns_begin_equals_end)
{
    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_tree<std::greater<int>>());

    auto begin = store->crbegin();
    auto end = store->crend();

    BOOST_REQUIRE(begin == end);
}

BOOST_AUTO_TEST_CASE(const_reverse_begin_end_nonempty_tree_returns_expected)
{
    std::vector<std::pair<int, int>> ordered_insertion = {
        std::make_pair(15, 212),
        std::make_pair(19, 12),
        std::make_pair(20, 45),
        std::make_pair(31, 64),
        std::make_pair(46, 235),
        std::make_pair(69, 111)
    };

    auto store = std::make_shared<red_black_store<int, int, std::greater<int>>>(
        get_populated_tree<std::greater<int>>(ordered_insertion));

    auto begin = store->crbegin();
    auto end = store->crend();

    BOOST_REQUIRE(begin != end);

    for (auto it = ordered_insertion.rbegin(); it != ordered_insertion.rend(); ++it)
    {
        BOOST_REQUIRE(begin.get_key() == (*it).first);
        BOOST_REQUIRE((*begin) == (*it).second);
        ++begin;
    }

    BOOST_REQUIRE(begin == end);
}

BOOST_AUTO_TEST_SUITE_END()

