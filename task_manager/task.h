#ifndef TASK_H
#define TASK_H

#include <string>
#include <chrono>
#include <ctime>

// Enum to represent the possible statuses of a task
enum class TaskStatus {
    TODO, 
    IN_PROGRESS,
    DONE
};

// Function to convert TaskStatus enum to string (useful for saving/displaying)
inline std::string statusToString(TaskStatus status) { 
    switch (status) {
        case TaskStatus:: TODO: return "todo";
        case TaskStatus::IN_PROGRESS: return "in-progress";
        case TaskStatus::DONE: return "done";
        default: return "unknown";
    }
}

// Function to convert string to TaskStatus enum (useful for loading/filtering)
inline TaskStatus stringToStatus(const std::string& statusStr) {
    if (statusStr == "in-progress") {
        return TaskStatus::IN_PROGRESS;
    } else if (statusStr == "done") {
        return TaskStatus::DONE;
    }
    return TaskStatus::TODO;
}

// Structure to represent a single task
struct Task { 
    int id; // Unique identifier
    std::string description; // Task description 
    TaskStatus status; // Current status (todo, in-progress, done)
    std::chrono::system_clock::time_point createdAt; // Creation timestamp
    std::chrono::system_clock::time_point updatedAt; // Last update timestamp

    // Default constructor
    Task() : id(0), status(TaskStatus::TODO) {}

    // Parametrized constructor
    Task(int i, std::string desc, TaskStatus stat, std::chrono::system_clock::time_point created,
        std::chrono::system_clock::time_point updated) 
        : id(i), description(std::move(desc)), status(stat), createdAt(created), updatedAt(updated) {}

};

#endif // TASK_H