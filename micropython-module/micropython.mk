
# Reqire `CFLAGS_EXTRA=-DMODULE_UICAL_ENABLED=1` on make invocation
# MODULE_UICAL_ENABLED is further used by MP_REGISTER_MODULE
ifneq (,$(findstring -DMODULE_UICAL_ENABLED=1,$(CFLAGS_EXTRA)))

$(info # Module Enabled: uICAL)

UICAL_MOD_DIR := $(USERMOD_DIR)

SRC_USERMOD_CXX += $(wildcard $(UICAL_MOD_DIR)/src/*.cpp)
SRC_USERMOD_CXX += $(wildcard $(UICAL_MOD_DIR)/micropython-module/*.cpp)

SRC_USERMOD += $(wildcard $(UICAL_MOD_DIR)/micropython-module/*.c)

CXXFLAGS_USERMOD += -I$(UICAL_MOD_DIR)/src

endif