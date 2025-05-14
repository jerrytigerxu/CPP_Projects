#include "display_handler.h"
#include <iostream>
#include <iomanip>
#include <string>

DisplayHandler::DisplayHandler() {

}

// Displays the movie data in a formmated table
void DisplayHandler::displayMoviesTable(const std::vector<Movie>& movies) const {
    if (movies.empty()) {
        std::cout << "No movies found for this category or an error occurred." << std::endl;
        return;
    }

    // Define column widths
    const int idWidth = 10;
    const int titleWidth = 40;
    const int dateWidth = 15;
    const int ratingWidth = 8;
    const int overviewIndent = 2;
    const int maxOverviewLineLength = idWidth + titleWidth + dateWidth + ratingWidth - overviewIndent;

    // Print table header
    std::cout << std::left << std::setw(idWidth) << "ID"
              << std::setw(titleWidth) << "Title"
              << std::setw(dateWidth) << "Release Date"
              << std::setw(ratingWidth) << "Rating"
              << std::endl;

    // Separator line
    std::cout << std::string(idWidth + titleWidth + dateWidth + ratingWidth, '-') << std::endl;

    // Print movie data row by row.
    for (const auto& movie : movies) {
        // Truncate title if it's too long to fit in the allocated width.
        std::string displayTitle = movie.title;
        if (displayTitle.length() > static_cast<size_t>(titleWidth - 1)) { // -1 for potential '...'
            displayTitle = displayTitle.substr(0, titleWidth - 4) + "...";
        }

        std::cout << std::left
                  << std::setw(idWidth) << movie.id
                  << std::setw(titleWidth) << displayTitle
                  << std::setw(dateWidth) << movie.release_date
                  << std::fixed << std::setprecision(1) // Format rating to one decimal place
                  << std::setw(ratingWidth) << movie.vote_average
                  << std::endl;

        // Print overview on a new line(s), indented and wrapped.
        std::cout << std::string(overviewIndent, ' ') << "Overview: ";
        if (movie.overview.empty() || movie.overview == "No overview available.") {
            std::cout << "N/A" << std::endl;
        } else {
            std::string currentOverview = movie.overview;
            size_t startPos = 0;
            bool firstLine = true;
            while(startPos < currentOverview.length()){
                if(!firstLine){
                     // Indent subsequent lines of the same overview
                    std::cout << std::string(overviewIndent + std::string("Overview: ").length(), ' ');
                }
                // Calculate remaining length for the overview line
                size_t lenToPrint = std::min(static_cast<size_t>(maxOverviewLineLength - (firstLine ? std::string("Overview: ").length() : 0)), 
                                             currentOverview.length() - startPos);
                
                // Find last space to wrap nicely
                size_t actualLen = lenToPrint;
                if (startPos + lenToPrint < currentOverview.length()) { // If not the end of the overview
                    size_t lastSpace = currentOverview.rfind(' ', startPos + lenToPrint -1 );
                    if (lastSpace != std::string::npos && lastSpace > startPos) {
                        actualLen = lastSpace - startPos;
                    }
                }
                
                std::cout << currentOverview.substr(startPos, actualLen) << std::endl;
                startPos += actualLen;
                // Skip leading spaces for the next line
                while(startPos < currentOverview.length() && currentOverview[startPos] == ' '){
                    startPos++;
                }
                firstLine = false;
            }
        }
        // Print a separator line after each movie entry.
        std::cout << std::string(idWidth + titleWidth + dateWidth + ratingWidth, '-') << std::endl;
    }

}