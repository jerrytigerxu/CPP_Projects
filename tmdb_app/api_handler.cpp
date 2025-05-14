#include "api_handler.h"
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <iostream>
#include <stdexcept>
#include <map>

using json = nlohmann::json;

// --- Constructor and Destructor ---

// Constructor: Initializes the ApiHandler with the API key and libcurl handle
ApiHandler::ApiHandler(std::string apiKey) : apiKey_(std::move(apiKey)), curl_handle_(nullptr) {
    // Initialize libcurl globally
    CURLcode global_init_res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (global_init_res != CURLE_OK) {
        throw std::runtime_error("Failed to initialize libcurl globally: " + std::string(curl_easy_strerror(global_init_res)));
    }
    curl_handle_ = curl_easy_init();
    if (!curl_handle_) {
        curl_global_cleanup();
        throw std::runtime_error("Failed to initialize libcurl easy handle.");
    }
}

// Destructor: Cleans up the libcurl handle and global state
ApiHandler::~ApiHandler() {
    if (curl_handle_) {
        curl_easy_cleanup(curl_handle_);
    }
    curl_global_cleanup();
}

// --- Public Methods ---

// Fetches and parses movies of a specific type 
std::vector<Movie> ApiHandler::fetchMovies(const std::string& movieType) {
    std::string jsonResponse;
    fetchData(movieType, jsonResponse);
    return parseJson(jsonResponse);
}

// --- Private Helper Methods ---

// Callback function for libcurl to handle incoming data
// It appends the received data chunks to the string pointed to by userp
size_t ApiHandler::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    // Calculate the total size of the data chunk received 
    size_t realSize = size * nmemb; 
    // Cast userp pointer back to a std::string pointer 
    std::string* responseBuffer = static_cast<std::string*>(userp);
    if (responseBuffer == nullptr) {
        return 0;
    }
    try {
        // Append the received data chunk to the string 
        // contents is not null-terminated, so specify length explicitly 
        responseBuffer->append(static_cast<char*>(contents), realSize);
    } catch (const std::bad_alloc& e) {
        // Handle potential memory allocation errors during append 
        std::cerr << "Memory allocation error in WriteCallback: " << e.what() << std::endl;
        return 0;
    }
    // Return the number of bytes processed to libcurl
    return realSize; 
}

// Fetches raw data from the TMDB API
// Constructs the URL, sets libcurl options, and performs the HTTP GET request
void ApiHandler::fetchData(const std::string& apiKey, const std::string& movieType, std::string& responseBuffer) {
    if (!curl_handle_) {
        throw std::runtime_error("curl_handle_ is not initialized in fetchData.");
    }
    responseBuffer.clear();

    std::map<std::string, std::string> movieTypeToPath = {
        {"popular", "popular"},
        {"top", "top_rated"},
        {"playing", "now_playing"},
        {"upcoming", "upcoming"}
    };

    std::string apiMovieTypePath;
    auto it = movieTypeToPath.find(movieType);
    if (it != movieTypeToPath.end()) {
        apiMovieTypePath = it->second;
    } else {
        // If the movieType is not recognized, default to "popular" or throw an error.
        // For now, throwing an error is safer to indicate incorrect usage.
        throw std::runtime_error("Unknown movie type provided to fetchData: " + movieType);
    }
    
    // Construct the API URL.
    std::string url = "https://api.themoviedb.org/3/movie/" + apiMovieTypePath + "?api_key=" + apiKey_;
    // For more complex queries, you might add language, page, region, etc.
    // e.g., url += "&language=en-US&page=1";

    // --- Set libcurl Options ---
    // 1. Set the URL.
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    // 2. Set the callback function for writing data.
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, WriteCallback);
    // 3. Pass the responseBuffer to the callback.
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &responseBuffer);
    // 4. Set "accept: application/json" header.
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "accept: application/json");
    if (!headers) {
         // Handle error if slist append fails, though unlikely for a single header.
         std::cerr << "Warning: Failed to create curl_slist for headers." << std::endl;
    }
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, headers);
    // 5. Follow HTTP redirects.
    curl_easy_setopt(curl_handle_, CURLOPT_FOLLOWLOCATION, 1L);
    // 6. Set a reasonable timeout (e.g., 10 seconds).
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, 10L);
    // 7. Enable verbose mode for debugging (optional).
    // curl_easy_setopt(curl_handle_, CURLOPT_VERBOSE, 1L);


    // --- Perform the HTTP request ---
    CURLcode res = curl_easy_perform(curl_handle_);

    // --- Clean up headers ---
    if (headers) {
        curl_slist_free_all(headers);
    }

    // --- Check for libcurl errors ---
    if (res != CURLE_OK) {
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }

    // --- Get and check HTTP status code ---
    long http_code = 0;
    curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        std::string errorMsg = "HTTP request failed with status code: " + std::to_string(http_code);
        if (!responseBuffer.empty()) {
            // Include a snippet of the response if available, helpful for API errors.
            errorMsg += "\nResponse snippet: " + responseBuffer.substr(0, 200) + (responseBuffer.length() > 200 ? "..." : "");
        }
        throw std::runtime_error(errorMsg);
    }


}


// Parses the JSON response string into a vector of Movie objects
std::vector<Movie> ApiHandler::parseJson(const std::string& jsonResponse) {
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