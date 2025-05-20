# TMDB CLI - Command Line Interface for The Movie Database
tmdb-cli is a C++ command-line tool that fetches and displays movie information (popular, top-rated, now playing, upcoming) from The Movie Database (TMDB) API.

## Features
- Fetch movie data for different categories:
  - Popular
  - Top Rated
  - Now Playing
  - Upcoming
- Display movie information in a clear, formatted table in the terminal (ID, Title, Release Date, Rating, Overview).
- Sort movie results by title, release date, or rating in ascending or descending order.
- Securely read the TMDB API key from the TMDB_API_KEY environment variable or a .env file.
- Robust error handling for API calls, network issues, and data parsing.

## Prerequisites
- A C++17 compatible compiler (e.g., g++, Clang, MSVC).
- `make` for building the project.
- Git for cloning the repository.
- vcpkg: This project uses vcpkg for managing dependencies (libcurl, nlohmann-json).

## Setup and Installation
### 1. Clone the Repository
```git clone <repository-url>
cd tmdb_cli```

### 2. Install vcpkg (if not already installed)
`vcpkg` is a C/C++ library manager. Follow the official instructions to install it:
- Official vcpkg Quick Start: https://vcpkg.io/en/getting-started.html
Typically, this involves cloning the vcpkg repository and running its bootstrap script:

```# Clone vcpkg (choose a suitable location, e.g., your home directory or /opt)
git clone [https://github.com/microsoft/vcpkg.git](https://github.com/microsoft/vcpkg.git)
cd vcpkg

# Bootstrap vcpkg (Windows)
./bootstrap-vcpkg.bat

# Bootstrap vcpkg (Linux/macOS)
./bootstrap-vcpkg.sh```

After installation, vcpkg recommends integrating with your user environment or build system. One common step is to set the VCPKG_ROOT environment variable to point to your vcpkg installation directory.

- Linux/macOS (add to ~/.bashrc, ~/.zshrc, etc.):

```export VCPKG_ROOT="/path/to/your/vcpkg" 
# Example: export VCPKG_ROOT="$HOME/vcpkg"```

- Windows: Set VCPKG_ROOT as a system or user environment variable.

The provided Makefile attempts to locate vcpkg in common paths like $(HOME)/vcpkg, /opt/vcpkg, or /usr/local/vcpkg if VCPKG_ROOT is not explicitly set in your environment or passed to make.

### 3. Install Dependencies using vcpkg
Navigate to your vcpkg directory (defined by VCPKG_ROOT) and install the required libraries. You'll need to specify your target triplet (e.g., x64-linux, x64-windows, x64-osx).

```# Example for Linux (from your VCPKG_ROOT directory):
./vcpkg install curl nlohmann-json --triplet x64-linux

# Example for Windows (from your VCPKG_ROOT directory):
./vcpkg install curl nlohmann-json --triplet x64-windows

# Example for macOS (from your VCPKG_ROOT directory):
./vcpkg install curl nlohmann-json --triplet x64-osx
```
Note: If you want to attempt static linking, you might use a static triplet like x64-linux-static or x64-windows-static. However, fully static linking libcurl and its dependencies (like OpenSSL) can be complex and might require additional configuration or system libraries.

### 4. Set up TMDB API Key
This application requires a TMDB API key.

- Option 1: Environment Variable (Recommended)
Set the TMDB_API_KEY environment variable:

```export TMDB_API_KEY="your_actual_api_key"```

To make this permanent, add it to your shell's configuration file (e.g., .bashrc, .zshrc, .profile).

- Option 2: .env File
Create a file named .env in the root of the tmdb_cli project directory (i.e., the same directory as your Makefile and src/include folders).
The content should be:

```TMDB_API_KEY=your_actual_api_key```

Important: Add .env to your .gitignore file to prevent committing your API key to version control. Your .gitignore should include:

```
# .gitignore
.env
build/
*.o
tmdb_app ```

### 5. Build the Application
Navigate to the root of the tmdb_cli project directory (where the Makefile is located) and run:

```make```

This will compile the source files and create an executable named tmdb_app inside a build/ directory.

To specify a different vcpkg triplet during the build (if the default x64-linux is not correct for your system):

```make VCPKG_TRIPLET=your-triplet-here 
# e.g., make VCPKG_TRIPLET=x64-windows```

If VCPKG_ROOT is not set as an environment variable and not automatically detected correctly by the Makefile, you can also specify it:

```make VCPKG_ROOT=/path/to/your/vcpkg VCPKG_TRIPLET=your-triplet```

## Usage
The executable will be located at build/tmdb_app.

Important Note on Running the Executable:
For the application to find the .env file (if you are using that method for the API key), you should run the executable from the project's root directory. This is the directory where your .env file is located.

Correct way to run (from project root):

```./build/tmdb_app --type <movie_type> [options] ```

If you navigate into the build/ directory and run ./tmdb_app, it will look for .env inside the build/ directory, which is likely not where you placed it.

### Command-Line Interface (CLI):

```./build/tmdb_app --type <movie_type> [options]```

### Mandatory Arguments:

--type <type>: Specify the category of movies to fetch.
Allowed types: popular, top (for top-rated), playing (for now playing), upcoming.

### Optional Arguments:

--sort-by <field>: Field to sort the results by.

Allowed fields: title, date (release date), rating.

--order <asc|desc>: Sort order (default: asc if --sort-by is used). Requires --sort-by.

Allowed orders: asc (ascending), desc (descending).

--help, -h: Display the help message and exit.

Examples (run from project root):

Show popular movies:

```./build/tmdb_app --type popular```

Show top-rated movies, sorted by rating in descending order:

```./build/tmdb_app --type top --sort-by rating --order desc```

Show upcoming movies, sorted by release date (ascending by default):

```./build/tmdb_app --type upcoming --sort-by date```

Display help:

```./build/tmdb_app --help```

---

## Development
### Directory Structure


```tmdb_cli/
├── .git/
├── .gitignore
├── build/              # Compiled files and executable
├── include/            # Header files (.h)
│   ├── api_handler.h
│   ├── cli_parser.h
│   ├── display_handler.h
│   └── movie.h
├── src/                # Source files (.cpp)
│   ├── api_handler.cpp
│   ├── cli_parser.cpp
│   ├── display_handler.cpp
│   └── main.cpp
├── .env                # For TMDB_API_KEY (user-created, in project root)
├── Makefile            # Build instructions
└── README.md           # This file```

### Building for Debug
To build with debugging symbols:

```
make clean
make CXXFLAGS="-std=c++17 -Wall -Wextra -pedantic -g"
```
### Cleaning Build Files
```make clean```

## Contributing
Please refer to the project's contribution guidelines if available.

## License
Specify your project's license here (e.g., MIT, Apache 2.0). If not specified, it's proprietary.
