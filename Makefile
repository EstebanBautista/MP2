CXX      := g++
CXXFLAGS := -O2 -Wall -Wextra -std=c++17
CPPFLAGS := -Iheaders -Itests

CORE_SRC := src/image.cpp src/pnm_io.cpp src/timer.cpp
FILTER_SRC := src/filters.cpp src/convolver.cpp src/cli.cpp src/pipeline.cpp
TEST_SRC := tests/test_main.cpp tests/test_image.cpp tests/test_pnm_io.cpp tests/test_timer.cpp \
            tests/test_filters.cpp tests/test_convolver.cpp tests/test_cli.cpp

BINS := processor filterer

all: $(BINS)

processor: $(CORE_SRC) src/processor.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

filterer: $(CORE_SRC) $(FILTER_SRC) src/filterer.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

run_tests: $(CORE_SRC) $(FILTER_SRC) $(TEST_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

test: run_tests
	mkdir -p output
	./run_tests

clean:
	rm -f $(BINS) run_tests

.PHONY: all test clean
