#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>
#include <set>

// Structure to hold the result of parsing command-line arguments
struct parsedArgs {
    std::string movieType;
    bool helpRequested;
    bool error;
    std::string errorMessage;

    // Default constructor
    ParsedArgs() : helpRequested(false), error(false) {}
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
};

#endif // CLI_PARSER_H