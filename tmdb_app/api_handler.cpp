#include "api_handler.h"
#include <curl/curl.h>      // For libcurl functionalities
#include "nlohmann/json.hpp" // For JSON parsing
#include <iostream>         // For std::cerr, std::cout (can be removed if logging is handled differently)
#include <stdexcept>        // For std::runtime_error
#include <map>              // For mapping movie types to API paths

// Use the nlohmann::json namespace
using json = nlohmann::json;

// --- Constructor and Destructor ---

// Constructor: Initializes the ApiHandler with the TMDB API key and sets up libcurl.
ApiHandler::ApiHandler(std::string apiKey) : apiKey_(std::move(apiKey)), curl_handle_(nullptr) {
    // Initialize libcurl globally. This should ideally be done once per application.
    // If multiple ApiHandler instances are created, this could be moved to main()
    // or a dedicated initialization function.
    CURLcode global_init_res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (global_init_res != CURLE_OK) {
        // Throw an exception if global initialization fails, as the handler cannot function.
        throw std::runtime_error("Failed to initialize libcurl globally: " + std::string(curl_easy_strerror(global_init_res)));
    }

    // Get a curl easy handle. This handle will be reused for requests.
    curl_handle_ = curl_easy_init();
    if (!curl_handle_) {
        curl_global_cleanup(); // Clean up global state if handle init fails
        throw std::runtime_error("Failed to initialize libcurl easy handle.");
    }
}

// Destructor: Cleans up the libcurl easy handle and global state.
ApiHandler::~ApiHandler() {
    if (curl_handle_) {
        curl_easy_cleanup(curl_handle_); // Clean up the easy handle
    }
    // Global cleanup. Similar to init, this should ideally be done once when the application exits.
    curl_global_cleanup();
}

// --- Public Methods ---

// Fetches and parses movies of a specific type.
// This is the primary public interface for getting movie data.
std::vector<Movie> ApiHandler::fetchMovies(const std::string& movieType) {
    std::string jsonResponse;
    // Step 1: Fetch the raw data from the API.
    fetchData(movieType, jsonResponse); 
    
    // Step 2: Parse the JSON response into Movie objects.
    return parseJson(jsonResponse); 
}

// --- Private Helper Methods ---

// Callback function for libcurl to handle incoming data.
// It appends the received data chunks to the string pointed to by userp.
size_t ApiHandler::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realSize = size * nmemb;
    std::string* responseBuffer = static_cast<std::string*>(userp);
    if (responseBuffer == nullptr) {
        // Should not happen if CURLOPT_WRITEDATA is set correctly.
        return 0; 
    }
    try {
        responseBuffer->append(static_cast<char*>(contents), realSize);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation error in WriteCallback: " << e.what() << std::endl;
        return 0; // Signal an error to libcurl
    }
    return realSize; // Inform libcurl how many bytes were processed
}

// Fetches raw data from the TMDB API.
// Constructs the URL, sets libcurl options, and performs the HTTP GET request.
void ApiHandler::fetchData(const std::string& movieType, std::string& responseBuffer) {
    if (!curl_handle_) { // Safety check
        throw std::runtime_error("curl_handle_ is not initialized in fetchData.");
    }

    responseBuffer.clear(); // Ensure the buffer is empty before a new request

    // Map user-friendly movie types to TMDB API paths
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
        if (http_code == 401) { // Unauthorized
            errorMsg += "\nHint: This often means an invalid or missing API key. Please verify your TMDB_API_KEY.";
        } else if (http_code == 404) { // Not Found
             errorMsg += "\nHint: The requested resource was not found on the server.";
        }
        if (!responseBuffer.empty()) {
            // Include a snippet of the response if available, helpful for API errors.
            errorMsg += "\nResponse snippet: " + responseBuffer.substr(0, 200) + (responseBuffer.length() > 200 ? "..." : "");
        }
        throw std::runtime_error(errorMsg);
    }

    // If we reach here, the request was successful and responseBuffer contains the data.
}

// Parses the JSON response string into a vector of Movie objects.
std::vector<Movie> ApiHandler::parseJson(const std::string& jsonResponse) {
    std::vector<Movie> movies;
    try {
        json data = json::parse(jsonResponse);

        // TMDB API usually returns results in a "results" array.
        if (data.contains("results") && data["results"].is_array()) {
            for (const auto& item : data["results"]) {
                Movie movie;
                // Use .value("key", default_value) for safer access.
                // This prevents exceptions if a key is missing and provides a fallback.
                movie.id = item.value("id", -1); // -1 indicates missing or invalid ID
                movie.title = item.value("title", "N/A");
                movie.release_date = item.value("release_date", "N/A");
                movie.vote_average = item.value("vote_average", 0.0);
                movie.overview = item.value("overview", "No overview available.");

                // Basic validation: skip if essential data like ID or title is missing.
                if (movie.id == -1 && movie.title == "N/A") {
                    std::cerr << "Warning: Parsed a movie item with missing ID and title. Skipping." << std::endl;
                    continue; 
                }
                movies.push_back(movie);
            }
        } else {
            // Handle cases where the "results" array is not found.
            // This could be an API error message or an unexpected format.
            if (data.contains("status_message")) {
                // TMDB often includes a status_message for errors.
                throw std::runtime_error("TMDB API Error: " + data.value("status_message", "Unknown error from API."));
            } else {
                 // Log the beginning of the problematic JSON for debugging
                std::string responseStart = jsonResponse.substr(0, std::min((size_t)500, jsonResponse.length()));
                throw std::runtime_error("Failed to parse movies: 'results' array not found or not an array in JSON response. Response starts with: " + responseStart);
            }
        }
    } catch (const json::parse_error& e) {
        // Re-throw with more context or handle specifically.
        throw std::runtime_error("JSON parse error: " + std::string(e.what()) + " at byte " + std::to_string(e.byte));
    } catch (const json::type_error& e) {
        // Handle errors like accessing a field with the wrong type.
        throw std::runtime_error("JSON type error during parsing: " + std::string(e.what()));
    }
    return movies;
}
