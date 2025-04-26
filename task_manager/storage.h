#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <string> 
#include "task.h"

// Function to load tasks from the JSON file 
// Returns a vector of tasks
std::vector<Task> loadTasks();

// Function to save tasks to the JSON file 
// Takes a constant reference to a vector of tasks
void saveTasks(const std::vector<Task>& tasks);


#endif // STORAGE_H