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

static void print_tree(std::shared_ptr<red_black_tree<int, bool>> tree)
{
    std::cout << "----- begin tree -----" << std::endl;

    std::stack<std::tuple<const red_black_node<int, bool>*, int, bool>> queue;

    if (tree->root() != tree->nil())
        queue.push(std::make_tuple<const red_black_node<int, bool>*, int, bool>(tree->root(), 0, false));

    while (!queue.empty())
    {
        auto current = queue.top();
        queue.pop();

        auto node = std::get<0>(current);
        auto depth = std::get<1>(current);
        bool is_left_child = std::get<2>(current);

        if (node->right != tree->nil())
            queue.push(std::make_tuple<const red_black_node<int, bool>*, int, bool>(node->right, depth + 1, false));

        if (node->left != tree->nil())
            queue.push(std::make_tuple<const red_black_node<int, bool>*, int, bool>(node->left, depth + 1, true));

        if (depth > 0)
        {
            for (int i = 0; i < depth; i++)
                std::cout << ">>>>";

            if (is_left_child) { std::cout << "l" << depth << ": "; }
            else { std::cout << "r" << depth << ": "; }

            std::cout << node->key << ", color: " << node->is_red << std::endl;
        }
        else
        {
            std::cout << "root: " << node->key << ", color: " << node->is_red << std::endl;
        }
    }

    std::cout << "----- end tree -----" << std::endl;
}

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

    BOOST_REQUIRE(tree->root() == beta);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(theta->left == eta);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(eta->is_red);
    BOOST_REQUIRE(iota->is_red);
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

    BOOST_REQUIRE(tree->root() == theta);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(theta->left == delta);
    BOOST_REQUIRE(delta->right == eta);
    BOOST_REQUIRE(delta->left == beta);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(beta->left == alpha);

    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!iota->is_red);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(alpha->is_red);
    BOOST_REQUIRE(gamma->is_red);
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

    BOOST_REQUIRE(tree->root() == theta);
    BOOST_REQUIRE(theta->left == beta);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(delta->right == eta);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(!iota->is_red);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(eta->is_red);
    BOOST_REQUIRE(gamma->is_red);
}

BOOST_AUTO_TEST_CASE(add_textbook_example)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(1, false);
    auto beta = tree->create_node(2, false);
    auto delta = tree->create_node(5, false);
    auto eta = tree->create_node(7, false);
    auto theta = tree->create_node(8, false);
    auto iota = tree->create_node(11, false);
    auto kappa = tree->create_node(14, false);
    auto lambda = tree->create_node(15, false);

    tree->add(iota);
    tree->add(alpha);
    tree->add(kappa);
    tree->add(beta);
    tree->add(delta);
    tree->add(lambda);
    tree->add(eta);
    tree->add(theta);

    BOOST_REQUIRE(tree->root() == iota);
    BOOST_REQUIRE(!iota->is_red);
    BOOST_REQUIRE(iota->left == beta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == eta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == delta);
    BOOST_REQUIRE(eta->right == theta);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);

    auto gamma= tree->create_node(4, false);
    tree->add(gamma);

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == iota);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(iota->is_red);
    BOOST_REQUIRE(iota->left == theta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);
}

BOOST_AUTO_TEST_CASE(add_textbook_example_mirror)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(1, false);
    auto beta = tree->create_node(2, false);
    auto gamma= tree->create_node(4, false);
    auto delta = tree->create_node(5, false);
    auto eta = tree->create_node(7, false);
    auto theta = tree->create_node(8, false);
    auto iota = tree->create_node(11, false);
    auto kappa = tree->create_node(14, false);
    auto lambda = tree->create_node(15, false);

    tree->add(gamma);
    tree->add(lambda);
    tree->add(beta);
    tree->add(kappa);
    tree->add(theta);
    tree->add(alpha);
    tree->add(eta);
    tree->add(delta);

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == kappa);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(alpha->is_red);
    BOOST_REQUIRE(kappa->is_red);
    BOOST_REQUIRE(kappa->left == eta);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == delta);
    BOOST_REQUIRE(eta->right == theta);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!lambda->is_red);

    tree->add(iota);

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == gamma);
    BOOST_REQUIRE(eta->right == kappa);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(kappa->is_red);
    BOOST_REQUIRE(kappa->left == theta);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(theta->right == iota);
    BOOST_REQUIRE(iota->is_red);
    BOOST_REQUIRE(!lambda->is_red);
}

//
// remove tests
//
BOOST_AUTO_TEST_CASE(remove_node_missing_left_child)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(1, false);
    auto beta = tree->create_node(2, false);
    auto delta = tree->create_node(5, false);
    auto eta = tree->create_node(7, false);
    auto theta = tree->create_node(8, false);
    auto iota = tree->create_node(11, false);
    auto kappa = tree->create_node(14, false);
    auto lambda = tree->create_node(15, false);

    tree->add(iota);
    tree->add(alpha);
    tree->add(kappa);
    tree->add(beta);
    tree->add(delta);
    tree->add(lambda);
    tree->add(eta);
    tree->add(theta);

    BOOST_REQUIRE(tree->root() == iota);
    BOOST_REQUIRE(!iota->is_red);
    BOOST_REQUIRE(iota->left == beta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == eta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == delta);
    BOOST_REQUIRE(eta->right == theta);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);

    tree->remove(kappa);
    tree->destroy_node(kappa);
    kappa = nullptr;

    BOOST_REQUIRE(tree->root() == iota);
    BOOST_REQUIRE(!iota->is_red);
    BOOST_REQUIRE(iota->left == beta);
    BOOST_REQUIRE(iota->right == lambda);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == eta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == delta);
    BOOST_REQUIRE(eta->right == theta);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!lambda->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_missing_right_child)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(1, false);
    auto beta = tree->create_node(2, false);
    auto gamma = tree->create_node(4, false);
    auto delta = tree->create_node(5, false);
    auto eta = tree->create_node(7, false);
    auto theta = tree->create_node(8, false);
    auto iota = tree->create_node(11, false);
    auto kappa = tree->create_node(14, false);
    auto lambda = tree->create_node(15, false);

    tree->add(iota);
    tree->add(alpha);
    tree->add(kappa);
    tree->add(beta);
    tree->add(delta);
    tree->add(lambda);
    tree->add(eta);
    tree->add(theta);
    tree->add(gamma);

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == iota);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(iota->is_red);
    BOOST_REQUIRE(iota->left == theta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);

    tree->remove(delta);
    tree->destroy_node(delta);
    delta = nullptr;

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == iota);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == gamma);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(iota->is_red);
    BOOST_REQUIRE(iota->left == theta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_has_both_children_subtree_minimum_immediate_child)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(1, false);
    auto beta = tree->create_node(2, false);
    auto gamma = tree->create_node(4, false);
    auto delta = tree->create_node(5, false);
    auto eta = tree->create_node(7, false);
    auto theta = tree->create_node(8, false);
    auto iota = tree->create_node(11, false);
    auto kappa = tree->create_node(14, false);
    auto lambda = tree->create_node(15, false);

    tree->add(iota);
    tree->add(alpha);
    tree->add(kappa);
    tree->add(beta);
    tree->add(delta);
    tree->add(lambda);
    tree->add(eta);
    tree->add(theta);
    tree->add(gamma);

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == iota);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(iota->is_red);
    BOOST_REQUIRE(iota->left == theta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);

    tree->remove(iota);
    tree->destroy_node(iota);
    iota = nullptr;

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == kappa);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(kappa->is_red);
    BOOST_REQUIRE(kappa->left == theta);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!lambda->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_has_both_children_subtree_minimum_grandchild)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(1, false);
    auto beta = tree->create_node(2, false);
    auto gamma = tree->create_node(4, false);
    auto delta = tree->create_node(5, false);
    auto eta = tree->create_node(7, false);
    auto theta = tree->create_node(8, false);
    auto iota = tree->create_node(11, false);
    auto kappa = tree->create_node(14, false);
    auto lambda = tree->create_node(15, false);

    tree->add(iota);
    tree->add(alpha);
    tree->add(kappa);
    tree->add(beta);
    tree->add(delta);
    tree->add(lambda);
    tree->add(eta);
    tree->add(theta);
    tree->add(gamma);

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == iota);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(iota->is_red);
    BOOST_REQUIRE(iota->left == theta);
    BOOST_REQUIRE(iota->right == kappa);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(!kappa->is_red);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(lambda->is_red);

    tree->remove(eta);
    tree->destroy_node(eta);
    eta = nullptr;

    BOOST_REQUIRE(tree->root() == theta);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(theta->left == beta);
    BOOST_REQUIRE(theta->right == kappa);
    BOOST_REQUIRE(beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(beta->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(gamma->is_red);
    BOOST_REQUIRE(kappa->is_red);
    BOOST_REQUIRE(kappa->left == iota);
    BOOST_REQUIRE(kappa->right == lambda);
    BOOST_REQUIRE(!iota->is_red);
    BOOST_REQUIRE(!lambda->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_u_or_v_red)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(10, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(30, false);
    auto delta = tree->create_node(40, false);

    tree->add(gamma);
    tree->add(delta);
    tree->add(beta);
    tree->add(alpha);

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(beta->left == alpha);
    BOOST_REQUIRE(alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);

    tree->remove(beta);
    tree->destroy_node(beta);
    beta = nullptr;

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == alpha);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!alpha->is_red);
    BOOST_REQUIRE(!delta->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_case_left_right)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(10, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(30, false);
    auto delta = tree->create_node(40, false);
    auto eta = tree->create_node(25, false);

    tree->add(gamma);
    tree->add(delta);
    tree->add(beta);
    tree->add(alpha);

    tree->remove(alpha);
    tree->destroy_node(alpha);
    alpha = nullptr;

    tree->add(eta);

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(beta->right == eta);
    BOOST_REQUIRE(eta->is_red);
    BOOST_REQUIRE(!delta->is_red);

    tree->remove(delta);
    tree->destroy_node(delta);
    delta = nullptr;

    BOOST_REQUIRE(tree->root() == eta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(eta->left == beta);
    BOOST_REQUIRE(eta->right == gamma);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(!gamma->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_case_right_left)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(10, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(30, false);
    auto delta = tree->create_node(40, false);
    auto theta = tree->create_node(35, false);

    tree->add(gamma);
    tree->add(delta);
    tree->add(beta);
    tree->add(alpha);

    tree->remove(alpha);
    tree->destroy_node(alpha);
    alpha = nullptr;

    tree->add(theta);

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == theta);
    BOOST_REQUIRE(theta->is_red);

    tree->remove(beta);
    tree->destroy_node(beta);
    beta = nullptr;

    BOOST_REQUIRE(tree->root() == theta);
    BOOST_REQUIRE(!theta->is_red);
    BOOST_REQUIRE(theta->left == gamma);
    BOOST_REQUIRE(theta->right == delta);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(!delta->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_case_left_left)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(50, false);
    auto beta = tree->create_node(40, false);
    auto gamma = tree->create_node(35, false);
    auto theta = tree->create_node(30, false);
    auto delta = tree->create_node(20, false);
    auto eta = tree->create_node(10, false);

    tree->add(gamma);
    tree->add(delta);
    tree->add(beta);
    tree->add(alpha);

    tree->remove(alpha);
    tree->destroy_node(alpha);
    alpha = nullptr;

    tree->add(eta);
    tree->add(theta);

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->right == beta);
    BOOST_REQUIRE(gamma->left == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(delta->left == eta);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(eta->is_red);

    tree->remove(beta);
    tree->destroy_node(beta);
    beta = nullptr;

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->right == gamma);
    BOOST_REQUIRE(delta->left == eta);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == theta);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!eta->is_red);
}

BOOST_AUTO_TEST_CASE(remove_node_case_right_right)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(10, false);
    auto beta = tree->create_node(20, false);
    auto gamma = tree->create_node(30, false);
    auto delta = tree->create_node(40, false);
    auto eta = tree->create_node(50, false);
    auto theta = tree->create_node(35, false);

    tree->add(gamma);
    tree->add(delta);
    tree->add(beta);
    tree->add(alpha);

    tree->remove(alpha);
    tree->destroy_node(alpha);
    alpha = nullptr;

    tree->add(eta);
    tree->add(theta);

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == theta);
    BOOST_REQUIRE(delta->right == eta);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(eta->is_red);

    tree->remove(beta);
    tree->destroy_node(beta);
    beta = nullptr;

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(delta->right == eta);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->right == theta);
    BOOST_REQUIRE(theta->is_red);
    BOOST_REQUIRE(!eta->is_red);
}

BOOST_AUTO_TEST_CASE(remove_sibling_red_case_left)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(40, false);
    auto beta = tree->create_node(35, false);
    auto gamma = tree->create_node(30, false);
    auto eta = tree->create_node(25, false);
    auto delta = tree->create_node(20, false);
    auto theta = tree->create_node(10, false);
    auto iota = tree->create_node(5, false);

    tree->add(gamma);
    tree->add(beta);
    tree->add(delta);
    tree->add(alpha);
    tree->add(eta);
    tree->add(theta);
    tree->add(iota);

    tree->remove(alpha);
    tree->destroy_node(alpha);
    alpha = nullptr;

    tree->remove(iota);
    tree->destroy_node(iota);
    iota = nullptr;

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->right == beta);
    BOOST_REQUIRE(gamma->left == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(delta->right == eta);
    BOOST_REQUIRE(delta->left == theta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(!theta->is_red);

    tree->remove(beta);
    tree->destroy_node(beta);
    beta = nullptr;

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->right == gamma);
    BOOST_REQUIRE(delta->left == theta);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == eta);
    BOOST_REQUIRE(eta->is_red);
    BOOST_REQUIRE(!theta->is_red);
}

BOOST_AUTO_TEST_CASE(remove_sibling_red_case_right)
{
    std::allocator<red_black_node<int, bool>> allocator;
    auto tree = std::make_shared<red_black_tree<int, bool>>(allocator);

    auto alpha = tree->create_node(5, false);
    auto beta = tree->create_node(10, false);
    auto gamma = tree->create_node(20, false);
    auto delta = tree->create_node(30, false);
    auto eta = tree->create_node(25, false);
    auto theta = tree->create_node(35, false);
    auto iota = tree->create_node(40, false);

    tree->add(gamma);
    tree->add(beta);
    tree->add(delta);
    tree->add(alpha);
    tree->add(eta);
    tree->add(theta);
    tree->add(iota);

    tree->remove(alpha);
    tree->destroy_node(alpha);
    alpha = nullptr;

    tree->remove(iota);
    tree->destroy_node(iota);
    iota = nullptr;

    BOOST_REQUIRE(tree->root() == gamma);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->left == beta);
    BOOST_REQUIRE(gamma->right == delta);
    BOOST_REQUIRE(!beta->is_red);
    BOOST_REQUIRE(delta->is_red);
    BOOST_REQUIRE(delta->left == eta);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(!eta->is_red);
    BOOST_REQUIRE(!theta->is_red);

    tree->remove(beta);
    tree->destroy_node(beta);
    beta = nullptr;

    BOOST_REQUIRE(tree->root() == delta);
    BOOST_REQUIRE(!delta->is_red);
    BOOST_REQUIRE(delta->left == gamma);
    BOOST_REQUIRE(delta->right == theta);
    BOOST_REQUIRE(!gamma->is_red);
    BOOST_REQUIRE(gamma->right == eta);
    BOOST_REQUIRE(eta->is_red);
    BOOST_REQUIRE(!theta->is_red);
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
    BOOST_REQUIRE(alpha->is_red == true);
    BOOST_REQUIRE(beta->is_red == false);
    BOOST_REQUIRE(gamma->is_red == true);

    BOOST_REQUIRE(tree->retrieve(10) == tree->nil());
    BOOST_REQUIRE(tree->retrieve(15) == alpha);
    BOOST_REQUIRE(tree->retrieve(20) == beta);
    BOOST_REQUIRE(tree->retrieve(25) == gamma);
}

BOOST_AUTO_TEST_SUITE_END()

