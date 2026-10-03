CXX      := g++
CXXFLAGS := -O2 -Wall -Wextra -std=c++17
CPPFLAGS := -Iheaders -Itests

CORE_SRC := src/image.cpp src/pnm_io.cpp
TEST_SRC := tests/test_main.cpp tests/test_image.cpp tests/test_pnm_io.cpp

run_tests: $(CORE_SRC) $(TEST_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

test: run_tests
	mkdir -p output
	./run_tests

clean:
	rm -f run_tests

.PHONY: test clean
