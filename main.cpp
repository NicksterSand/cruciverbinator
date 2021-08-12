#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Takes a filepath and gets the wordlist from that file
// (newline separated, '#' for comments)
std::vector<std::string> wordlistFromFile(std::string filepath) {
    std::vector<std::string> wordlist;
    std::string tmpString;
    std::ifstream wordFile("D:\\Documents\\Programming\\Cruciverbinator\\" +
                           filepath);
    if (wordFile.is_open()) {
        while (std::getline(wordFile, tmpString)) {
            if (tmpString[0] != '#') wordlist.push_back(tmpString);
        }
    } else {
        std::cout << "Couldn't open file '" << filepath << "'" << std::endl;
    }
    return wordlist;
}

int main() {
    // Grid size
    std::string sizeString;
    int sizeX;
    int sizeY;
    bool invalidGridSize = true;

    // The Grid
    std::vector<std::string> startingGrid;
    std::vector<std::string> grid;
    bool gridOkay = false;

    // Wordlists
    std::vector<std::string> defaultWordlist;
    std::vector<std::vector<std::string>> wordlists;
    std::vector<std::vector<int>> coordinates;  // Across = 0, Down = 1
    bool stillEnteringWordlists = true;

    // List and grid traversal
    std::vector<int> depths;
    int dIndex = 0;
    bool goingUp = false;
    std::vector<std::string> resets;  // Keeps track of previous grid values
    int xPos = 0;
    int yPos = 0;

    // Temporary variables
    std::string tmpString;
    std::vector<int> tmpInts;

    // Get size of grid from the user
    while (invalidGridSize) {
        std::cout << "Enter the size of your grid section (e.g. 4x5): ";
        std::cin >> sizeString;
        std::size_t crossPos = sizeString.find('x');
        if (crossPos != std::string::npos) {
            sizeX = stoi(sizeString.substr(0, crossPos));
            sizeY = stoi(sizeString.substr(crossPos + 1, sizeString.length()));
            if (sizeX > 0 && sizeY > 0 && sizeX < 100 && sizeY < 100) {
                invalidGridSize = false;
            } else {
                std::cout << std::endl
                          << "Invalid size string. All sizes must be numeric "
                             "and between 1 and 99 (inclusive)."
                          << std::endl;
            }
        } else {
            std::cout << std::endl
                      << "Invalid size string. Please use the format '#x#'. "
                         "Note the lowercase 'x'. It is required."
                      << std::endl;
        }
    }

    // Get grid values from the user
    while (!gridOkay) {
        std::cout
            << "Enter grid one row at a time. Use '_' for empty spaces and "
               "'/' for black squares."
            << std::endl;
        for (int i = 0; i < sizeY; i++) {
            std::cin >> tmpString;

            startingGrid.push_back(tmpString);
        }
        std::cout << "------------------" << std::endl;
        for (std::string gridLine : startingGrid) {
            std::cout << gridLine << std::endl;
        }
        std::cout << "------------------" << std::endl
                  << "Is this grid correct? (y/n): ";
        std::cin >> tmpString;
        if (tmpString != "n" && tmpString != "N") {
            gridOkay = true;
        } else {
            startingGrid.clear();
        }
    }
    grid = startingGrid;

    // Get wordlists from the user
    std::cout << "Enter the wordlists for each of the coordinates you would "
                 "like to check. Wordlists from files will automatically be "
                 "reduced to only words that fit the given grid position."
              << std::endl;
    std::cout << "'default: \"wordlist.txt\"' sets the default wordlist to be "
                 "the file 'wordlist.txt'"
              << std::endl;
    std::cout << "'1,1D:' sets the word going down starting at the top left "
                 "corner as using the default wordlist (as set above)"
              << std::endl;
    std::cout << "'4,1A: can cat car boy dog cat' sets the word going across "
                 "starting four letters in on the top row as using the words "
                 "listed after the colon."
              << std::endl;
    std::cout << "Enter ':UNDO' to remove the most recently entered wordlist."
              << std::endl;
    std::cout << "Note: Make sure to type in every word you want it to check, "
                 "even if they are all using the same wordlist. (that's what "
                 "'default: ' is for)."
              << std::endl;
    std::cout << "When you have entered all of the wordlists, enter ':DONE'."
              << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cin >> tmpString;
    while (stillEnteringWordlists) {
        if (tmpString == ":DONE") {
            stillEnteringWordlists = false;
        } else if (tmpString == ":UNDO") {
            tmpInts = coordinates.back();
            coordinates.pop_back();
            wordlists.pop_back();
            std::cout << "Removing wordlist for " << tmpInts[0] << ","
                      << tmpInts[1] << (tmpInts[2] == 0 ? "A" : "D")
                      << std::endl;
            tmpInts.clear();
            std::cin >> tmpString;
        } else if (tmpString.substr(0, 7) == "default") {
            bool wordlisting = true;
            std::vector<std::string> wordlist;
            while (wordlisting) {
                std::cin >> tmpString;
                if (tmpString.find(':') == std::string::npos) {
                    wordlist.push_back(tmpString);
                } else {
                    wordlisting = false;
                }
            }
            if (wordlist.size() == 1) {
                if (wordlist[0][0] == '"') {
                    // Using wordlist from file
                    defaultWordlist = wordlistFromFile(
                        wordlist[0].substr(1, wordlist[0].length() - 2));
                } else {
                    defaultWordlist = wordlist;
                }
            } else {
                defaultWordlist = wordlist;
            }
        } else {
            // TODO: Make it so that the processing isn't delayed by one entry
            // Add the coordinates
            std::size_t commaPos = tmpString.find(',');
            std::size_t colonPos = tmpString.find(':');
            if (commaPos == std::string::npos ||
                colonPos == std::string::npos) {
            } else {
                tmpInts.push_back(stoi(tmpString.substr(0, commaPos)));
                tmpInts.push_back(stoi(
                    tmpString.substr(commaPos + 1, (colonPos - commaPos) - 1)));
                if (tmpString[colonPos - 1] == 'A') {
                    tmpInts.push_back(0);
                } else if (tmpString[colonPos - 1] == 'D') {
                    tmpInts.push_back(1);
                }
                coordinates.push_back(tmpInts);
                tmpInts.clear();
                // Add the actual wordlist
                bool wordlisting = true;
                std::vector<std::string> wordlist;
                while (wordlisting) {
                    std::cin >> tmpString;
                    if (tmpString.find(':') == std::string::npos) {
                        wordlist.push_back(tmpString);
                    } else {
                        wordlisting = false;
                    }
                }
                if (wordlist.size() == 0) {
                    // Using default wordlist
                    wordlists.push_back(defaultWordlist);
                } else if (wordlist.size() == 1) {
                    if (wordlist[0][0] == '"') {
                        // Using wordlist from file
                        wordlists.push_back(wordlistFromFile(
                            wordlist[0].substr(1, wordlist[0].length() - 2)));
                    } else {
                        wordlists.push_back(wordlist);
                    }
                } else {
                    wordlists.push_back(wordlist);
                }
            }
        }
    }

    // Remove impossible words from wordlists
    std::cout << "Optimizing wordlists..." << std::endl;
    for (int i = 0; i < wordlists.size(); i++) {
        // Determine length of word
        int wordLength = 0;
        xPos = coordinates[i][0];
        yPos = coordinates[i][1];
        bool movingAlong = true;
        while (movingAlong) {
            if (xPos <= sizeX && yPos <= sizeY &&
                startingGrid[yPos - 1][xPos - 1] != '/') {
                wordLength++;
            } else {
                movingAlong = false;
            }

            if (coordinates[i][2] == 0) {  // Across
                xPos++;
            } else {  // Down
                yPos++;
            }
        }

        for (int j = wordlists[i].size() - 1; j >= 0; j--) {
            if (wordlists[i][j].length() != wordLength) {
                wordlists[i].erase(wordlists[i].begin() + j);
            } else {
                for (int k = 0; k < wordLength; k++) {
                    if (coordinates[i][2] == 0) {  // Across
                        xPos = coordinates[i][0] + k;
                        yPos = coordinates[i][1];
                    } else {  // Down
                        xPos = coordinates[i][0];
                        yPos = coordinates[i][1] + k;
                    }
                    if (startingGrid[yPos - 1][xPos - 1] != '_' &&
                        startingGrid[yPos - 1][xPos - 1] !=
                            wordlists[i][j][k]) {
                        wordlists[i].erase(wordlists[i].begin() + j);
                        break;
                    }
                }
            }
        }
    }

    // Sort wordlists by size
    for (int i = 0; i < wordlists.size() - 1; i++) {
        int minSize = -1;
        int minId = -1;
        for (int j = i; j < wordlists.size(); j++) {
            if (wordlists[j].size() < minSize || minSize == -1) {
                minSize = wordlists[j].size();
                minId = j;
            }
        }
        std::vector<std::string> tmpList = wordlists[i];
        wordlists[i] = wordlists[minId];
        wordlists[minId] = tmpList;
        std::vector<int> tmpCoords = coordinates[i];
        coordinates[i] = coordinates[minId];
        coordinates[minId] = tmpCoords;
    }
    if (wordlists[0].size() == 0) {
        std::cout << "The wordlist for (" << coordinates[0][0] << ", "
                  << coordinates[0][1]
                  << (coordinates[0][2] == 0 ? " Across)" : " Down)")
                  << " has no valid words for this grid.";
        return 1;
    }

    // Start checking words
    std::cout << "Finished optimization. Checking for possible solutions. "
                 "(This may take a while)"
              << std::endl;

    for (int i = 0; i < wordlists.size(); i++) {
        depths.push_back(-1);
    }

    dIndex = 0;
    while (true) {
        if (goingUp) {
            for (int i = 0; i < wordlists[dIndex][0].length(); i++) {
                if (coordinates[dIndex][2] == 0) {  // Across
                    xPos = coordinates[dIndex][0] + i;
                    yPos = coordinates[dIndex][1];
                } else {  // Down
                    xPos = coordinates[dIndex][0];
                    yPos = coordinates[dIndex][1] + i;
                }
                grid[yPos - 1][xPos - 1] = resets[dIndex][i];
            }
        }
        depths[dIndex]++;
        if (depths[dIndex] >= wordlists[dIndex].size()) {
            depths[dIndex] = -1;
            if (dIndex == 0) {
                break;
            } else {
                goingUp = true;
                dIndex--;
            }
        } else {
            if (depths[dIndex] == 0) {
                std::string reset;
                for (int i = 0; i < wordlists[dIndex][0].length(); i++) {
                    if (coordinates[dIndex][2] == 0) {  // Across
                        xPos = coordinates[dIndex][0] + i;
                        yPos = coordinates[dIndex][1];
                    } else {  // Down
                        xPos = coordinates[dIndex][0];
                        yPos = coordinates[dIndex][1] + i;
                    }
                    reset.push_back(grid[yPos - 1][xPos - 1]);
                }
                if (resets.size() > dIndex) {
                    resets[dIndex] = reset;
                } else {
                    resets.push_back(reset);
                }
            }
            // For each letter see if it can be placed. If they can
            // all be placed, place them and go down. if not, just
            // continue. If at the last layer, print out the current solution.
            bool canBePlaced = true;
            for (int i = 0; i < wordlists[dIndex][0].length(); i++) {
                if (coordinates[dIndex][2] == 0) {  // Across
                    xPos = coordinates[dIndex][0] + i;
                    yPos = coordinates[dIndex][1];
                } else {  // Down
                    xPos = coordinates[dIndex][0];
                    yPos = coordinates[dIndex][1] + i;
                }
                if (grid[yPos - 1][xPos - 1] != '_' &&
                    grid[yPos - 1][xPos - 1] !=
                        wordlists[dIndex][depths[dIndex]][i]) {
                    canBePlaced = false;
                    break;
                }
            }
            if (canBePlaced) {
                for (int i = 0; i < wordlists[dIndex][0].length(); i++) {
                    if (coordinates[dIndex][2] == 0) {  // Across
                        xPos = coordinates[dIndex][0] + i;
                        yPos = coordinates[dIndex][1];
                    } else {  // Down
                        xPos = coordinates[dIndex][0];
                        yPos = coordinates[dIndex][1] + i;
                    }
                    grid[yPos - 1][xPos - 1] =
                        wordlists[dIndex][depths[dIndex]][i];
                }
                goingUp = false;
                dIndex++;
                if (dIndex >= wordlists.size()) {
                    for (int y = 0; y < sizeY; y++) {
                        for (int x = 0; x < sizeX; x++) {
                            std::cout << grid[y][x];
                        }
                        std::cout << std::endl;
                    }
                    std::cout << "------------------------------------"
                              << std::endl;

                    // Reset
                    dIndex--;
                    for (int i = 0; i < wordlists[dIndex][0].length(); i++) {
                        if (coordinates[dIndex][2] == 0) {  // Across
                            xPos = coordinates[dIndex][0] + i;
                            yPos = coordinates[dIndex][1];
                        } else {  // Down
                            xPos = coordinates[dIndex][0];
                            yPos = coordinates[dIndex][1] + i;
                        }
                        grid[yPos - 1][xPos - 1] = resets[dIndex][i];
                    }
                    if (depths[dIndex] == wordlists[dIndex].size() - 1) {
                        depths[dIndex] = -1;
                        goingUp = true;
                        dIndex--;
                    }
                }
            } else {
                goingUp = true;
            }
        }
    }

    return 0;
}

// TO BE USED LATER MAYBE FOR DEBUGGING OR SOMETHING

/*
// Print out all information
// Print grid square-by-square
for (int y = 0; y < sizeY; y++) {
    for (int x = 0; x < sizeX; x++) {
        std::cout << startingGrid[y][x];
    }
    std::cout << std::endl;
}
std::cout << "------------------------------------" << std::endl;
// Print wordlists
for (int i = 0; i < wordlists.size(); i++) {
    std::cout << "(" << coordinates[i][0] << ", " << coordinates[i][1]
              << (coordinates[i][2] == 0 ? " Across):" : " Down):");
    for (int j = 0; j < wordlists[i].size(); j++) {
        std::cout << " " << wordlists[i][j];
    }
    std::cout << std::endl;
}
std::cout << "------------------------------------" << std::endl;
*/