TARGET_EXEC ?= main.out

BUILD_DIR ?= ./build
DAC_DIR_REL ?= ../../../src

DAC_DIR ?= $(shell realpath $(DAC_DIR_REL))# otherwise the *.o files end up anywhere
SRC_DIRS ?= ./ $(DAC_DIR)

FILES := Helpers.c Helpers.h error_functions.c error_functions.h\
	get_num.c get_num.h tlpi_hdr.h ename.c.inc
	
FILES_OBJ := $(FILES:%=$(BUILD_DIR)/%.file)

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name "*.cpp" -or -name "*.c" -or -name "*.s")

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

OBJS += $(FILES_OBJ)

OBJS_DEP := ename.c.inc
OBJS_DEP += $(OBJS)


INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += ../

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -Wall -Wextra -Wdouble-promotion -Werror -MMD -MP

LDFLAGS += -L$(BUILD_DIR)$(DAC_DIR)
LDLIBS := -lstdc++ -pthread

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS_DEP)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# files
$(BUILD_DIR)/%.file: $(BUILD_DIR)/%.copy
	ld -r -b binary $< -o $@
	
$(BUILD_DIR)/%.copy: $(DAC_DIR_REL)/embeddedFiles/%
	cp -f $< $@	

# ename
ename.c.inc:
	sh $(DAC_DIR_REL)/Build_ename.sh > $(DAC_DIR_REL)/ename.c.inc
	echo 1>&2 "ename.c.inc built"

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
