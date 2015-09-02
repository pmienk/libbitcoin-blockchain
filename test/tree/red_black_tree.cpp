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

BOOST_AUTO_TEST_SUITE(red_black_tree_tests)

//
// constructor new/delete tests
//
BOOST_AUTO_TEST_CASE(constructor_allocator_args)
{
    std::allocator<red_black_node<int, bool>> allocator;
    red_black_tree<int, bool>* tree = new red_black_tree<int, bool>(allocator);

    BOOST_REQUIRE(tree->nil() == tree->root());

    delete tree;

    BOOST_REQUIRE(true);
}

//
// add tests
//
BOOST_AUTO_TEST_CASE(add_first_node_sets_root)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto node = tree->create_node(15, false);

    BOOST_REQUIRE(node->is_red == true);

    tree->add(node);

    BOOST_REQUIRE(tree->root() == node);
    BOOST_REQUIRE(tree->nil() != node);
    BOOST_REQUIRE(node->is_red == false);
    BOOST_REQUIRE(node->key == 15);
    BOOST_REQUIRE(node->value == false);
}

BOOST_AUTO_TEST_CASE(add_sequence_increasing)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(15, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(25, false);
    auto delta = tree->create_node(30, false);
    auto eta = tree->create_node(35, false);
    auto theta = tree->create_node(40, false);
    auto iota = tree->create_node(45, false);

    tree->add(alpha);
    tree->add(beta);
    tree->add(gamma);
    tree->add(delta);
    tree->add(eta);
    tree->add(theta);
    tree->add(iota);

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(delta->left == beta);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(theta->left == eta);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(!iota->is_red);
}

BOOST_AUTO_TEST_CASE(add_sequence_decreasing)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(15, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(25, false);
    auto delta = tree->create_node(30, false);
    auto eta = tree->create_node(35, false);
    auto theta = tree->create_node(40, false);
    auto iota = tree->create_node(45, false);

    tree->add(iota);
    tree->add(theta);
    tree->add(eta);
    tree->add(delta);
    tree->add(gamma);
    tree->add(beta);
    tree->add(alpha);

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(delta->left == beta);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(theta->left == eta);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(!iota->is_red);
}

BOOST_AUTO_TEST_CASE(add_sequence_alternating)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(15, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(25, false);
    auto delta = tree->create_node(30, false);
    auto eta = tree->create_node(35, false);
    auto theta = tree->create_node(40, false);
    auto iota = tree->create_node(45, false);

    tree->add(iota);
    tree->add(alpha);
    tree->add(theta);
    tree->add(beta);
    tree->add(eta);
    tree->add(gamma);
    tree->add(delta);

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(delta->left == beta);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(theta->left == eta);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(!iota->is_red);
}

//
// remove tests
//
BOOST_AUTO_TEST_CASE(remove_exclusive_node)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto node = tree->create_node(15, false);

    BOOST_REQUIRE(node->is_red == true);

    tree->add(node);

    BOOST_REQUIRE(tree->root() == node);
    BOOST_REQUIRE(tree->nil() != node);
    BOOST_REQUIRE(node->is_red == false);
    BOOST_REQUIRE(node->key == 15);
    BOOST_REQUIRE(node->value == false);

    tree->remove(node);

    BOOST_REQUIRE(tree->root() == tree->nil());
    BOOST_REQUIRE(tree->root() != node);

    tree->destroy_node(node);
    node = nullptr;
}

BOOST_AUTO_TEST_CASE(remove_root_from_three_node_tree)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(15, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(25, false);

    tree->add(alpha);
    tree->add(beta);
    tree->add(gamma);

    BOOST_REQUIRE(tree->root() == beta);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(alpha->is_red == false);
    BOOST_REQUIRE(beta->is_red == true);
    BOOST_REQUIRE(gamma->is_red == true);

    tree->remove(beta);

    BOOST_REQUIRE(tree->root() == alpha);
    BOOST_REQUIRE(alpha->left == tree->nil());
    BOOST_REQUIRE(alpha->right == gamma);

    tree->destroy_node(beta);
    beta = nullptr;
}

//
// retrieve tests
//
BOOST_AUTO_TEST_CASE(retrieve_from_empty_tree)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto result = tree->retrieve(15);

    BOOST_REQUIRE(tree->nil() == result);
}

BOOST_AUTO_TEST_CASE(retrieve_from_nonempty_tree)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(15, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(25, false);

    tree->add(alpha);
    tree->add(beta);
    tree->add(gamma);

    BOOST_REQUIRE(tree->root() == beta);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(alpha->is_red == false);
    BOOST_REQUIRE(beta->is_red == true);
    BOOST_REQUIRE(gamma->is_red == true);

    BOOST_REQUIRE(tree->retrieve(10) == tree->nil());
    BOOST_REQUIRE(tree->retrieve(15) == alpha);
    BOOST_REQUIRE(tree->retrieve(20) == beta);
    BOOST_REQUIRE(tree->retrieve(25) == gamma);
}

BOOST_AUTO_TEST_SUITE_END()

