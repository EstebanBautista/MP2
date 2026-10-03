CXX      := g++
CXXFLAGS := -O2 -Wall -Wextra -std=c++17
CPPFLAGS := -Iheaders -Itests

CORE_SRC := src/image.cpp src/pnm_io.cpp src/timer.cpp
FILTER_SRC := src/filters.cpp src/convolver.cpp src/cli.cpp src/pipeline.cpp
PAR_SRC := src/threaded_convolver.cpp
TEST_SRC := tests/test_main.cpp tests/test_image.cpp tests/test_pnm_io.cpp tests/test_timer.cpp \
            tests/test_filters.cpp tests/test_convolver.cpp tests/test_cli.cpp \
            tests/test_parallel.cpp

BINS := processor filterer th_filterer

all: $(BINS)

processor: $(CORE_SRC) src/processor.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

filterer: $(CORE_SRC) $(FILTER_SRC) src/filterer.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

th_filterer: $(CORE_SRC) $(FILTER_SRC) src/threaded_convolver.cpp src/th_filterer.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -pthread $^ -o $@

run_tests: $(CORE_SRC) $(FILTER_SRC) $(PAR_SRC) $(TEST_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -pthread $^ -o $@

test: run_tests
	mkdir -p output
	./run_tests

clean:
	rm -f $(BINS) run_tests

.PHONY: all test clean
