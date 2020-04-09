TARGET_EXEC ?= main.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -or -name "*.c" -or -name "*.s")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := ./dac ../
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -Wall -Wextra -Wdouble-promotion -Werror -MMD -MP -Ofast -march=native -flto
LDLIBS := -lstdc++ -lm -pthread
LDFLAGS += -flto

BCONFIG ?= NORMAL

ifeq ($(BCONFIG), PROFILE_GENERATE)
	CPPFLAGS += -fprofile-generate="../profile.gcda"
	LDLIBS += -lgcov --coverage
	LDFLAGS += -fprofile-generate="../profile.gcda"
else ifeq ($(BCONFIG), PROFILE_USE)
	CPPFLAGS += -fprofile-use="../profile.gcda"
else ifneq ($(BCONFIG), NORMAL)
$(error Unknown BCONFIG $(BCONFIG))
endif

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


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
