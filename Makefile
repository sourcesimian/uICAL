CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++11 -g #-DUICAL_LOG_LEVEL=5
CXX_GCOV := -fprofile-arcs -ftest-coverage

BIN		:= build/bin
SRC		:= src
INCLUDE	:= src

.PHONY: coverage


test-cpp: $(BIN)/test
	./$(BIN)/test


test-python:
	pip install --break-system-packages -e .
	pytest ./test/python/


test-all: test-cpp test-python


$(BIN)/test: $(SRC)/*.cpp $(INCLUDE)/uical/*.h test/cpp/*.cpp
	mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE)/ $(SRC)/*.cpp test/cpp/*.cpp -o $@


$(BIN)/cov: $(SRC)/*.cpp $(INCLUDE)/uical/*.h test/cpp/*.cpp
	mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) $(CXX_GCOV) -I$(INCLUDE)/ $(SRC)/*.cpp test/cpp/*.cpp -o $@


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
