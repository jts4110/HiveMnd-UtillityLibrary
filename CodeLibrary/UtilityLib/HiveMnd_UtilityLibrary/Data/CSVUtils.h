#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

namespace HiveMnd::Data
{
    // =========================================================================
    // CSVUtils
    // Purpose : Provides simple CSV parsing and writing utilities.
    // Notes   : Designed for readability, small datasets, and debugging logs.
    // =========================================================================
    class CSVUtils
    {
    public:
        // ---------------------------------------------------------------------
        // Function: ReadCSV
        // Purpose : Reads a CSV file into a 2D vector of strings.
        // Params  : filePath - Path to CSV file.
        // Returns : Vector of rows, where each row is a vector of cell strings.
        // ---------------------------------------------------------------------
        static std::vector<std::vector<std::string>> ReadCSV(const std::string& filePath)
        {
            std::ifstream file(filePath);
            std::vector<std::vector<std::string>> data;

            if (!file.is_open())
            {
                std::cerr << "CSVUtils: Failed to open file: " << filePath << std::endl;
                return data;
            }

            std::string line;
            while (std::getline(file, line))
            {
                std::vector<std::string> row;
                std::stringstream ss(line);
                std::string cell;

                while (std::getline(ss, cell, ','))
                    row.push_back(cell);

                data.push_back(row);
            }

            file.close();
            return data;
        }

        // ---------------------------------------------------------------------
        // Function: WriteCSV
        // Purpose : Writes 2D vector data to a CSV file.
        // Params  : filePath - Output file path.
        //           data - 2D vector containing rows of strings.
        // Returns : true if successful, false otherwise.
        // ---------------------------------------------------------------------
        static bool WriteCSV(const std::string& filePath,
            const std::vector<std::vector<std::string>>& data)
        {
            std::ofstream file(filePath);
            if (!file.is_open())
            {
                std::cerr << "CSVUtils: Failed to open file for writing: "
                    << filePath << std::endl;
                return false;
            }

            for (const auto& row : data)
            {
                for (size_t i = 0; i < row.size(); ++i)
                {
                    file << row[i];
                    if (i < row.size() - 1)
                        file << ",";
                }
                file << "\n";
            }

            file.close();
            return true;
        }

        // ---------------------------------------------------------------------
        // Function: PrintCSV
        // Purpose : Outputs CSV data to the console for quick inspection.
        // Params  : data - 2D vector containing CSV data.
        // ---------------------------------------------------------------------
        static void PrintCSV(const std::vector<std::vector<std::string>>& data)
        {
            for (const auto& row : data)
            {
                for (size_t i = 0; i < row.size(); ++i)
                {
                    std::cout << row[i];
                    if (i < row.size() - 1)
                        std::cout << " | ";
                }
                std::cout << std::endl;
            }
        }
    };
}

