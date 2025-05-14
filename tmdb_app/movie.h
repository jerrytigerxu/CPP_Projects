#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <vector>

// --- Movie Struct Definition ---
// Represents a movie with its essential fields fetched from TMDB
struct Movie {
    int id;
    std::string title; // Unique TMDB identifier for the movie 
    std::string release_date; // The release date of the movie (YYYY-MM-DD)
    double vote_average; // The average user rating for the movie 
    std::string overview; // A brief summary or overview of the movie

    // Default constructor
    Movie() : id(0), vote_average(0.0) {}

    // Parametrized constructor 
    Movie(int i, std::string t, std::string rd, double va, std::string ov)
        : id(i), title(std::move(t)), release_date(std::move(rd)), vote_average(va), overview(std::move(ov)) {}
};

#endif // MOVIE_H