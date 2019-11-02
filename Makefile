CXX		  := clang++
CXX_FLAGS := -Wall -Wextra -std=c++11 -g

BIN		:= build/bin
SRC		:= src
INCLUDE	:= src

virtualenv:
	./new_virtualenv3.sh ./virtualenv
	{ \
		. ./virtualenv/bin/activate; \
		pip3 install pytest; \
	}

test-python: virtualenv
	{ \
		if ! declare -f deactivate >/dev/null; then \
			. ./virtualenv/bin/activate; \
		fi; \
		python3 ./setup.py build install && \
		pytest ./test/test_*.py; \
	}


test-cpp: $(BIN)/test
	./$(BIN)/test

$(BIN)/test: $(SRC)/*.cpp $(INCLUDE)/uical/*.h
	mkdir -p $(BIN)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE)/ $(SRC)/*.cpp  test/*.cpp -o $@

clean:
	rm -rf $(BIN)/*
