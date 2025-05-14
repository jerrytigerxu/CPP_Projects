#include <iostream> // For standard IO operations
#include <vector> // For potentially storing command-line arguments or movie data 
#include <string> // For handling strings 
#include <fstream> // For file input (reading .env)
#include <cstdlib> // For exit()
#include <stdexcept> // For runtime_error

#include "movie.h" // Definition of Movie struct
#include "cli_parser.h" // For CliParser class
#include "api_handler.h" // For ApiHandler class
#include "display_handler.h" // For DisplayHandler class

// --- Helper Function to Read API Key ---
// Reads the TMDB API key from an environment variable "TMDB_API_KEY"
// or from a .env file as a fallback.
// @return The API key as a string.
// @throws std::runtime_error if the API key cannot be found.
std::string getApiKey() {
    // 1. Try to get API key from environment variable
    const char* apiKeyEnv = std::getenv("TMDB_API_KEY");
    if (apiKeyEnv != nullptr && std::string(apiKeyEnv).length() > 0) {
        std::cout << "Successfully retrieved API key from TMDB_API_KEY environment variable." << std::endl;
        return std::string(apiKeyEnv);
    }

    // 2. Fallback: Try to read from .env file (simple implementation)
    std::cout << "TMDB_API_KEY environment variable not set or empty. Trying to read from .env file..." << std::endl;
    std::ifstream envFile(".env");
    std::string line;
    std::string keyName = "TMDB_API_KEY";

    if (envFile.is_open()) {
        while (std::getline(envFile, line)) {
            // Remove whitespace and comments
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t separatorPos = line.find('=');
            if (separatorPos != std::string::npos) {
                std::string currentKey = line.substr(0, separatorPos);
                currentKey.erase(currentKey.find_last_not_of(" \t") + 1); // Trim trailing space from key

                if (currentKey == keyName) {
                    std::string value = line.substr(separatorPos + 1);
                    // Remove potential quotes and leading/trailing whitespace from value
                    value.erase(0, value.find_first_not_of(" \t\n\r\f\v\"'"));
                    value.erase(value.find_last_not_of(" \t\n\r\f\v\"'") + 1);
                    if (!value.empty()) {
                        std::cout << "Successfully retrieved API key from .env file." << std::endl;
                        envFile.close();
                        return value;
                    }
                }
            }
        }
        envFile.close();
    } else {
        std::cerr << "Warning: Could not open .env file." << std::endl;
    }

    throw std::runtime_error("TMDB_API_KEY not found. Please set it as an environment variable or in a .env file.");
}

int main(int argc, char* argv[]) {
    // --- 1. Parse Command Line Arguments
    CliParser cliParser;
    ParsedArgs parsedArgs = cliParser.parse(argc, argv);

    // Handle help request
    if (parsedArgs.helpRequested) {
        std::cout << cliParser.getUsageString(argc > 0 ? argv[0] : "tmdb-app") << std::endl;
        return 0;
    }
    // Handle parsing errors
    if (parsedArgs.error) {
        std::cerr << "Argument Error: " << parsedArgs.errorMessage << std::endl;
        return 1;
    }

    std::cout << "Requested movie type: " << parsedArgs.movieType << std::endl;

    // --- 2. Get API Key ---
    std::string apiKey;
    try {
        apiKey = getApiKey();
    } catch (const std::runtime_error& e) {
        std::cerr << "API Key Error: " << e.what() << std::endl;
        return 1;
    }

    // --- 3. Fetch and Display Movie Data ---
    try {
        // Initialize ApiHandler with the API key
        ApiHandler apiHandler(apiKey);

        std::cout << "\nFetching movie data from TMDB for type: " << parsedArgs.movieType << "..." << std::endl;
        std::vector<Movie> movies = apiHandler.fetchMovies(parsedArgs.movieType);
        std::cout << "Successfully fetched and parsed " << movies.size() << " movies." << std::endl;

        // Display the movies
        DisplayHandler displayHandler;
        std::cout << "\nDisplaying movie data..." << std::endl;
        displayHandler.displayMoviesTable(movies);

    } catch (const std::runtime_error& e) {
        // Catch errors from ApiHandler (network, API errors, JSON parsing issues)
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        // Catch any other unexpected standard exceptions
        std::cerr << "\nAn unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

