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

static std::shared_ptr<red_black_tree<int, int, std::less<int>,
    std::allocator<red_black_node<int, int>>>> get_tree()
{
    std::allocator<red_black_node<int, int>> allocator;
    return std::make_shared<red_black_tree<int, int>>(allocator);
}

static std::shared_ptr<red_black_tree<int, int, std::less<int>,
    std::allocator<red_black_node<int, int>>>> get_populated_tree()
{
    auto tree = get_tree();

    tree->add(tree->create_node(15, 1));
    tree->add(tree->create_node(20, 2));
    tree->add(tree->create_node(25, 3));
    tree->add(tree->create_node(30, 4));
    tree->add(tree->create_node(35, 5));
    tree->add(tree->create_node(40, 6));
    tree->add(tree->create_node(45, 7));

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
    auto store = new red_black_store<int, int>(get_tree());

    delete store;

    BOOST_REQUIRE(true);
}

//
// add tests
//
BOOST_AUTO_TEST_CASE(add_new_key_success)
{
    auto store = std::make_shared<red_black_store<int, int>>(get_populated_tree());

    BOOST_REQUIRE_EQUAL(true, store->add(23, 17, false));
}

BOOST_AUTO_TEST_CASE(add_existing_key_no_replace_failure)
{
    auto store = std::make_shared<red_black_store<int, int>>(get_populated_tree());

    BOOST_REQUIRE_EQUAL(false, store->add(25, 100, false));
}

BOOST_AUTO_TEST_CASE(add_existing_key_replace_success)
{
    auto store = std::make_shared<red_black_store<int, int>>(get_populated_tree());

    BOOST_REQUIRE_EQUAL(false, store->add(25, 123, true));
}

//
// remove tests
//
BOOST_AUTO_TEST_CASE(remove_nonexistent_key_failure)
{
    auto store = std::make_shared<red_black_store<int, int>>(get_populated_tree());

    BOOST_REQUIRE_EQUAL(false, store->remove(22));
}

BOOST_AUTO_TEST_CASE(remove_existing_key_success)
{
    auto store = std::make_shared<red_black_store<int, int>>(get_populated_tree());

    BOOST_REQUIRE_EQUAL(false, store->remove(25));
}

//
// retrieve tests
//
BOOST_AUTO_TEST_CASE(retrieve_nonexisting_key_failure)
{
    auto store = std::make_shared<red_black_store<int, bool>>(get_populated_tree());

    auto result = store->retrieve(22);

    BOOST_REQUIRE_EQUAL(false, result.second);
}

BOOST_AUTO_TEST_CASE(retrieve_existing_key_success)
{
    auto store = std::make_shared<red_black_store<int, bool>>(get_populated_tree());

    BOOST_REQUIRE_EQUAL(false, store->add(25, 87545, true));

    auto result = store->retrieve(25);

    BOOST_REQUIRE_EQUAL(true, result.second);
    BOOST_REQUIRE_EQUAL(87545, result.first);
}

BOOST_AUTO_TEST_SUITE_END()

