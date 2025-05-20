#include "cli_parser.h"
#include <iostream> // For potential debug/error output, though errors are returned in ParsedArgs
#include <sstream>  // For constructing error messages and usage string
#include <vector>
#include <algorithm>

// Constructor: Initializes the set of allowed movie types.
CliParser::CliParser() {
    allowedTypes_ = {
        "popular",
        "top",       // Corresponds to "top_rated" in API handler
        "playing",   // Corresponds to "now_playing" in API handler
        "upcoming"
    };
    allowedSortFields_ = {
        "title",
        "date",
        "rating"
    };
    allowedSortOrders_ = {
        "asc",
        "desc"
    };
}

// Parses the command-line arguments provided to the application.
ParsedArgs CliParser::parse(int argc, char* argv[]) {
    ParsedArgs args;
    std::string programName = "tmdb-app"; // Default program name
    if (argc > 0 && argv[0] != nullptr) {
        programName = argv[0];
    }

    std::vector<std::string> tokens;
    for (int i=1;i<argc;++i) {
        tokens.push_back(argv[i]);
    }

    bool typeFlagFound = false;
    bool sortByFlagFound = false;
    bool orderFlagFound = false;

    for (size_t i=0;i<tokens.size();++i) { // Read the arguments
        const std::string& argc = tokens[i];

        if (argc == "--help" || argc == "-h") {
            args.helpRequested = true;
            return args;
        } else if (argc == "--type") {
            if (typeFlagFound) {
                args.error = true;
                args.errorMessage = "Argument --type specified more than once.\n" + getUsageString(programName);
                return args;
            }
            if (i + 1 < tokens.size()) {
                args.movieType = tokens[++i];
                typeFlagFound = true;
            } else {
                args.error = true;
                args.errorMessage = "Missing value for --type argument.\n" + getUsageString(programName);
                return args;
            } 
        } else if (argc == "--sort-by") {
            if (i + 1 < tokens.size()) {
                args.sortByField = tokens[++i];
                sortByFlagFound = true;
            } else {
                args.error = true;
                args.errorMessage = "Missing value for --sort-by argument.\n" + getUsageString(programName);
                return args;
            }
        } else if (argc == "--order") {
            if (i + 1 < tokens.size()) {
                args.sortOrder = tokens[++i];
                orderFlagFound = true;
            } else {
                args.error = true;
                args.errorMessage = "Missing value for --order argument.\n" + getUsageString(programName);
                return args;
            }
        } else {
            args.error = true;
            args.errorMessage = "Unknown argument: " + argc + "\n" + getUsageString(programName);
            return args;
        }
    }

    // --type is mandatory
    if (!typeFlagFound && !args.helpRequested) {
        args.error = true;
        args.errorMessage = "Mandatory argument --type is missing.\n" + getUsageString(programName);
        return args;
    }

    // Validate movieType
    if (typeFlagFound && allowedTypes_.count(args.movieType) == 0) {
        args.error = true;
        std::stringstream ss;
        ss << "Invalid movie type specified: '" << args.movieType << "'.\n";
        ss << "Allowed types: ";
        bool first = true;
        for (const auto& type : allowedTypes_) {
            if (!first) ss << ", ";
            ss << type;
            first = false;
        }
        args.errorMessage = ss.str() + "\n" + getUsageString(programName);
        return args;
    }

    // Validate sortByField if provided
    if (sortByFlagFound && allowedSortFields_.count(args.sortByField) == 0) {
        args.error = true;
        std::stringstream ss;
        ss << "Invalid sort field specified: '" << args.sortByField << "'.\n";
        ss << "Allowed sort fields: ";
        bool first = true;
        for (const auto& field : allowedSortFields_) {
            if (!first) ss << ", ";
            ss << field;
            first = false;
        }
        args.errorMessage = ss.str() + "\n" + getUsageString(programName);
        return args;
    }

    // Validate sortOrder if sortByField is provided
    if (orderFlagFound && allowedSortOrders_.count(args.sortOrder) == 0) {
        args.error = true;
        std::stringstream ss;
        ss << "Invalid sort order specified: '" << args.sortOrder << "'.\n";
        ss << "Allowed orders: asc, desc.";
        args.errorMessage = ss.str() + "\n" + getUsageString(programName);
        return args;
    }

    // If --order is specified but --sort-by is not
    if (!sortByFlagFound && orderFlagFound ) {
        args.error = true;
        args.errorMessage = "Argument -- order can only be used when --sort-by is also specified.\n" + getUsageString(programName);
        return args;
    }

    return args;
    
}

// Generates a usage string for the application.
std::string CliParser::getUsageString(const std::string& programName) const {
    std::stringstream ss;
    ss << "Usage: " << programName << " --type <movie_type> [options]\n\n";
    ss << "Mandatory Arguments:\n";
    ss << "  --type <type>   Specify the category of movies to fetch.\n";
    ss << "                  Allowed types: ";
    bool first = true;
    for (const auto& type : allowedTypes_) {
        if (!first) ss << ", ";
        ss << type;
        first = false;
    }
    ss << "\n\n";
    ss << "Optional Arguments:\n";
    ss << "  --sort-by <field>    Field to sort the results by.\n";
    ss << "                       Allowed fields: ";
    first = true;
    for (const auto& field : allowedSortFields_) {
        if (!first) ss << ", ";
        ss << field;
        first = false;
    }
    ss << "\n";
    ss << "  --order <asc|desc>   Sort order (default: asc). Requires --sort-by.\n";
    ss << "                       Allowed orders: asc, desc.\n";
    ss << "  --help, -h           Display this help message and exit.\n\n";
    ss << "Examples:\n";
    ss << "  " << programName << " --type popular\n";
    ss << "  " << programName << " --type top --sort-by rating --order desc\n";
    ss << "  " << programName << " --type upcoming --sort-by date\n";
    return ss.str();
}
