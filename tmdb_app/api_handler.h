#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <string>
#include <vector>
#include "movie.h"

struct CURL;

// ApiHandler class is responsible for all interactions with the TMDB API
// This includes constructing API requests, fetching data, and parsing JSON responses
class ApiHandler {
public:
    // Constructor: Initializes the ApiHandler with the API key
    // \@param apiKey The TMDB API key 
    explicit ApiHandler(std::string apiKey);

    // Destructor: Cleans up any resources, like the global curl state
    ~ApiHandler();

    // Fetches movies from the TMDB API based on the specified movie type
    // \@param movieType 
    // \@return A vector of Movie objects 
    // \@throws std::runtime_error if fetching or parsing fails 
    std::vector<Movie> fetchMovies(const std::string& movieType);
private:
    // The TMDB API key
    std::string apiKey_;

    // libcurl handle, managed internally
    CURL* curl_handle_;

    // Static callback function for libcurl to write received data into a string 
    // \@param contents 
    // \@param size 
    // \@param nmemb
    // \@param userp
    // \@return Total size of the data chunk processed 
    static size_t WriteCallback(void* contents, size_t, size, size_t nmemb, void* userp);

    // Fetches raw data from the TMDB API for a given movie type
    // \@param movieType
    // \@param responseBuffer 
    // \@throws std::runtime_error on libcurl errors or non-200 HTTP status codes
    void fetchData(const std::string& movieType, std::string& responseBuffer);

    // Parses the JSON response from the TMDB API into a vector of Movie objects
    // \@param jsonResponse
    // \@return A vector of Movie objects
    // \@throws nlohmann::json::parse_error if JSON is malformed
    // \@throws std::runtime_error if essential fields are missing or structure is unexpected
    std::vector<Movie> parseJson(const std::string& jsonResponse);

};

#endif // API_HANDLER_H