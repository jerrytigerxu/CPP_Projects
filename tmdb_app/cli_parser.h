#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>
#include <set>

// Structure to hold the result of parsing command-line arguments
struct ParsedArgs {
    std::string movieType;
    std::string sortByField; 
    std::string sortOrder;
    bool helpRequested;
    bool error;
    std::string errorMessage;

    // Default constructor
    ParsedArgs() : 
        movieType(""),
        sortByField(""),
        sortOrder("asc"),
        helpRequested(false),
        error(false),
        errorMessage("")
    {}
};

// CliParser class is responsible for parsing command-line arguments
// to determine the requested movie type or if help is needed
class CliParser {
public:
    CliParser();

    // Parses the command-line arguments
    // \@param argc
    // \@param argv 
    // \@return A ParsedArgs struct containing the movie type and help status
    ParsedArgs parse(int argc, char* argv[]);

    // Returns a string detailing the command-line usage of the application
    // \@param programName The name of the executable 
    // \@return A string containing the help/usage message
    std::string getUsageString(const std::string& programName) const; 

    private:
    // Set of allowed movie types for validation
    std::set<std::string> allowedTypes_;
    // Set of allowed fields for sorting
    std::set<std::string> allowedSortFields_;
    // Set of allowed orders for sorting
    std::set<std::string> allowedSortOrders_;
};

#endif // CLI_PARSER_H