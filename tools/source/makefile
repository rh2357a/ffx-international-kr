ifeq ($(DEBUG), 1)
BUILD_TARGET := debug
else
BUILD_TARGET := release
endif

SOURCE_DIR       := src
BUILD_DIR        := build
BUILD_TARGET_DIR := $(BUILD_DIR)/$(BUILD_TARGET)
BUILD_OBJ_DIR    := $(BUILD_TARGET_DIR)/obj
BUILD_OUTPUT_DIR := $(BUILD_TARGET_DIR)/bin

################################################################################

CXX := x86_64-w64-mingw32-g++

CXXFLAGS := -std=c++20 -fpermissive \
            -Wall -Wextra \
            -Wno-comment \
            -Wno-unused-parameter \
            -Wno-unused-variable \
            -Wno-unused-function

CXXDEFINES  := -DUNICODE -D_UNICODE

LDFLAGS := -static -static-libgcc -static-libstdc++ -lmsvcrt

ifeq ($(DEBUG), 1)
CXXFLAGS   += -g -O0
CXXDEFINES += -DDEBUG
else
CXXFLAGS   += -O3 -flto -ffunction-sections -fdata-sections
CXXDEFINES += -DRELEASE
LDFLAGS    += -s -Wl,--gc-sections
endif

################################################################################

ffxftcx_obj := $(BUILD_OBJ_DIR)/ffxftcx.o
ffxname_obj := $(BUILD_OBJ_DIR)/ffxname.o

ffxiso_obj := \
	$(BUILD_OBJ_DIR)/ffxiso.o \
	$(BUILD_OBJ_DIR)/ffxiso/extract.o \
	$(BUILD_OBJ_DIR)/ffxiso/import.o

################################################################################

.PHONY: all clean

all: ffxiso ffxftcx ffxname

ffxiso: $(ffxiso_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)

ffxftcx: $(ffxftcx_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)

ffxname: $(ffxname_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)

$(BUILD_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(CXXDEFINES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.exe
