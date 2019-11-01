CXX		  := clang++
CXX_FLAGS := -Wall -Wextra -std=c++11 -g

BIN		:= build/bin
SRC		:= src
INCLUDE	:= src


test-python:
	python3 ./setup.py build install
	pytest ./test/test_*.py



test-cpp: $(BIN)/test
	./$(BIN)/test

$(BIN)/test: $(SRC)/*.cpp $(INCLUDE)/uical/*.h
	mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE)/ $(SRC)/*.cpp  test/*.cpp -o $@

clean:
	rm -rf $(BIN)/*
