#include "utils.h"
#include <vector>
#include <algorithm> // For std::max_element
#include <chrono> 
#include <ctime> // For std::time_t, std::localtime, std::strftime 
#include <iomanip> // For std::put_time (alternative formmating)
#include <sstream> // For string stream formatting


/**
 * \@brief Generates the next unique task ID
 * Finds the maximum existing ID and returns max + 1
 * Returns 1 if the task list is empty
 * \@param tasks The vector of existing tasks 
 * \@return The next available integer ID
 */
int generateNextId(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        return 1; // Start IDs from 1 if the list is empty
    }
    // Find the task with the maximum ID 
    auto maxIt = std::max_element(tasks.begin(), tasks.end(), 
                                  [](const Task& a, const Task& b) {
                                        return a.id < b.id;
                                  });
    return maxIt->id + 1; // Return the next ID
}

/**
 * \@brief Gets the current system time point
 * \@return A std::chrono::system_clock::time_point representing the current time
 */
std::chrono::system_clock::time_point getCurrentTimestamp() {
    return std::chrono::system_clock::now();
}

/**
 * \@brief Formats a time point into a string (YYYY-MM-DD HH:MM:SS)
 * \@param tp The time point to format 
 * \@return A formatted string representation of the timestamp
 */
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
    // Convert time_point to time_t 
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    // Convert time_t to tm (local time)
    std::tm local_tm = *std::localtime(&time);

    // Use stringstream and put_time for safer formatting
    std::stringstream ss; 
    // Format: YYYY-MM-DD HH:MM:SS
    ss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();

}
