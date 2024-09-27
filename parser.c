#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Please specify an input file." << std::endl;
        return 1;
    }

    const std::string input_file = argv[1];
    const std::string HEADER = "INSTR           FORMAT          OAT             TAAM            OBJ\n";
    
    // Open input file
    std::ifstream objectFile(input_file);
    if (!objectFile.is_open()) {
        std::cout << "Couldn't open input file." << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outputFile("obj_struct.txt");
    if (!outputFile.is_open()) {
        std::cout << "Couldn't open output file." << std::endl;
        return 1;
    }

    // Write header to output file
    outputFile << HEADER;

    // Discard the first line of input file
    std::string line;
    std::getline(objectFile, line);

    // Skip 7 characters using seekg
    objectFile.seekg(7, std::ios::cur);
    if (!objectFile.good()) {
        std::cout << "Error skipping characters." << std::endl;
        return 1;
    }

    // Read the next two characters (sizeBytes) and convert them to a long integer
    char sizeBytes[3] = {0};  // We need 3 elements for 2 characters + null terminator
    objectFile.read(sizeBytes, 2);  // Read 2 characters
    if (objectFile.gcount() < 2) {
        std::cout << "Failed to read size bytes." << std::endl;
        return 1;
    }

    try {
        long result = std::stol(sizeBytes, nullptr, 16);
        std::cout << "Size of object code for first line: " << result << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid argument: couldn't convert size bytes." << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: couldn't convert size bytes." << std::endl;
        return 1;
    }

    return 0;
}

