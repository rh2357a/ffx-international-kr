ifeq ($(DEBUG), 1)
BUILD_TARGET := debug
UPX          := true
else
BUILD_TARGET := release

ifeq (, $(shell which upx))
	UPX := true
else
	UPX := upx
endif

endif

SOURCE_DIR       := src
BUILD_DIR        := build
BUILD_TARGET_DIR := $(BUILD_DIR)/$(BUILD_TARGET)
BUILD_OBJ_DIR    := $(BUILD_TARGET_DIR)/obj
BUILD_OUTPUT_DIR := $(BUILD_TARGET_DIR)/bin

################################################################################

XDELTA3LDFLAGS := -L$(SOURCE_DIR)/utils/xdelta3 -lxdelta3 -llzma

WXCXXFLAGS  := --cxxflags --unicode --static
WXLDFLAGS   := --libs --unicode --static

ifeq ($(DEBUG), 1)
WXCXXFLAGS += --debug
else
WXCXXFLAGS +=
endif

CFLAGS  := -std=c17 \
           -Isrc \
           -Wall -Wextra \
           -Wno-unused-parameter \
           -Wno-unused-variable \
           -Wno-unused-function

CXXFLAGS := -std=c++20 -fpermissive \
            -Isrc \
            -Wall -Wextra \
            -Wno-comment \
            -Wno-unused-parameter \
            -Wno-unused-variable \
            -Wno-unused-function \
            $(shell wx-config $(WXCXXFLAGS))

DEFINES := -DUNICODE -D_UNICODE

LDFLAGS := -static -static-libgcc -static-libstdc++ -lmsvcrt

ifeq ($(DEBUG), 1)
DEFINES    += -DDEBUG
CFLAGS     += -g -O0
CXXFLAGS   += -g -O0
else
DEFINES    += -DRELEASE
CFLAGS     += -O3 -ffunction-sections -fdata-sections
CXXFLAGS   += -O3 -flto -ffunction-sections -fdata-sections
LDFLAGS    += -s -Wl,--gc-sections
endif

################################################################################

font_adjustment_obj := $(BUILD_OBJ_DIR)/font_adjustment.o
ffxftcx_obj         := $(BUILD_OBJ_DIR)/ffxftcx.o
ffxname_obj         := $(BUILD_OBJ_DIR)/ffxname.o

ffxiso_obj := \
	$(BUILD_OBJ_DIR)/ffxiso.o \
	$(BUILD_OBJ_DIR)/ffxiso/extract.o \
	$(BUILD_OBJ_DIR)/ffxiso/import.o

ffxppkg_obj := \
	$(BUILD_OBJ_DIR)/ffxppkg.o \
	$(BUILD_OBJ_DIR)/utils/xdelta3/xdelta3_wrapper.o

ffx_international_kr_obj := \
	$(BUILD_OBJ_DIR)/ffx_international_kr.o \
	$(BUILD_OBJ_DIR)/ffx_international_kr.rc.o \
	$(BUILD_OBJ_DIR)/ffx_international_kr/gui_base.o \
	$(BUILD_OBJ_DIR)/utils/xdelta3/xdelta3_wrapper.o

################################################################################

.PHONY: all clean

all: ffxiso ffxftcx ffxname ffxppkg font_adjustment ffx_international_kr

ffxiso: $(ffxiso_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)
	$(UPX) $(BUILD_OUTPUT_DIR)/$@.exe

ffxftcx: $(ffxftcx_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)
	$(UPX) $(BUILD_OUTPUT_DIR)/$@.exe

ffxname: $(ffxname_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)
	$(UPX) $(BUILD_OUTPUT_DIR)/$@.exe

ffxppkg: $(ffxppkg_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS) $(XDELTA3LDFLAGS)
	$(UPX) $(BUILD_OUTPUT_DIR)/$@.exe

font_adjustment: $(font_adjustment_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS)
	$(UPX) $(BUILD_OUTPUT_DIR)/$@.exe

ffx_international_kr: $(ffx_international_kr_obj)
	@mkdir -p $(BUILD_OUTPUT_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) -o $(BUILD_OUTPUT_DIR)/$@ $^ $(LDFLAGS) $(XDELTA3LDFLAGS) $(shell wx-config $(WXLDFLAGS)) -mwindows
	$(UPX) $(BUILD_OUTPUT_DIR)/$@.exe

$(BUILD_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $< -o $@

$(BUILD_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

$(BUILD_OBJ_DIR)/%.rc.o: $(SOURCE_DIR)/%.rc
	@mkdir -p $(shell dirname $@)
	windres $(DEFINES) $(CXXINCLUDES) $< $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.exe
