CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++11 -g #-DUICAL_LOG_LEVEL=5
CXX_GCOV := -fprofile-arcs -ftest-coverage

BIN		:= build/bin
SRC		:= src
INCLUDE	:= src
TEST_CPP:= test/cpp

.PHONY: test-cpp test-python test-all coverage memory clean

SRC_FILES      := $(wildcard $(SRC)/*.cpp)
HEADER_FILES   := $(wildcard $(INCLUDE)/uical/*.h)
TEST_FILES     := $(wildcard $(TEST_CPP)/*.cpp)

test-cpp: $(BIN)/test
	./$(BIN)/test


test-python:
	pip install --break-system-packages -e .
	pytest ./test/python/


test-all: test-cpp test-python


$(BIN)/test: $(SRC_FILES) $(HEADER_FILES) $(TEST_FILES)
	mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE)/ $(SRC_FILES) $(TEST_FILES) -o $@


$(BIN)/cov: $(SRC_FILES) $(HEADER_FILES) $(TEST_FILES)
	mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) $(CXX_GCOV) -I$(INCLUDE)/ $(SRC_FILES) $(TEST_FILES) -o $@


coverage: $(BIN)/cov
	./$(BIN)/cov
	gcov -dump *.gcno *.gcda >/dev/null 2>/dev/null
	rm -f *.gcno *.gcda
	mkdir -p build/coverage/
	rm -f build/coverage/*
	mv *.gcov build/coverage/
	ls build/coverage/*.gcov | grep -v -e ".cpp" -e ".h" | xargs rm
	grep "####" build/coverage/src*.cpp.gcov | grep -v "\-:" | awk '{print $$1}' | sed -e 's|build/coverage/src#||' -e 's|\.gcov:||' | uniq -c 


memory: $(BIN)/test
	valgrind --leak-check=full -s ./$(BIN)/test


clean:
	rm -rf ./$(BIN)/*
	rm -rf ./build/*
