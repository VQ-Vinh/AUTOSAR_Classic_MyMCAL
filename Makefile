TOOLCHAIN_FILE := arm-none-eabi-gcc.cmake
BUILD_DIR      := build
BINARY         := mcal.elf
BIN            := mcal.bin
CMAKE_FLAGS    := -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE)

.PHONY: all f1 flash clean

# ---------- Build (auto-configure if needed) ----------
all: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR)

# ---------- Configure (only if build/ missing) ----------
$(BUILD_DIR)/Makefile:
	cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)

# ---------- Clean + Build ----------
f1: clean
	cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)
	cmake --build $(BUILD_DIR)

# ---------- Flash ----------
flash: all
	arm-none-eabi-objcopy -O binary $(BUILD_DIR)/$(BINARY) $(BUILD_DIR)/$(BIN)
	st-flash write $(BUILD_DIR)/$(BIN) 0x08000000

# ---------- Clean ----------
clean:
	rm -rf $(BUILD_DIR)
