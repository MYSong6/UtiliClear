
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif

# include<iostream>
#include <fstream>
#include "mysql.h"
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

#define PORT_LOC 10248   // Replace with your available port number
#define MSGSIZE 2048	 // The size of message exchanged between the DO and recipient (maximum 2048)
WSADATA wsaData;
SOCKET sListen;
SOCKET sClient;
SOCKADDR_IN local;
SOCKADDR client_user;
int nsize;
MYSQL ceshi;
MYSQL_ROW nextRow;
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
        string unimportantFileName = outputDir2+"/unimportant_column_" + to_string(colIndex + 1) + ".txt";

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

//Helper function to connect local MySQL database
//Replace the MySQL connection setup (i.e., host, user, password, database, and port) in mysql_real_connect with your own configuration based on the MySQL installation settings.
void DB_Con_Init() {
	mysql_init(&ceshi);
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "paper8", 3306, NULL, 0)) {
		cout << "MySQL connected!!! " << endl;
	}
}

//Helper function to execute database query
void DB_Query(string sql) {
	const char* select_sql_char = sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char))
	{
		printf("Query fails: %s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);
		return;
	}
	MYSQL_RES *res;
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
}


/* It takes the input of the file floder storing the database												  */
/* It output the grouping results to two floders to store group results of signifincat and insignificant bits */
/* Each column has two output files corresponding to the groups of significant bits and insignificant bits    */
int main() {

	DB_Con_Init();
    string inputFile = "C:/Users/62555/Desktop/Paper8Data/DB1_Encode.txt";			// Repace with the path of encoded database
	string DB_name = "DB1";															// Database name
	string Recipient = "Recipient1";												//Recipient 
	string outputDir1 = "C:/Users/62555/Desktop/Paper8Data/Significant_group";		// Replace with the path to store groups of significant bits
	string outputDir2 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_group";	// Replace with the path to store groups of insignificant bits
	/* Retrieve the parameters of the recipient */
	string select_sql = "SELECT * FROM recipient_parameters WHERE Name='" + Recipient + "';";
	DB_Query(select_sql);
	string  Num_InsignBits_str, Num_Group_str;
	Num_InsignBits_str = nextRow[1];
	Num_Group_str = nextRow[2];
	int numGroup = atoi(Num_Group_str.c_str());									// Number of groups
    int numInSign = atoi(Num_InsignBits_str.c_str());							// Number of unimportant bits

	/* Database grouping */
    processData(inputFile, numGroup, numInSign, outputDir1, outputDir2);
	system("pause");
    return 0;
}



