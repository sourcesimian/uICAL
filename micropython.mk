UICAL_MOD_DIR := $(USERMOD_DIR)

SRC_USERMOD_CXX += $(wildcard $(UICAL_MOD_DIR)/src/*.cpp)
SRC_USERMOD += $(wildcard $(UICAL_MOD_DIR)/micropython-module/*.c)

CXXFLAGS_USERMOD += -I$(UICAL_MOD_DIR)/src
