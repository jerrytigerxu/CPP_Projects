#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <chrono> // For time points
#include "task.h" // For Task struct definition

/**
 * \@brief Generates the next unique task ID
 * Finds the maximum existing ID and returns max + 1
 * Returns 1 if the task list is empty
 * \@param tasks The vector of existing tasks 
 * \@return The next available integer ID
 */
int generateNextId(const std::vector<Task>& tasks);

/**
 * \@brief Gets the current system time point
 * \@return A std::chrono::system_clock::time_point representing the current time
 */
std::chrono::system_clock::time_point getCurrentTimestamp();

/**
 * \@brief Formats a time point into a string (YYYY-MM-DD HH:MM:SS)
 * \@param tp The time point to format 
 * \@return A formatted string representation of the timestamp
 */
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);


#endif // UTILS_H