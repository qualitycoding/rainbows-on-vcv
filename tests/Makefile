# FROZEN — DO NOT MODIFY. Verified planning test command.
CXX ?= g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc
TEST_BIN := /tmp/rainbows-frozen-tests
.PHONY: test
test:
	$(CXX) $(CXXFLAGS) tests/frozen_tests.cpp -o $(TEST_BIN)
	$(TEST_BIN)
