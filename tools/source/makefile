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
           -MMD -MP \
           -Wall -Wextra \
           -Wno-unused-parameter \
           -Wno-unused-variable \
           -Wno-unused-function

CXXFLAGS := -std=c++20 -fpermissive \
            -Isrc \
            -MMD -MP \
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
CXXFLAGS   += -O3 -ffunction-sections -fdata-sections
LDFLAGS    += -s -Wl,--gc-sections
endif

################################################################################

targets := ffxiso ffxftcx ffxname ffxppkg font_adjustment ffx_international_kr

# 타겟의 실제 경로 생성
$(foreach t,$(targets),$(eval $(t) := $(BUILD_OUTPUT_DIR)/$(t).exe))

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
	$(BUILD_OBJ_DIR)/utils/xdelta3/xdelta3_wrapper.o \
	$(BUILD_OBJ_DIR)/ffxiso/extract.o \
	$(BUILD_OBJ_DIR)/ffxiso/import.o \
	$(BUILD_OBJ_DIR)/ffx_international_kr/ffx_international_kr.rc.o \
	$(BUILD_OBJ_DIR)/ffx_international_kr/patch_data.o \
	$(BUILD_OBJ_DIR)/ffx_international_kr/gui_base.o \
	$(BUILD_OBJ_DIR)/ffx_international_kr/main_frame.o

all_objs := $(foreach t,$(targets),$($(t)_obj))

################################################################################

define target_alias
$1: $($(1))
endef

define link_binary
$1: $2
	@mkdir -p $$(dir $$@)
	$$(CXX) -o $$@ $$^ $$(LDFLAGS) $3
ifneq ($$(DEBUG), 1)
	upx $$@
endif
endef

.PHONY: all clean $(targets)
all: $(targets)

# alias 생성
$(foreach t,$(targets),$(eval $(call target_alias,$(t))))

# link 타겟 생성
$(eval $(call link_binary, $(ffxiso), $(ffxiso_obj)))
$(eval $(call link_binary, $(ffxftcx), $(ffxftcx_obj)))
$(eval $(call link_binary, $(ffxname), $(ffxname_obj)))
$(eval $(call link_binary, $(font_adjustment), $(font_adjustment_obj)))

$(eval $(call link_binary, \
    $(ffxppkg), $(ffxppkg_obj), \
    $(XDELTA3LDFLAGS) \
))

$(eval $(call link_binary, \
    $(ffx_international_kr), $(ffx_international_kr_obj), \
    $(XDELTA3LDFLAGS) $(shell wx-config $(WXLDFLAGS)) -mwindows \
))

$(BUILD_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $< -o $@

$(BUILD_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

$(BUILD_OBJ_DIR)/%.rc.o: $(SOURCE_DIR)/%.rc
	@mkdir -p $(dir $@)
	windres $(DEFINES) $(CXXINCLUDES) $< $@

clean:
	rm -rf $(BUILD_DIR)

-include $(all_objs:.o=.d)
