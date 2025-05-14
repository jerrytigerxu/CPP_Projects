#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <string>
#include <vector>
#include "movie.h" // Assuming movie.h is in the same directory or include path
#include <curl/curl.h> // Include libcurl header directly for CURL type

// ApiHandler class is responsible for all interactions with The Movie Database (TMDB) API.
// This includes constructing API requests, fetching data, and parsing JSON responses.
class ApiHandler {
public:
    // Constructor: Initializes the ApiHandler with the TMDB API key.
    // @param apiKey The TMDB API key.
    explicit ApiHandler(std::string apiKey);

    // Destructor: Cleans up any resources, like the global curl state.
    ~ApiHandler();

    // Fetches movies from the TMDB API based on the specified movie type.
    // @param movieType A string representing the category of movies to fetch 
    //                  (e.g., "popular", "top_rated", "now_playing", "upcoming").
    // @return A vector of Movie objects.
    // @throws std::runtime_error if fetching or parsing fails.
    std::vector<Movie> fetchMovies(const std::string& movieType);

private:
    // The TMDB API key.
    std::string apiKey_;

    // libcurl handle, managed internally.
    // CURL type is now known from #include <curl/curl.h>
    CURL* curl_handle_; 

    // Static callback function for libcurl to write received data into a string.
    // @param contents Pointer to the data received.
    // @param size Size of each data element.
    // @param nmemb Number of data elements.
    // @param userp User-provided pointer (to a std::string in this case).
    // @return Total size of the data chunk processed.
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    // Fetches raw data from the TMDB API for a given movie type.
    // @param movieType The category of movies to fetch.
    // @param responseBuffer A string reference to store the raw JSON response.
    // @throws std::runtime_error on libcurl errors or non-200 HTTP status codes.
    void fetchData(const std::string& movieType, std::string& responseBuffer);

    // Parses the JSON response from TMDB API into a vector of Movie objects.
    // @param jsonResponse The raw JSON string.
    // @return A vector of Movie objects.
    // @throws nlohmann::json::parse_error if JSON is malformed.
    // @throws std::runtime_error if essential fields are missing or structure is unexpected.
    std::vector<Movie> parseJson(const std::string& jsonResponse);
};

#endif // API_HANDLER_H
