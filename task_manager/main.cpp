#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "task.h"
#include "commands.h" // Task manipulation functions (add, update, delete, list, mark) 
#include "storage.h" // For loadTasks and saveTasks (stubs for now)

// Helper function to print usage instructions
void printUsage(const char* progName) {
    std::cerr << " " << std::endl;
    std::cerr << "Usage: " << progName << " <command> [options]" << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << " add \"<description>\"" << std::endl;
    std::cerr << " update <id> \"<new_description>\"" << std::endl;
    std::cerr << " delete <id>" << std::endl;
    std::cerr << " mark-in-progress <id>" << std::endl; 
    std::cerr << " mark-done <id>" << std::endl; 
    std::cerr << " list [todo|in-progress|done]" << std::endl;
}

int main(int argc, char* argv[]) {

    // --- Load existing tasks ---
    std::vector<Task> tasks = loadTasks(); // Calls the load function from storage.cpp

    // --- Argument Count Check ---
    // Need at least the program name and a command
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    // --- Command Extraction ---
    std::string command = argv[1]; 
    bool tasksModified = false; // Flag to track if saveTasks should be called 

    try {
        // --- Command Handling --- 
        if (command == "add") {
            if (argc != 3) {
                std::cerr << "Error: 'add' command requires exactly one argument: \"<description>\"" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
            std::string description = argv[2];
            addTask(tasks, description);
            tasksModified = true;
        } else if (command == "update") {
            if (argc != 4) {
                std::cerr << "Error: 'update' command requires two arguments: <id> \"<new_description>\"" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
            // Convert ID argument from string to integer
            int id = std::stoi(argv[2]);
            std::string newDescription = argv[3];
            if (updateTask(tasks, id, newDescription)) {
                tasksModified = true;
            }
        } else if (command == "delete") {
            if (argc != 3) {
                std::cerr << "Error: 'delete' command requires exactly one argument: <id>" << std::endl; 
                printUsage(argv[0]);
                return 1;
            }
            int id = std::stoi(argv[2]);
            if (deleteTask(tasks, id)) {
                tasksModified = true;
            }
        } else if (command == "mark-in-progress") {
            if (argc != 3) {
                std::cerr << "Error: 'mark-in-progress' command requires exactly one argument: <id>" << std::endl; 
                printUsage(argv[0]);
                return 1; 
            }
            int id = std::stoi(argv[2]);
            if (markTaskStatus(tasks, id, TaskStatus::IN_PROGRESS)) {
                tasksModified = true;
            }
        } else if (command == "mark-done") {
            if (argc != 3) {
                std::cerr << "Error: 'mark-done' command requires exactly one argument: <id>" << std::endl;
                printUsage(argv[0]);
                return 1; 
            }
            int id = std::stoi(argv[2]);
            if (markTaskStatus(tasks, id, TaskStatus::DONE)) {
                tasksModified = true;
            }
        } else if (command == "list") {
            std::string filterStatus = ""; // Default to list all 
            if (argc == 3) {
                filterStatus = argv[2];
                if (filterStatus != "todo" && filterStatus != "in-progress" && filterStatus != "done") {
                    std::cerr << "Error: Invalid status filter. Use 'todo', 'in-progress', or 'done'." << std::endl;
                    printUsage(argv[0]);
                    return 1;
                }
            } else if (argc > 3) {
                std::cerr << "Error: 'list' command takes at most one optional argument: [todo|in-progress|done]" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
            listTasks(tasks, filterStatus);
        } else {
            std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid task ID provided. ID must be a number." << std::endl;
        return 1;
    } catch (const std::out_of_range&e) {
        std::cerr << "Error: Task ID provided is too large." << std::endl;
        return 1; 
    } catch (const std::exception& e) {
        std::cerr << "Error: An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    // --- Save tasks if modified ---
    if (tasksModified) {
        saveTasks(tasks); // Calls the save function from storage.cpp
    }

    return 0; // Indicate success
}