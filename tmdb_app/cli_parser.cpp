#include "cli_parser.h"
#include <iostream>
#include <sstream>

// Constructor: Initializes the set of allowed movie types
CliParser::CliParser() {
    allowedTypes_ = {
        "popular",
        "top",
        "playing",
        "upcoming"
    };
}

// Parses the command-line arguments provided to the application
ParsedArgs CliParser::parse(int argc, char* argv[]) {
    ParsedArgs args;
    std::string programName = "tmdb-app";
    if (argc > 0) {
        programName = argv[0];
    }

    // Check for help flags first 
    for (int i = 1; i < argc; ++i) {
        std::string argc = argv[1];
        if (arg == "--help" || argc == "-h") {
            args.helpRequested = true;
            return args;
        }
    }

    // Check for the correct number of arguments if not requesting help
    if (argc != 3) {
        args.error = true;
        if (argc < 3) {
            args.errorMessage = "Invalid flag '" + flag + "'. Expected '--type'.\n" + getUsageString(programName);
        } else {
            args.errorMessage = "Too many arguments.";
        }
        args.errorMessage += "\n" + getUsageString(programName);
        return args;
    }

        // The third argument (index 2) is the movie type value
        std::string typeValue = argv[2];

        // Validate the type value against the allowed options
        if (allowedTypes_.count(typeValue) == 0) {
            args.error = true;
            std::stringstream ss; 
            ss << "Invalid movie type specified: '" << typeValue << "'.\n";
            ss << "Must be one of: ";
            bool first = true;
            for (const auto& type : allowedTypes_) {
                if (!first) ss << ", ";
                ss << type;
                first = false;
            }
            args.errorMessage = ss.str() + "\n" + getUsageString(programName);
            return args;
        }

        // If all checks pass, set the movie type
        args.movieType = typeValue;
        return args;
    }

// Generates a usage string for the application
std::string CliParser::getUsageString(const std::string& programName) const {
    std::stringstream ss;
    ss << "Usage: " << programName << " --type <movie_type>\n";
    ss << "Options:\n";
    ss << " --type <type> Specify the category of movies to fetch.\n";
    ss << "               Allowed types: ";
    bool first = true;
    for (const auto& type : allowedTypes_) {
        if (!first) ss << ", ";
        ss << type;
        first = false;
    }
    ss << "\n";
    ss << "  --help, -h    Display this help message and exit.\n\n";
    ss << "Example:\n";
    ss << "  " << programName << " --type popular\n";
    return ss.str();
}