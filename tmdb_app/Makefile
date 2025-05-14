# Makefile for the TMDB CLI Application

# --- Compiler and Flags ---
# CXX: The C++ compiler to use (defaulting to g++)
CXX = g++
# CXXFLAGS: Compiler flags
# -std=c++17: Use the C++17 standard
# -Wall: Enable all common warnings
# -Wextra: Enable some extra warnings
# -pedantic: Issue all warnings demanded by strict ISO C++
# -g: Generate debugging information
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# CPPFLAGS: C/C++ Preprocessor flags (for include paths)
# -I.: Include the current directory (for your local .h files like movie.h, api_handler.h)
CPPFLAGS = -I.

# --- Target Executable ---
TARGET = tmdb_app

# --- Source and Object Files ---
# SRCS: List of all .cpp source files
# This is a crucial line: ensure all your .cpp files are listed here.
SRCS = main.cpp api_handler.cpp cli_parser.cpp display_handler.cpp
# OBJS: Object files, derived automatically from SRCS (e.g., main.o, api_handler.o)
OBJS = $(SRCS:.cpp=.o)

# --- VCPKG Integration ---
# VCPKG_ROOT: Path to your vcpkg installation.
# This Makefile expects the VCPKG_ROOT environment variable to be set.
# Example: export VCPKG_ROOT=/home/user/vcpkg
# Using ?= allows VCPKG_ROOT to be set by environment, otherwise defaults to $(HOME)/vcpkg
VCPKG_ROOT ?= $(HOME)/vcpkg

# VCPKG_TARGET_TRIPLET: The vcpkg triplet you used for installing packages.
# This should match the triplet used in your `vcpkg install` command (e.g., x64-linux).
# Using ?= allows this to be overridden by an environment variable if needed, but defaults here.
VCPKG_TARGET_TRIPLET ?= x64-linux

# Add vcpkg include paths to CPPFLAGS
CPPFLAGS += -I$(VCPKG_ROOT)/installed/$(VCPKG_TARGET_TRIPLET)/include

# Define linker paths and libraries separately for correct order
LDFLAGS_PATHS = -L$(VCPKG_ROOT)/installed/$(VCPKG_TARGET_TRIPLET)/lib
LDFLAGS_LIBS = -lcurl -lssl -lcrypto -lz

# Combine all linker flags in the correct order for the link command
# $(LDFLAGS_PATHS) ensures the linker knows where to look before $(LDFLAGS_LIBS) asks for the library.
FINAL_LDFLAGS = $(LDFLAGS_PATHS) $(LDFLAGS_LIBS)

# --- Build Rules ---

# Default rule: 'all' builds the target executable.
all: $(TARGET)

# Rule to link the target executable from object files.
$(TARGET): $(OBJS)
	@echo "--- Configuration Diagnostics ---"
	@echo "VCPKG_ROOT           : $(VCPKG_ROOT)"
	@echo "VCPKG_TARGET_TRIPLET : $(VCPKG_TARGET_TRIPLET)"
	@echo "CPPFLAGS             : $(CPPFLAGS)"
	@echo "LDFLAGS_PATHS        : $(LDFLAGS_PATHS)"
	@echo "LDFLAGS_LIBS         : $(LDFLAGS_LIBS)"
	@echo "OBJS to link         : $(OBJS)"
	@echo "--- Linking $(TARGET)... ---"
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(FINAL_LDFLAGS)
	@echo "--- Build complete: $(TARGET) is ready. ---"

# Rule to compile .cpp source files into .o object files.
# '$<' is an automatic variable representing the first prerequisite (the .cpp file).
# '$@' is an automatic variable representing the target file (the .o file).
%.o: %.cpp
	@echo "--- Compiling $<... ---"
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