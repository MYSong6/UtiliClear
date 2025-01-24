
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif

# include<iostream>
#include <fstream>
#include "openssl/aes.h"
#include "openssl/sha.h"
# include "gmp.h"
# include <stdio.h>
# include <string.h>
# include "pbc.h"
# include <vector> 
# include <map>
# include <sstream>
# include <string>
# include <bitset>
# include <stdlib.h>
# include <math.h>
# include <ctime>
# include <algorithm>
# include <assert.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

// Function to remove double quotes from a string
string removeQuotes(const string &s) {
    if (s.size() > 1 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);  // Remove the first and last character (quotes)
    }
    return s;  // If no quotes, return the string as it is
}

// Function to process data from the input file
void processData(const string &inputFile, int numGroup, int numSign, const string &outputDir1, const string &outputDir2) {
    ifstream input(inputFile);
    if (!input.is_open()) {
        cerr << "Error opening input file." << endl;
        return;
    }

    string line;
    vector<vector<string>> columnsData;
    vector<string> headers;

    // Read the first line (headers)
    getline(input, line);
    stringstream ss(line);
    string header;
    while (getline(ss, header, '\t')) {
        headers.push_back(header);  // Save the header for reference, not used in data processing
    }

    // Read the data lines
    while (getline(input, line)) {
        stringstream ss(line);
        vector<string> rowData;
        string value;

        while (getline(ss, value, '\t')) {
            rowData.push_back(removeQuotes(value));  // Remove quotes before adding to data
        }

        // Add rowData to columnsData
        if (columnsData.empty()) {
            columnsData.resize(rowData.size());
        }

        for (size_t i = 0; i < rowData.size(); i++) {
            columnsData[i].push_back(rowData[i]);
        }
    }

    input.close();

    // Process each column separately except for the last column
    for (size_t colIndex = 0; colIndex < columnsData.size() - 1; colIndex++) {  // Skip the last column
        vector<string> &column = columnsData[colIndex];
        int totalRows = column.size();

        // Divide the column into groups
        int groupSize = totalRows / numGroup;
        int remainder = totalRows % numGroup;

        vector<string> importantBits(numGroup, "");
        vector<string> unimportantBits(numGroup, "");

        int startIndex = 0;
        for (int i = 0; i < numGroup; i++) {
            int currentGroupSize = groupSize + (i < remainder ? 1 : 0); // Distribute remainder across groups

            for (int j = startIndex; j < startIndex + currentGroupSize; j++) {
                string value = column[j];
                int len = value.length();
                int signEnd = len - numSign; // Last numSign characters are unimportant
                if (signEnd < 0) signEnd = 0;

                string importantPart = value.substr(0, signEnd);
                string unimportantPart = value.substr(signEnd);

                // Concatenate important and unimportant parts into corresponding groups
                importantBits[i] += importantPart ;
                unimportantBits[i] += unimportantPart;
            }
            importantBits[i] += "\t";
            unimportantBits[i] += "\t";
            startIndex += currentGroupSize;
        }

        // Repace with your pathes and filenames for each column's output
        string importantFileName = outputDir1+"/important_column_" + to_string(colIndex + 1) + ".txt";
        string unimportantFileName = outputDir2 + "/unimportant_column_" + to_string(colIndex + 1) + ".txt";

        // Write the important and unimportant bits to separate files
        ofstream outImportant(importantFileName);
        ofstream outUnimportant(unimportantFileName);

        if (!outImportant.is_open() || !outUnimportant.is_open()) {
            cerr << "Error opening output files for column " << colIndex + 1 << endl;
            return;
        }

        for (int i = 0; i < numGroup; i++) {
            // Remove the trailing tab and add a newline
            importantBits[i].pop_back();
            unimportantBits[i].pop_back();

            // Write to the output files (Important and Unimportant)
            outImportant << importantBits[i] << "\n";
            outUnimportant << unimportantBits[i] << "\n";
        }

        outImportant.close();
        outUnimportant.close();
    }

    cout << "Processing completed." << endl;
}


/* It takes the input of the file floder storing the fingerprinted database                                   */
/* It output the grouping results to two floders to store group results of signifincat and insignificant bits */
/* Each column has two output files corresponding to the groups of significant bits and insignificant bits    */
int main() {

    string inputFile = "C:/Users/62555/Desktop/Paper8Data/FDB1_Encode.txt";					// Replace with the path of encoded database
	string outputDir1 = "C:/Users/62555/Desktop/Paper8Data/Sign_group";						// Replace with the path to store groups of significant bits
	string outputDir2 = "C:/Users/62555/Desktop/Paper8Data/Insign_group";					// Replace with the path to store groups of insignificant bits

	const int numGroup = 50;																// Number of groups
    const int numSign = 1;																	// Number of unimportant bits

	/* Database grouping */
    processData(inputFile, numGroup, numSign, outputDir1, outputDir2);
	system("pause");
    return 0;
}



