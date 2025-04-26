#include "storage.h"
#include "task.h" // For Task struct, statusToString, stringToStatus
#include "utils.h" // For formatTimestamp, getCurrentTimestamp 
#include <iostream>
#include <vector>
#include <fstream> // For file streams (ofstream, ifstream)
#include <sstream> // For string streams (used in parsing and formatting)
#include <string>
#include <stdexcept> // For exception handling during parsing if needed 
#include <iomanip> // For std::get_time, std::put_time 
#include <algorithm> // For std::remove_if, std::find_if
#include <cctype> // For ::isspace

// --- Helper Functions for JSON Handling ---

/**
 * \@brief Escapes special JSON characters in a string
 * Currently handles double qutoes and backslashes 
 * \@param input The string to escape 
 * \@return The escaped string suitable for embedding in a JSON string value
 */
std::string escapeJsonString(const std::string& input) {
    std::ostringstream ss; 
    for (char c : input) {
        switch (c) {
            case '"': ss << "\\\""; break; // Escape double quote 
            case '\\': ss << "\\\\"; break; // Escape backslash
            case '\b': ss << "\\b"; break; // Backspace 
            case '\f': ss << "\\f"; break; // Form feed 
            case '\n': ss << "\\n"; break; // Newline 
            case '\r': ss << "\\r"; break; // Carriage return 
            case '\t': ss << "\\t"; break; // Tab
            default: ss << c; break;
        }
    }
    return ss.str(); 
}

/**
 * \@brief Parses a timestamp string (expected format: YYYY-MM-DD HH:MM:SS) 
 * into a std::chrono::system_clock::time_point 
 * \@param timestampStr The string representation of the timestamp
 * \@return The corresponding time_point. Returns epoch on parsing failure.
 */
std::chrono::system_clock::time_point parseTimestamp(const std::string& timestampStr) {
    std::tm tm = {}; // Initialize tm struct to zeros 
    std::stringstream ss(timestampStr);

    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        std::cerr << "Warning: Failed to parse timestamp string: " << timestampStr << std::endl;
        return std::chrono::system_clock::from_time_t(0); // Return epoch on failure
    }

    std::time_t time = std::mktime(&tm);
    if (time == -1) {
        std::cerr << "Warning: Failed to convert parsed time components for: " << timestampStr << std::endl;
        return std::chrono::system_clock::from_time_t(0); // Return epoch on failure 
    }
    return std::chrono::system_clock::from_time_t(time);
}

/**
 * \@brief Trims leading/trailing whitespace and double quotes from a string
 * \@param s The string to trim
 * \@return The trimmed string
 */
std::string trimQuotesAndWhitespace(const std::string& s) {
    // Find the first non-whitespace character
    size_t first = s.find_first_not_of(" \t\n\r\"");
    if (std::string::npos == first) {
        return "";
    }
    // Find the last non-whitespace/non-quote character
    size_t last = s.find_last_not_of(" \t\n\r\"");
    return s.substr(first, (last - first + 1));
}

/**
 * \@brief Parses a string containing a simplified JSON object into a Task struct
 * This is a basic paraser assuming a specific field order and simple structure
 * It does NOT handled nested structures, arrays within the object, or complex escaping robustly
 * \@param objStr The string segment representing the JSON object 
 * \@param task Output parameter: The task struct to populate
 * \@return True if parsing was successful, false otherwise
 */
bool parseTaskObject(const std::string& objStr, Task& task) {
    std::stringstream ss(objStr);
    std::string key;
    std::string valueStr;
    int numeric_id_value = -1; 
    char ch;

    // Basic validation
    if (objStr.empty() || objStr.front() != '{' || objStr.back() != '}') {
        std::cerr << "Error: Invalid task object string format (missing braces): " << objStr << std::endl;
        return false;
    }

    // Consume '{' and initial whitespace
    ss >> ch >> std::ws;
    if (ch != '{') {
         std::cerr << "Error: Expected '{' at the beginning of task object string." << std::endl;
         return false;
    }


    while (ss.peek() != EOF && ss.peek() != '}') {
        // 1. Extract the key (must be quoted)
        ss >> std::quoted(key);
        if (ss.fail()) {
            std::cerr << "Error: Failed to read quoted key in task object: " << objStr << std::endl;
            return false;
        }

        // 2. Consume the colon ':'
        ss >> std::ws >> ch >> std::ws; // Skip whitespace around colon
        if (ch != ':') {
            std::cerr << "Error: Expected ':' after key '" << key << "' in task object: " << objStr << std::endl;
            return false;
        }

        bool value_is_numeric = false; 

        // 3. Extract the value
        if (ss.peek() == '"') { // Value is a quoted string
            ss >> std::quoted(valueStr);
            if (ss.fail()) {
                std::cerr << "Error: Failed to read quoted value for key '" << key << "' in task object: " << objStr << std::endl;
                return false;
            }
        } else { // Assume value is a number (only for ID in this structure)
             if (!(ss >> numeric_id_value)) {
                std::cerr << "Error: Failed to read numeric value for key '" << key << "' in task object: " << objStr << std::endl;
                return false; 
             }
             value_is_numeric = true; 
            // Optional: Add more validation here to ensure valueStr is purely numeric if needed
        }

        // 4. Assign value to the correct Task member
        try {
            if (key == "id") {
                if (!value_is_numeric) {
                    std::cerr << "Error: Expected numeric value for key 'id', but found: " << valueStr << std::endl;
                    return false;
                }
                task.id = numeric_id_value; 
            } else if (key == "description") {
                task.description = valueStr; // Already unescaped by std::quoted
            } else if (key == "status") {
                task.status = stringToStatus(valueStr); // Already unescaped by std::quoted
            } else if (key == "createdAt") {
                task.createdAt = parseTimestamp(valueStr); // Already unescaped by std::quoted
            } else if (key == "updatedAt") {
                task.updatedAt = parseTimestamp(valueStr); // Already unescaped by std::quoted
            } else {
                std::cerr << "Warning: Unknown key '" << key << "' in task object." << std::endl;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid numeric format for key '" << key << "': " << valueStr << std::endl;
            return false;
        } catch (const std::out_of_range& e) {
            std::cerr << "Error: Numeric value out of range for key '" << key << "': " << valueStr << std::endl;
            return false;
        }

        // 5. Check for comma or closing brace
        ss >> std::ws; // Skip whitespace after value
        ch = ss.peek();

        if (ch == ',') {
            ss.ignore(1); // Consume comma
            ss >> std::ws; // Consume whitespace leading to the next key
        } else if (ch == '}') {
            // End of object is next, let the loop condition handle it
        } else if (ch == EOF) {
             std::cerr << "Error: Unexpected end of input after value for key '" << key << "'." << std::endl;
             return false;
        } else {
            std::cerr << "Error: Expected ',' or '}' after value for key '" << key << "', but found '" << (char)ch << "' in task object: " << objStr << std::endl;
            return false;
        }
    } // End while loop

    // Consume the final '}'
    if (ss.peek() == '}') {
        ss.ignore(1);
    } else if (ss.peek() != EOF) {
         // If it's not EOF and not '}', something is wrong after the last element
         std::cerr << "Error: Expected '}' at the end of task object, but found '" << (char)ss.peek() << "'." << std::endl;
         return false;
    }


    return true; // Parsing successful
}


// --- Public Interface Functions --- 

/**
 * \@brief Loads tasks from the specified JSON file ("tasks.json")
 * Handles file not existing, empty file, and basic JSON array structure 
 * Uses parseTaskObject to individual task parsing
 * \@return A vector containing the loaded tasks. Returns empty vector on error or if file is empty
 */
std::vector<Task> loadTasks() {
    const std::string filename = "tasks.json"; 
    std::vector<Task> tasks; 
    std::ifstream inputFile(filename);

    // Check if the file stream was opened successfully
    if (!inputFile.is_open()) {
        return tasks; // Return empty vector if file doesn't exist or can't be opened
    }

    // Read the entire file content into a stringstream buffer
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    inputFile.close();
    std::string content = buffer.str();

    // --- Basic JSON array validation ---
    // Trim leading/trailing whitespace from the whole content 
    content.erase(0, content.find_first_not_of(" \t\n\r"));
    content.erase(content.find_last_not_of(" \t\n\r") + 1);

    // Check if content is empty or doesn't look like a JSON array 
    if (content.length() <= 1 || content.front() != '[' || content.back() != ']') {
        if (!content.empty()) {
            std::cerr << "Warning: '" << filename << "' is malformed or empty. Starting with empty task list." << std::endl;
        }
        // Otherwise, return empty vector
        return tasks; 
    }

    // --- Parse task objects within the array ---
    size_t startPos = 1; 
    size_t objEndPos = 0;
    int braceLevel = 0;

    while (startPos < content.length() - 1) {
        // Find the start of the next object '{'
        size_t objStartPos = content.find('{', startPos);
        if (objStartPos == std::string::npos) {
            break; // No more objects found 
        }

        // Find the corresponding closing brace '}' for this object 
        braceLevel = 0; 
        objEndPos = objStartPos; 
        while (objEndPos < content.length() - 1) {
            if (content[objEndPos] == '{') {
                braceLevel++;
            } else if (content[objEndPos] == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    break; // Found the matching closing brace
                }
            }
            objEndPos++;
        }

        if (braceLevel != 0) {
            std::cerr << "Warning: Malformed JSON structure in '" << filename << "'. Mismatched braces." << std::endl; 
            tasks.clear();
            return tasks; 
        }

        // Extract the object string 
        std::string taskStr = content.substr(objStartPos, objEndPos - objStartPos + 1); 

        Task task;
        if (parseTaskObject(taskStr, task)) {
            tasks.push_back(task); // Add successfully parsed task 
        } else {
            std::cerr << "Warning: Skipping malformed task object in '" << filename << "'." << std::endl;
        }
        startPos = objEndPos + 1; 
    } // End while loop for finding objects

    // --- Final Output ---
    // Only print the "Loaded..." message if tasks were actually parsed
    if (!tasks.empty()) {
        std::cout << "Loaded " << tasks.size() << " task(s) from " << filename << "." << std::endl;
    }
    return tasks; 

}

/**
 * \@brief Saves the provided vector of tasks to the specified JSON file ("tasks.json")
 * Overwrites the file if it exists. Creates it if it doesn't
 * Formats the output as a JSON array of task objects 
 * \@param tasks The vector of tasks to save
 */
void saveTasks(const std::vector<Task>& tasks) {
    const std::string filename = "tasks.json"; 
    // Open the file for writing 
    std::ofstream outputFile(filename);

    // Check if the file stream was opened successfully 
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open '" << filename << "' for writing." << std::endl;
        return; // Exit if file can't be opened 
    }

    // Write the opening bracket for the JSON array 
    outputFile << "[" << std::endl;

    // Iterate through the tasks and write each one as a JSON object
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i]; 

        // Start of the JSON object for the task
        outputFile << " {" << std::endl;

        // Write key-value pairs for each field
        outputFile << "   \"id\": " << task.id << "," << std::endl;
        outputFile << "   \"description\": \"" << escapeJsonString(task.description) << "\"," << std::endl; 
        outputFile << "   \"status\": \"" << statusToString(task.status) << "\"," << std::endl;
        outputFile << "   \"createdAt\": \"" << formatTimestamp(task.createdAt) << "\"," << std::endl;
        outputFile << "   \"updatedAt\": \"" << formatTimestamp(task.updatedAt) << "\"" << std::endl;

        // End of the JSON object for the task 
        outputFile << " }";

        // Add a comma after the object if it's not the last task in the vector
        if (i < tasks.size() - 1) {
            outputFile << ",";
        }
        outputFile << std::endl;
    }

    // Write the closing bracket for the JSON array 
    outputFile << "]" << std::endl;

    // The ofstream destructor will automatically close the file when outputFile goes out of scope
    std::cout << "Saved " << tasks.size() << " task(s) to " << filename << "." << std::endl;
}