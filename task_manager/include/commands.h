#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>
#include "task.h" // Include the Task struct definition

// --- Function prototypes for task commands ---
// These functions operate directly on the provided vector of tasks


/**
 * \@brief Adds a new task to the task list 
 * \@param tasks The vector of tasks (will be modified)
 * \@param definition The description for the new task
 */
void addTask(std::vector<Task>& tasks, const std::string& description);

/**
 * \@brief Updates the description of an existing task
 * \@param tasks The vector of tasks (will be modified)
 * \@param id The ID of the task to update 
 * \@param newDescription The new description for the task
 * \@return True if the task was found and updated, false otherwise
 */
bool updateTask(std::vector<Task>& tasks, int id, const std::string& newDescription);

/**
 * \@brief Deletes a task from the list by its ID
 * \@param tasks The vector of tasks (will be modified)
 * \@param id The ID of the task to delete 
 * \@return True if the task was found and deleted, false otherwise 
 */
bool deleteTask(std::vector<Task>& tasks, int id);

/**
 * \@brief Marks the status of an existing task
 * \@param tasks The vector of tasks (will be modified)
 * \@param id The ID of the task to mark
 * \@param status The new status for the task
 * \@return True if the task was found and its status updated, false otherwise 
 */
bool markTaskStatus(std::vector<Task>& tasks, int id, TaskStatus status);

/**
 * \@brief Lists tasks, optionally filtering by status
 * \@param tasks The vector of tasks to list 
 * \@param filterStatus The status to filter by ("todo", "in progress", "done", or empty string for all)
 */
void listTasks(const std::vector<Task>& tasks, const std::string& filterStatus);

#endif // COMMANDS_H