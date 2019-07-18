TARGET_EXEC ?= main.out

BUILD_DIR ?= ./build
DAC_DIR_REL ?= ../../src

DAC_DIR ?= $(shell realpath $(DAC_DIR_REL))# otherwise the *.o files end up anywhere
SRC_DIRS ?= ./ $(DAC_DIR)

FILES := Helpers.c Helpers.h
FILES_OBJ := $(FILES:%=$(BUILD_DIR)$(DAC_DIR)/%.file)

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -name "*.cpp" -or -name "*.c" -or -name "*.s")

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

OBJS += $(FILES_OBJ)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

LDLIBS := -lstdc++ -pthread

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
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
$(BUILD_DIR)$(DAC_DIR)/%.file: $(DAC_DIR)/%
	ld -r -b binary $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
