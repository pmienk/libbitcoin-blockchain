#
# documentation
#
doc_DATA = README INSTALL COPYING

#
# pkg-config
#
pkgconfig_DATA = libbitcoin-blockchain.pc

# look for macros in the m4 subdirectory
ACLOCAL_AMFLAGS = -I m4

#
# headers
#
bitcoin_includedir = $(includedir)/bitcoin
bitcoin_include_HEADERS = \
    include/bitcoin/blockchain.hpp

bitcoin_blockchain_includedir = $(includedir)/bitcoin/blockchain
bitcoin_blockchain_include_HEADERS = \
    include/bitcoin/blockchain/define.hpp 

#
# libraries
#
lib_LTLIBRARIES = libbitcoin-blockchain.la

libbitcoin_blockchain_la_SOURCES = 

libbitcoin_blockchain_la_CPPFLAGS = \
    -I$(srcdir)/include \
    ${bitcoin_CFLAGS}

#
# tests
#
#if WITH_TESTS
#
#TESTS = test/libbitcoin_blockchain_test
#
#check_PROGRAMS = test/libbitcoin_blockchain_test
#
#test_libbitcoin_blockchain_test_CPPFLAGS = \
#    -I$(srcdir)/include \
#    ${bitcoin_CFLAGS}
#
#test_libbitcoin_blockchain_test_SOURCES = \
#    test/databases.cpp \
#    test/disk_objs.cpp \
#    test/htdb.cpp \
#    test/interface.cpp \
#    test/main.cpp
#
#test_libbitcoin_blockchain_test_LDADD = \
#    libbitcoin-blockchain.la \
#    -lleveldb \
#    -lboost_unit_test_framework \
#    ${bitcoin_LIBS}
#
#endif
