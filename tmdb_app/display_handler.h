#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <vector>
#include "movie.h"

// The DisplayHandler class is responsible for formatting and displaying
// movie data to the console
class DisplayHandler {
public: 
    // Constructor
    DisplayHandler();

    // Displays a list of movies in a formatted table on the console
    // \@param movies A constant reference to a vector of Movie objects to be displayed
    void displayMoviesTable(const std::vector<Movie>& movies) const;

private:

};

#endif // DISPLAY_HANDLER_H