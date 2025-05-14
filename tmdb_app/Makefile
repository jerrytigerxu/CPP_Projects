# Compiler to use
CXX = g++
# Compiler flags:
# -std=c++17: Use the C++17 standard
# -Wall -Wextra -pendantic: Enable most warning messages
# -g: Generate debugging information
# Initial flags (ensure no trailing spaces here)
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Linker flags:
LDFLAGS = -lcurl 

# CPPFLAGS
# -I.: Include the current directory (for local .h files)
CPPFLAGS = -I.

# Name of the final executable
TARGET = tmdb-app

# Source files (currently just main.cpp)
# Add more .cpp files as you create them (e.g., api_handler.cpp)
SRCS = main.cpp

# Object files (derived from source files)
OBJS = $(SRCS:.cpp=.o)

# --- vcpkg Integration (WSL2/Linux) ---
# !! ADJUST VCPKG_ROOT if you cloned vcpkg elsewhere !!
# Ensure no trailing spaces on these lines
VCPKG_ROOT ?= $(HOME)/vcpkg
VCPKG_TRIPLET ?= x64-linux
VCPKG_INSTALLED_DIR = $(VCPKG_ROOT)/installed/$(VCPKG_TRIPLET)

# Add vcpkg include and library paths to compiler and linker flags
CPPFLAGS += -I$(VCPKG_ROOT)/installed/$(VCPKG_TARGET_TRIPLET)/include
LDFLAGS += -L$(VCPKG_ROOT)/installed/$(VCPKG_TARGET_TRIPLET)/lib

# --- Build Rules ---

# Default rule: 'all' builds the target executable.
# This is the rule executed if you just run 'make'.
all: $(TARGET)

# Rule to link the target executable from object files.
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Build complete: $(TARGET) is ready."

# Rule to compile .cpp source files into .o object files.
# '$<' is an automatic variable representing the first prerequisite (the .cpp file).
# '$@' is an automatic variable representing the target file (the .o file).
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# --- Clean Rule ---

# 'clean' rule: Removes generated object files and the target executable.
clean:
	@echo "Cleaning up project..."
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete."

# --- Phony Targets ---
# .PHONY declares targets that are not actual files.
# This prevents 'make' from getting confused if a file named 'all' or 'clean' exists.
.PHONY: all clean