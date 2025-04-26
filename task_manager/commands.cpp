#include "commands.h"
#include "utils.h" // For generateNextId and getCurrentTimestamp
#include "task.h" // For Task struct and statusToString 
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::find_if, std::remove_if
#include <chrono> // For time points

/**
 * \@brief Adds a new task to the task list 
 * \@param tasks The vector of tasks (will be modified)
 * \@param definition The description for the new task
 */
void addTask(std::vector<Task>& tasks, const std::string& description) {
    int newId = generateNextId(tasks); // Get the next available ID
    auto now = getCurrentTimestamp(); // Get the current time 
    tasks.emplace_back(newId, description, TaskStatus::TODO, now, now); // Add the new task
    std::cout << "Task " << newId << " added: \"" << description << "\"" << std::endl;
}

/**
 * \@brief Finds a task by ID using std::find_if 
 * \@param tasks The vector of tasks to search within 
 * \@param id The ID of the task to find 
 * \@return An iterator to the found task, or tasks.end() if not found 
 */
std::vector<Task>::iterator findTaskById(std::vector<Task>& tasks, int id) {
    return std::find_if(tasks.begin(), tasks.end(),
                        [id](const Task& task) { return task.id == id; });
}

/**
 * \@brief Updates the description of an existing task
 * \@param tasks The vector of tasks (will be modified)
 * \@param id The ID of the task to update 
 * \@param newDescription The new description for the task
 * \@return True if the task was found and updated, false otherwise
 */
bool updateTask(std::vector<Task>& tasks, int id, const std::string& newDescription) {
    auto it = findTaskById(tasks, id); 
    if (it != tasks.end()) {
        it->description = newDescription;
        it->updatedAt = getCurrentTimestamp(); // Update the timestamp 
        std::cout << "Task " << id << " updated." << std::endl;
        return true;
    } else {
        std::cerr << "Error: Task with ID " << id << " not found." << std::endl;
        return false;
    }
}

/**
 * \@brief Deletes a task from the list by its ID
 * \@param tasks The vector of tasks (will be modified)
 * \@param id The ID of the task to delete 
 * \@return True if the task was found and deleted, false otherwise 
 */
bool deleteTask(std::vector<Task>& tasks, int id) {
    auto original_size = tasks.size(); 
    //remove_if moves the elements to be removed to the end and returns an iterator to the start of the removed range
    auto new_end = std::remove_if(tasks.begin(), tasks.end(),
                                  [id](const Task& task) { return task.id == id; });

    if (new_end != tasks.end()) {
        tasks.erase(new_end, tasks.end()); // Actually erase the elements
        std::cout << "Task " << id << " deleted." << std::endl;
        return true;
    } else {
        std::cerr << "Error: Task with ID " << id << " not found." << std::endl;
        return false;
    }
}

/**
 * \@brief Marks the status of an existing task
 * \@param tasks The vector of tasks (will be modified)
 * \@param id The ID of the task to mark
 * \@param status The new status for the task
 * \@return True if the task was found and its status updated, false otherwise 
 */
bool markTaskStatus(std::vector<Task>& tasks, int id, TaskStatus status) { 
    auto it = findTaskById(tasks, id); 
    if (it != tasks.end()) {
        it->status = status;
        it->updatedAt = getCurrentTimestamp(); // Update the timestamp 
        std::cout << "Task " << id << " marked as " << statusToString(status) << "." << std::endl;
        return true;
    } else {
        std::cerr << "Error: Task with ID " << id << " not found." << std::endl;
        return false;
    }
}

/**
 * \@brief Lists tasks, optionally filtering by status
 * \@param tasks The vector of tasks to list 
 * \@param filterStatus The status to filter by ("todo", "in progress", "done", or empty string for all)
 */
void listTasks(const std::vector<Task>& tasks, const std::string& filterStatus = "") {
    std::cout << "\n--- Task List ---" << std::endl;
    bool tasksDisplayed = false; 
    TaskStatus filterEnum = TaskStatus::TODO; // Default, only used if filterStatus is not empty 
    bool applyFilter = !filterStatus.empty(); 

    if (applyFilter) {
        filterEnum = stringToStatus(filterStatus); // Convert filter string to enum 
    }

    for (const auto& task : tasks) {
        // Apply filter if specified 
        if (applyFilter && task.status != filterEnum) {
            continue; // Skip this task if it doesn't match the filter
        }

        // Print task details 
        std::cout << "ID: " << task.id 
                  << " | Status: " << statusToString(task.status) 
                  << " | Created: " << formatTimestamp(task.createdAt) 
                  << " | Updated: " << formatTimestamp(task.updatedAt) << std::endl;
        std::cout << "Description: " << task.description << std::endl; 
        std::cout << "------------------------" << std::endl;
        tasksDisplayed = true; 
    }

    if (!tasksDisplayed) {
        if (applyFilter) {
            std::cout << "No tasks found with status: " << filterStatus << std::endl;
        } else {
            std::cout << "No tasks in the list." << std::endl;
        }
    }
    std::cout << "Total tasks: " << tasks.size() << std::endl;
    std::cout << "------------------------" << std::endl;
}