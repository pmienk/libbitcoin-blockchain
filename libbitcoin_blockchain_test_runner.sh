#!/bin/sh
###############################################################################
#  Copyright (c) 2014-2015 libbitcoin-blockchain developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Define tests and options.
#==============================================================================
BOOST_UNIT_TEST_OPTIONS=\
"--run_test=databases,disk_objs,htdb,block_index_store_tests,modified_patricia_trie_tests,payment_address_index_store_tests,red_black_store_tests,red_black_tree_tests,stealth_transaction_index_store_tests,transaction_index_store_tests "\
"--show_progress=no "\
"--detect_memory_leak=0 "\
"--report_level=no "\
"--build_info=yes"


# Run tests.
#==============================================================================
./test/libbitcoin_blockchain_test ${BOOST_UNIT_TEST_OPTIONS}
