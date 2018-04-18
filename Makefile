TARGET_EXEC ?= boot1.elf

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

include ./Makefile.config

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

IP:
	./neoIP-tool/maketmpl bin/IP.BIN build/boot1 bin/IP.BIN.HAK ;echo ;

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ -Xlinker -Map=MAP_bin1.map $(DREAM_CFLAGS) $(DREAM_LDFLAGS)
	sh-elf-objcopy -R .stack -O binary $@ $(basename $@)

	
# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) -little -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) -fomit-frame-pointer $(DREAM_CFLAGS) $(DREAM_LDFLAGS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
#$(BUILD_DIR)/%.cpp.o: %.cpp
#	$(MKDIR_P) $(dir $@)
#	$(CXX) -fomit-frame-pointer $(DREAM_CFLAGS) $(DREAM_LDFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p