#include <iostream> // For standard IO operations
#include <vector> // For potentially storing command-line arguments or movie data 
#include <string> // For handling strings 
#include <fstream> // For file input (reading .env)
#include <sstream> // For potentially parsing lines
#include <cstdlib> // For exit()
#include <stdexcept> // For runtime_error
#include <set> // For storing valid movie types 
#include <curl/curl.h> // For making HTTP requests with libcurl
#include "nlohmann/json.hpp" // For parsing JSON 

// Use the nlohmann::json namespace 
using json = nlohmann::json;

// --- Movie Struct Definition ---
struct Movie {
    int id;
    std::string title;
    std::string release_date;
    double vote_average;
    std::string overview;
 };

// Reads the TMDB API key from a specified .env file 
std::string getApiKeyFromEnvFile(const std::string& filename = ".env");

// Parses command line arguments to find the --type flag and its value 
// Returns the movie type string or throws an exception on error
std::string parseArguments(int argc, char* argv[]);

// Callback function for libcurl to write received data into a string (in order to optimize the receiving of data in chunks)
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp); 

// Fetches movie data from the TMDB API for a given type
std::string fetchData(const std::string& apiKey, const std::string& movieType);

// Parses the JSON response from the TMDB API and returns a vector of Movie structs 
std::vector<Movie> parseJson(const std::string& jsonResponse);

// void displayMoviesTable(const std::vector<Movie>& movies);

int main(int argc, char* argv[]) {
    // --- 1. Get API Key ---
    std::string apiKey;
    try {
        apiKey = getApiKeyFromEnvFile(); // Read from .env file 
        std::cout << "Successfully retrieved API key from .env file." << std::endl;
        // For debugging: std::cout << "API Key: " << apiKey << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; // Exit if API key is missing or file error
    }
    
    
    // --- 2. Parse Command Line Arguments ---
    std::string movieType;
    try {
        movieType = parseArguments(argc, argv);
        std::cout << "Requested movie type: " << movieType << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Argument Error: " << e.what() << std::endl; 
        // Print usage instructions on argument error 
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "tmdb-app") << " --type <playing|popular|top|upcoming>" << std::endl;
        return 1; 
    } catch (const std::exception& e) {
        // Catch any other potential standard exceptions during parsing 
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; 
    }

    // --- 3. Fetch Data using libcurl ---
    std::string jsonResponse;
    try {
        std::cout << "\nFetching movie data from TMDB for type: " << movieType << "..." << std::endl;
        // Call fetchData with the retrieved API key and movie type 
        std::string jsonResponse = fetchData(apiKey, movieType);

        // Print the raw JSON response to verify 
        std::cout << "\n--- Raw JSON Response---" << std::endl;
        std::cout << jsonResponse << std::endl;
        std::cout << "--- End of Response ---" << std::endl;
    } catch (const std::runtime_error& e) {
        // Catch errors specifically from the fetchData process
        std::cerr << "\nFetch Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        // Catch any other unexpected errors during fetch 
        std::cerr << "\nUnexpected Error: " << e.what() << std::endl;
        return 1; 
    }
    

    // --- 4. Parse JSON Response ---
    std::vector<Movie> movies; // Declare vector to store parsed movies 
    try {
        std::cout << "\nParsing JSON response..." << std::endl;
        movies = parseJson(jsonResponse); // Call the parseJson function
        std::cout << "Successfully parsed " << movies.size() << " movies." << std::endl;
    } catch (const json::parse_error& e) {
        std::cerr << "\nJSON Parse Error: " << e.what() << std::endl;
        std::cerr << "Error occurred at byte " << e.byte << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\nError during JSON processing: " << e.what() << std::endl;
        return 1;
    }



    // --- 5. Display Data (Placeholder) ---
    std::cout << "\nDisplaying movie data..." << std::endl;
    // TODO: Implement display logic, likely using a formatted table

    return 0;
}

// --- Function Implementations (Placeholders/Initial Structure) ---

/**
 * \@brief Reads the TMDB API key from a specified .env file 
 * \@param filename The name of the environment file (defaults to ".env")
 * \@return The API key as a string, or an empty string if not found or error occurs
 */
std::string getApiKeyFromEnvFile(const std::string& filename) {
    std::ifstream envFile(filename); // Attempt to open the file 
    std::string line;
    std::string key = "TMDB_API_KEY";

    // Check if the file was opened successfully
    if (!envFile.is_open()) {
        std::cerr << "Error: Could not open the environment file: " << filename << std::endl;
        std::cerr << "Please ensure the '.env' file exists in the same directory as the executable." << std::endl;
        return "";
    }

    // Read the file line by line 
    while (std::getline(envFile, line)) {
        // Remove leading/trailing whitespace 
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        if (line.empty() || line[0] == '#') {
            continue; // Skip empty lines and comments
        }

        // Find the position of the '=' separator
        size_t separatorPos = line.find('=');
        if (separatorPos != std::string::npos) {
            std::string currentKey = line.substr(0, separatorPos);
            // Trim potential whitespace around the key
            currentKey.erase(currentKey.find_last_not_of(" \t") + 1);

            if (currentKey == key) {
                std::string value = line.substr(separatorPos + 1);
                // Remove potential quotes and whitespace around the value 
                value.erase(0, value.find_first_not_of(" \t\n\r\f\v\"'"));
                value.erase(value.find_last_not_of(" \t\n\r\f\v\"'") + 1);
                envFile.close();
                return value;
            }
        }
    }

    envFile.close(); // Close the file if key wasn't found 
    std::cerr << "Error: " << key << " not found in " << filename << std::endl;
    return ""; // Return empty string if key wasn't found
}

/**
 * \@brief Parses command line arguments to find the --type flag and its value
 * \@param argc Argument count from main()
 * \@param argv Argument vector from main() 
 * \@return The movie type string specified by the user 
 * \@throws std::invalid_argument if arguments are missing, the flag is incorrect,
 * or the type value is not one of the allowed options 
 */
std::string parseArguments(int argc, char* argv[]) {
    // Expected usage: tmdb-app --type <value>
    // Expect exactly 3 arguments: program_name --type value

    // Define the set of allowed movie types (to make it easier to add/remove options later)
    const std::set<std::string> allowedTypes = {
        "popular",
        "top", 
        "playing", 
        "upcoming"
        // Add more types here as needed
    };

    if (argc != 3) {
        if (argc < 3) {
            throw std::invalid_argument("Missing arguments.");
        } else {
            throw std::invalid_argument("Too many arguments.");
        }
    }

    // The second argument (index 1) should be the flag "--type"
    std::string flag = argv[1];
    if (flag != "--type") {
        throw std::invalid_argument("Invalid flag ' " + flag + "'. Expected '--type'.");
    }

    // The third argument (index 2) is the movie type value 
    std::string typeValue = argv[2];
    // Valid the type value against the allowed options
    // set::count returns 1 if the element exists, 0 otherwise 
    if (allowedTypes.count(typeValue) == 0) {
        // Constructing the error message dynamically 
        std::stringstream ss;
        ss << "Invalid movie type specified: '" << typeValue << "'. Must be one of: ";
        bool first = true;
        for (const auto& type: allowedTypes) {
            if (!first) ss << ", ";
            ss << type; 
            first = false;
        }
        throw std::invalid_argument(ss.str());
    }

    // If all checks pass, return the valid movie type 
    return typeValue;
}

/**
 * \@brief Callback function for libcurl. Appends received data to a std::string 
 * This function matches the signature required by CURLOPT_WRITEFUNCTION
 * \@param contents Pointer to the data received by libcurl 
 * \@param size Size of each data element 
 * \@param nmemb Number of data elements received in this chunk 
 * \@param userp User-provided pointer 
 * \@return Total size of the received data chunk, or 0 on error 
 */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    // Calculate the total size of the data chunk received 
    size_t realSize = size * nmemb; 
    // Cast userp pointer back to a std::string pointer 
    std::string* readBuffer = static_cast<std::string*>(userp);
    if (readBuffer == nullptr) {
        return 0;
    }
    try {
        // Append the received data chunk to the string 
        // contents is not null-terminated, so specify length explicitly 
        readBuffer->append(static_cast<char*>(contents), realSize);
    } catch (const std::bad_alloc& e) {
        // Handle potential memory allocation errors during append 
        std::cerr << "Memory allocation error in WriteCallback: " << e.what() << std::endl;
        return 0;
    }
    // Return the number of bytes processed to libcurl
    return realSize; 
}

/**
 * \@brief Fetches movie data from the TMDB API for a given type using libcurl
 * \@param apiKey The TMDB API key 
 * \@param movieType The category of movies to fetch (e.g., "popular", "top", etc.)
 * \@return The raw JSON response as a string 
 * \@throws std::runtime_error on libcurl errors or non-200 HTTP status codes
 */
std::string fetchData(const std::string& apiKey, const std::string& movieType) {
    CURL *curl = nullptr; // libcurl handle
    CURLcode res; // Result code from libcurl operations 
    std::string readBuffer; // String to store the API response 
    long http_code = 0; // To store the HTTP status code from the response 
    struct curl_slist *headers = nullptr; // List of custom HTTP headers 

    std::string apiMovieTypePath = movieType;
    if (movieType == "top") {
        apiMovieTypePath = "top_rated";
    } else if (movieType == "playing") {
        apiMovieTypePath = "now_playing";
    }

    // --- Construct API URL --- 
    // Base URL for TMDB API v3 movie endpoints 
    std::string url = "https://api.themoviedb.org/3/movie/" + apiMovieTypePath + "?api_key=" + apiKey; 

    // --- Initialize libcurl ---
    curl_global_init(CURL_GLOBAL_DEFAULT); // Initialize libcurl globally
    curl = curl_easy_init(); // Get a curl easy handle

    if (!curl) {
        curl_global_cleanup(); // Cleanup global state if handle init fails 
        throw std::runtime_error("Failed to initialize libcurl easy handle.");
    }

    try {
        // --- Set libcurl Options ---

        // 1. Set the URL to fetch 
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 

        // 2. Set the callback function to process received data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); 

        // 3. Pass the readBuffer string to the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // 4. Set up accept header 
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "accept: application/json");
        if (headers) { // Only set if header list creation succeeded
             curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        } else {
            // Optional: Log a warning if header creation failed, but proceed
            std::cerr << "Warning: Failed to create accept header." << std::endl;
        }

        // 5. Follow HTTP redirects  
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        // 6. Set a reasonable timeout 
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 seconds timeout 

        // --- Perform the HTTP request ---
        res = curl_easy_perform(curl);

        // --- Check for libcurl errors ---
        if (res != CURLE_OK) {
            // If curl_easy_perform failed, throw an error with details 
            std::string errorMsg = "curl_easy_perform() failed: ";
            errorMsg += curl_easy_strerror(res);
            if (headers) curl_slist_free_all(headers); // Free headers if they were set
            throw std::runtime_error(errorMsg);
        } 

        // --- Get and check HTTP status code ---
        // Retriev the HTTP response code from the completed transfer 
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            // If status code is not 200 OK, throw an error
            std::string errorMsg = "HTTP request failed with status code: " + std::to_string(http_code);
            if (!readBuffer.empty()) {
                errorMsg += "\nResponse snipper: " + readBuffer.substr(0, 200) + (readBuffer.length() > 200 ? "..." : "");
            }
            throw std::runtime_error(errorMsg);
        }

        // --- Cleanup ---
        curl_easy_cleanup(curl); // Clean up the easy handle
        curl_slist_free_all(headers); // Free the list of headers
        curl_global_cleanup(); // Clean up global libcurl state 
    } catch (...) {
        // --- Exception-safe cleanup ---
        // Ensure cleanup happens even if an exception occurred within the try block
        if (curl) curl_easy_cleanup(curl); 
        if (headers) curl_slist_free_all(headers);
        curl_global_cleanup();
        throw; // Re-throw the caught exception
    }

    // If everything succeeded, return the accumulated response data
    return readBuffer; 

}


/**
 * \@return A vector Movie objects
 * \@throws nlohmann::json::parse_error if the JSON is malformed
 * \@throws std::runtime_error if the JSON structure is missing or 
 * if essential movie fields are missing or have incorrect types
 */
std::vector<Movie> parseJson(const std::string& jsonResponse) {
    std::vector<Movie> movies; 
    try {
        json data = json::parse(jsonResponse); 

        // Check if the top-level 'results' key exists and is an array
        if (data.contains("results") && data["results"].is_array()) {
            for (const auto& item : data["results"]) {
                Movie movie;

                // Safely access fields, providing defaults or handling missing keys 
                movie.id = item.value("id", -1);
                movie.title = item.value("title", "N/A");
                movie.release_date = item.value("release_date", "N/A");
                movie.vote_average = item.value("vote_average", 0.0);
                movie.overview = item.value("overview", "No overview available.");

                // Perform basic validation if needed 
                if (movie.id == -1 && movie.title == "N/A") {
                    std::cerr << "Warning. Parsed a movie item with missing ID and title. Skipping..." << std::endl;
                    continue;
                }
                movies.push_back(movie);
            }
        } else {
            // If 'results' is missing or not an array 
            if (data.contains("status_message")) {
                throw std::runtime_error("TMDB API Error: " + data.value("status_message", "Unknown error."));        
            } else {
                throw std::runtime_error("Failed to parse movies: 'results' array not found.");
            }
        }
    } catch (const json::parse_error& e) {
        throw e;
    } catch (const json::type_error& e) {
        throw std::runtime_error("JSON type error during parsing: " + std::string(e.what()));
    }

    return movies; // Return the vector of parsed movies
}