# Makefile for the C++ Task Manager CLI (Organized Structure)

# Compiler to use
CXX = g++

# Compiler flags:
# -std=c++14 : Use the C++14 standard
# -Wall      : Enable all standard compiler warnings
# -g         : Include debugging information
# -Iinclude  : Tell compiler to look for headers in the 'include' directory
CXXFLAGS = -std=c++14 -Wall -g -Iinclude

# Linker flags
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Name of the final executable (will be placed in BUILD_DIR)
TARGET = $(BUILD_DIR)/task-cli

# Find all .cpp source files in the source directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Generate corresponding object file names, placing them in the build directory
# e.g., src/main.cpp becomes build/main.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Default rule: Build the target executable
# Ensures the build directory exists before trying to build the target
all: $(BUILD_DIR) $(TARGET)

# Rule to create the build directory
# This explicitly tells make how to handle the 'build' directory target
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Rule to link the executable:
# Depends on all the object files and the existence of the build directory.
$(TARGET): $(OBJECTS) $(BUILD_DIR)
	@echo "Linking $(TARGET)..."
	# No need for mkdir here as the dependency $(BUILD_DIR) handles it
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "$(TARGET) built successfully in $(BUILD_DIR)/"

# Rule to compile a .cpp source file (from src/) into a .o object file (in build/):
# Depends on the corresponding .cpp file and potentially any header in include/
# Also depends on the build directory existing.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(wildcard $(INCLUDE_DIR)/*.h) | $(BUILD_DIR)
	@echo "Compiling $<..."
	# No need for mkdir here as the dependency $(BUILD_DIR) handles it
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files:
# Removes the entire build directory.
clean:
	@echo "Cleaning up build files..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Declare phony targets
.PHONY: all clean