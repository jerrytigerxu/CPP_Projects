# Makefile for the TMDB CLI Application

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
# For debug: CXXFLAGS += -g
# For release: CXXFLAGS += -O2

# Project name
TARGET := tmdb_app

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
VCPKG_TRIPLET ?= x64-linux
VCPKG_ROOT ?= $(HOME)/vcpkg
ifeq ($(wildcard $(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake),)
    VCPKG_ROOT ?= /opt/vcpkg
    ifeq ($(wildcard $(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake),)
        VCPKG_ROOT ?= /usr/local/vcpkg
    endif
endif

# Source files and object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Include paths
VCPKG_INSTALLED_DIR := $(VCPKG_ROOT)/installed/$(VCPKG_TRIPLET)
INC_PATHS := -I$(INCLUDE_DIR) -I$(VCPKG_INSTALLED_DIR)/include

# Linker flags and libraries
LDFLAGS := -L$(VCPKG_INSTALLED_DIR)/lib
LIBS := -lcurl -lssl -lcrypto -lz -pthread # Added -lssl -lcrypto -lz for OpenSSL and zlib dependencies
# If linking libcurl statically, you might need: CXXFLAGS += -DCURL_STATICLIB
# And LIBS would need to list .a files for curl and its dependencies.

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Rule to link the executable
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)
	@echo "Executable $(TARGET) created in $(BUILD_DIR)/"

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c $< -o $@

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Phony targets
.PHONY: all clean print-vars

# Inform user about VCPKG_ROOT if it seems not found
ifeq ($(wildcard $(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake),)
$(warning VCPKG_ROOT not found or set incorrectly. Current VCPKG_ROOT=$(VCPKG_ROOT))
$(warning Please ensure VCPKG_ROOT is set to your vcpkg installation path, or adjust in Makefile.)
endif

# Debug: Print variables
print-vars:
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "TARGET: $(TARGET)"
	@echo "SRC_DIR: $(SRC_DIR)"
	@echo "INCLUDE_DIR: $(INCLUDE_DIR)"
	@echo "BUILD_DIR: $(BUILD_DIR)"
	@echo "VCPKG_TRIPLET: $(VCPKG_TRIPLET)"
	@echo "VCPKG_ROOT: $(VCPKG_ROOT)"
	@echo "VCPKG_INSTALLED_DIR: $(VCPKG_INSTALLED_DIR)"
	@echo "SRCS: $(SRCS)"
	@echo "OBJS: $(OBJS)"
	@echo "INC_PATHS: $(INC_PATHS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIBS: $(LIBS)"
