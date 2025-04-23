#include <iostream> // For input/output streams
#include <random>   // For random number generation
#include <chrono>   // For seeding the random number generator
#include <string>   // For using std::string  
#include <limits>   // For std::numeric_limits (used in input validation)
#include <ios>      // For std::streamsize (used in input validation)
#include <cctype>   // For std::isspace (used in input validation)

// Structure to hold the game settings based on difficulty
struct GameSettings {
    int minRange = 1;
    int maxRange = 50;
    int maxTries = 10;
    std::string difficultyName = "Easy";
};

// --- Function Declarations ---

// Prints usage instructions
void printUsage(const char* progName);

// Parses command-line arguments to determine game settings
// Returns: 0 on success, 1 on error, 2 if help was requested
int parseArguments(int argc, char** argv, GameSettings& settings);

// Generates the secret random number
int generateSecretNumber(int minRange, int maxRange);

// Handles getting a valid integer guess from the user
// Returns: true if a valid guess was entered, false otherwise
// Modifies: guess by reference
bool getValidGuess(int minRange, int maxRange, int& guess);

// Contains the main game loop logic
void playGame(const GameSettings& settings, int secretNumber);

// --- Main Function ---

int main(int argc, char** argv)
{  
    GameSettings settings; // Use default settings (Easy) initially

    // Parse command-line arguments
    int parseResult = parseArguments(argc, argv, settings);

    if (parseResult == 1) {
        return 1;
    }
    if (parseResult == 2) {
        return 0;
    }

    // -- Random Number Generation --
    int secretNumber = generateSecretNumber(settings.minRange, settings.maxRange);

    // -- Start Game --
    std::cout << "--- Number Guessing Game ---\n";
    std::cout << "Difficulty: " << settings.difficultyName << " Mode\n";
    std::cout << "I'm thinking of a number between " << settings.minRange << " and " << settings.maxRange << ".\n";

    playGame(settings, secretNumber);
    
    std::cout << "---Game Over ---\n";
    return 0;
}


// --- Function Definitions --

/**
 * \@brief Prints usage instructions for the program
 * \@param progName The name of the executable (argv[0])
 */
void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [difficulty_flag]\n";
    std::cout << "Guess the secret number.\n\n";
    std::cout << "Difficulty Flags:\n";
    std::cout << "  -e, --easy   Range 1-50,   10 tries (Default)\n";
    std::cout << "  -m, --medium Range 1-100,   7 tries\n";
    std::cout << "  -h, --hard   Range 1-200,   5 tries\n";
    std::cout << "  -?  --help   Show this help message\n";
}

/**
 * \@brief Parses command-line arguments to set game difficulty
 * \@param argc Argument count
 * \@param argv Argument values
 * \@param settings Reference to the GameSettings struct to be populated
 * \@return 0 if settings were determined successfully
 * \@return 1 if an error occurred
 * \@return 2 if the help flag was provided (and help message was printed)
 */
int parseArguments(int argc, char** argv, GameSettings& settings) {
    // Default settings are already set when 'settings' is created
        
    if (argc > 2) {
        std::cerr << "Error: Too many arguments.\n";
        printUsage(argv[0]);
        return 1;
    }

    if (argc == 2) {
        std::string arg = argv[1];

        if (arg == "-e" || arg == "--easy") {
            settings.minRange = 1;
            settings.maxRange = 50;
            settings.maxTries = 10;
            settings.difficultyName = "Easy";
        } else if (arg == "-m" || arg == "--medium") {
            settings.minRange = 1;
            settings.maxRange = 100;
            settings.maxTries = 7;
            settings.difficultyName = "Medium";
        } else if (arg == "-h" || arg == "--hard") {
            settings.minRange = 1;
            settings.maxRange = 200;
            settings.maxTries = 5;
            settings.difficultyName = "Hard";
        } else if (arg == "-?" || arg == "--help") {
            printUsage(argv[0]);
            return 2;
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // if argc == 1, the defaults (Easy) are already set
    return 0;
}

/**
 * \@brief Generates a secret random number within the specified range
 * \@param minRange The minimum possible value (inclusive)
 * \@param maxRange The maximum possible value
 * \@return The generated secret number
 */
int generateSecretNumber(int minRange, int maxRange) {
    // Seed the random number generator
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // Mersenne Twister engine

    // Define the distribution for the desired range (inclusive)
    std::uniform_int_distribution<int> distribution(minRange, maxRange);

    // Generate the secret number
    return distribution(generator);

}

/**
 * \@brief Gets a single, validated integer guess from the user
 * Handles non-numeric input, extra characters, and range checking
 * \@param minRange The minimum valid guess
 * \@param maxRange The maximum valid guess
 * \@param guess Reference to an integer where the valid guess will be stored 
 * \@return true if a valid guess within the range was successfully read, false otherwise
 */
bool getValidGuess(int minRange, int maxRange, int& guess) {
    while (true) {
        std::cin >> guess; 

        bool validInput = true;

        // Check 1: Did the read operation fail?
        if (std::cin.fail()) {
            validInput = false;
        } else {
            // Check 2: Is there anything left on the line other than whitespace?
            char remainingChar; 
            while (std::cin.get(remainingChar)) {
                if (remainingChar == '\n') {
                    break;
                } else if (!std::isspace(remainingChar)) {
                    validInput = false; 
                    break;
                }
            }
        }

        if (!validInput) {
            std::cout << "Invalid input. Please enter a whole number only.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue; 
        }

        // Check 3: Is the (now validated) number within the allowed range?
        if (guess < minRange || guess > maxRange) {
            std::cout << "Make sure your guess is within the proper range (" << minRange << "-" << maxRange << ").\n";
            continue;
        }
        return true;
    }
    return false;
}

/**
 * \@brief Handles the main game loop, including input, validation, and feedback
 * \@param settings The current game settings
 * \@param secretNumber The number the user needs to guess
 */
void playGame(const GameSettings& settings, int secretNumber) {
    int triesLeft = settings.maxTries;
    int userGuess = 0;
    bool guessedCorrectly = false; 

    while (triesLeft > 0) {
        std::cout <<"\n(" << triesLeft << " tries left) Enter your guess: ";

        // Get validated input using the helper function 
        if (!getValidGuess(settings.minRange, settings.maxRange, userGuess)) {
            std::cerr << "Error reading guess. Exiting." << std::endl;
            return;
        }

        // --- Guess Comparison Logic ---
        if (userGuess == secretNumber) {
            std::cout << "Great job, you guessed correctly with " << triesLeft << " guesses left to spare! The secret number is: " << secretNumber << "\n";
            guessedCorrectly = true;
            break;
        } else if (userGuess < secretNumber) {
            std::cout << "Your guess is too low.\n";
            triesLeft--;
        } else {
            std::cout << "Your guess is too high.\n";
            triesLeft--;
        } 
        // --- End Guess Comparison Logic ---

        if (triesLeft > 0) {
            std::cout << " Try again.\n";
        } else {
            std::cout << "\n";
        }
    }

    // --- Win/Loss Message ---
    if (!guessedCorrectly) {
        std::cout << "\nSorry, you ran out of tries! The secret number was: " << secretNumber << ".\n";
    }
}