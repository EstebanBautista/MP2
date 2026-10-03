CXX      := g++
CXXFLAGS := -O2 -Wall -Wextra -std=c++17
CPPFLAGS := -Iheaders -Itests

TEST_SRC := tests/test_main.cpp

run_tests: $(TEST_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

test: run_tests
	mkdir -p output
	./run_tests

clean:
	rm -f run_tests

.PHONY: test clean
