#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <vector>

// Helper function that explains how to use the program, especially when handling '--help'
void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [difficulty_flag]\n";
    std::cout << "Guess the secret number.\n\n";
    std::cout << "Difficulty Flags:\n";
    std::cout << "  -e, --easy   Range 1-50,   10 tries (Default)\n";
    std::cout << "  -m, --medium Range 1-100,   7 tries\n";
    std::cout << "  -h, --hard   Range 1-200,   5 tries\n";
    std::cout << "  -?  --help   Show this help message\n";
}

int main(int argc, char** argv)
{  
    // -- Argument Parsing --
    int minRange = 1;  // Default min
    int maxRange = 50; // Default max (Easy)
    int maxTries = 10; // Default tries (Easy)
    bool showHelp = false; 
    
    if (argc > 2) {
        std::cerr << "Error: Too many arguments.\n";
        printUsage(argv[0]);
        return 1;
    }

    if (argc == 2) {
        std::string arg = argv[1];

        if (arg == "-e" || arg == "--easy") {
            minRange = 1;
            maxRange = 50;
            maxTries = 10;
        } else if (arg == "-m" || arg == "--medium") {
            minRange = 1;
            maxRange = 100;
            maxTries = 7;
        } else if (arg == "-h" || arg == "--hard") {
            minRange = 1;
            maxRange = 200;
            maxTries = 5;
        } else if (arg == "-?" || arg == "--help") {
            showHelp = true;
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // if argc == 1, the defaults (Easy) are already set

    if (showHelp) {
        printUsage(argv[0]);
        return 0;
    }

    // -- End Argument Parsing --

    int triesLeft = maxTries;
    int userGuess;

    // -- Random Number Generator -- 
    // Seed the random number generator
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    // Define the distribution for the desired range (inclusive)
    std::uniform_int_distribution<int> distribution(minRange, maxRange);
    // Generate the secret number
    int secretNumber = distribution(generator);
    // -- End Random Number Generator -- 

    std::cout << "Difficulty: ";
    if (maxTries == 10) std::cout << "Easy";
    else if (maxTries == 7) std::cout << "Medium";
    else std::cout << "Hard";
    std::cout << " Mode\n";

    std::cout << "I'm thinking of a number between " << minRange << " and " << maxRange << ".\n";
    std::cout << "Try to guess the number!\n";

    // -- Game Loop -- 
    while (triesLeft > 0) {
        std::cout << "Your guess: ";
        std::cin >> userGuess;

        if (userGuess < minRange || userGuess > maxRange) {
            std::cout << "Make sure your guess is within the proper range (" << minRange << "-" << maxRange << ").\n";
            continue;
        }
        
        if (userGuess == secretNumber) {
            std::cout << "Great job, you guessed correctly! The secret number is: " << secretNumber << "\n";
            break;
        } else if (userGuess < secretNumber) {
            std::cout << "Your guess is too low. Try again.\n";
            triesLeft--;
        } else {
            std::cout << "Your guess is too high. Try again.\n";
            triesLeft--;
        } 

        std::cout << "(You have " << triesLeft << " tries remaining)\n\n";

    } 
    // -- End Game Loop --

    // -- Win/Loss Message --
    if (triesLeft == 0) {
        std::cout << "Sorry, you ran out of tries! The secret number was: " << secretNumber << ".\n";
    }
    // -- End Win/Loss Message --


    

    return 0;
}