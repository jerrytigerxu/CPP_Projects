# Compiler to use
CXX = g++
# Compiler flags:
# -std=c++17: Use the C++17 standard
# -Wall -Wextra: Enable most warning messages
# -g: Generate debugging information
# Initial flags (ensure no trailing spaces here)
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Linker flags:
LDFLAGS =

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

# Define specific include and lib paths (ensure no trailing spaces)
VCPKG_INCLUDE_DIR = $(VCPKG_INSTALLED_DIR)/include
VCPKG_LIB_DIR = $(VCPKG_INSTALLED_DIR)/lib

# Add vcpkg include directory to compiler flags
# Using := for immediate assignment with the explicit variable
CXXFLAGS := $(CXXFLAGS) -I$(VCPKG_INCLUDE_DIR)

# Add vcpkg library directory to linker flags
LDFLAGS += -L$(VCPKG_LIB_DIR)
# Link against the libraries installed via vcpkg
LDFLAGS += -lcurl # Link against libcurl
# Link against OpenSSL libraries (needed by libcurl for HTTPS)
LDFLAGS += -lssl -lcrypto
# Link against zlib (needed by libcurl for compression like gzip/deflat)
LDFLAGS += -lz
# nlohmann-json is header-only, no linking needed for it specifically
# --- End vcpkg ---

# Default rule: Build the target executable
all: $(TARGET)

# Rule to link the executable
# IMPORTANT: The next line MUST start with a TAB character, not spaces
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile source files into object files
# IMPORTANT: The next line MUST start with a TAB character, not spaces
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up generated files
# IMPORTANT: The next line MUST start with a TAB character, not spaces
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (targets that don't represent files)
.PHONY: all clean