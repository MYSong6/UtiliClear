
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif


# include<iostream>
# include <fstream>
# include "mysql.h"
# include "openssl/aes.h"
# include "openssl/sha.h"
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
# include <ctime>
# include <math.h>
# include <assert.h>
# include <random>
# include <algorithm>
#pragma comment(lib,"ws2_32.lib")
using namespace std;



/* Helper function to read each file from a folder (i.e., directory), */
vector<string> getFilesInDirectory(const string &directory) {
	vector<string> files;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((directory + "\\*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		cerr << "Failed to open directory: " << directory << endl;
		return files;
	}

	do {

		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}
		files.push_back(findFileData.cFileName);
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
	return files;
}

/* Helper function to verify the insignificant bits																								*/
/* The floder 'inputDir1' only contains files storing messages (m) for challenge insignifincat bits. (each conlumn corresponds to one file)		*/
/* The floder 'inputDir2' only contains files storing decoded messages (dm) received from the DO. (each conlumn corresponds to one file)        */
void processFiles(const std::string& inputDirectory1, const std::string& inputDirectory2) {


	bool flag = 1;
	int index = 0;

	vector<string> files1 = getFilesInDirectory(inputDirectory1); // Open the folder inputDirectory1
	vector<string> files2 = getFilesInDirectory(inputDirectory2); // Open the folder inputDirectory2


	if (files1.size() != files2.size()) {						  // Tow floder 'inputDirectory1' and  'inputDirectory2' have different number of files, indicating that there exist mistakes in previous execution operations
		cerr << "The number of files in the two folders does not match!" << endl;
		return;
	}

	

	for (size_t i = 0; i < files1.size(); ++i) {
		ifstream file1(inputDirectory1 + "\\" + files1[i]);  // Open one file in inputDirectory1 storing original messages (m) for challenge
		ifstream file2(inputDirectory2 + "\\" + files2[i]);  // Open one file in inputDirectory2 storing the received message (dm) for proof
		
		if (!file1.is_open() || !file2.is_open()) {			 
			cerr << "Failed to open files: " << files1[i] << " and/or " << files2[i] << endl;
			continue;
		}

		string line1, line2;
		while (getline(file1, line1)&&getline(file2,line2)) { // Read one line from file1 and read one line from file2
			index++;
			int num = 0;
			for (int j = 0; j < line1.size(); j++) {
				if (line1[j] == line2[j]) {
					num++;
				}
			}
			if (num<0.85*line1.size()) {
				flag = 0;
				break;
			}
		}
		if (flag == 1) {
			cout << files1[i] << ": Verify success" << endl;
		}
		else {
			cout << files1[i] << ": Verify fail" << endl;
			flag = 1;
		}
		index = 0;
	}

}

/* It inputs two floders 'inputDir1' and 'inputDir2',and output verify results for insignifincat bits											*/
/* The floder 'inputDir1' only contains files storing messages (m) for challenge insignifincat bits. (each conlumn corresponds to one file)		*/
/* The floder 'inputDir2' only contains files storing decoded messages (dm) received from the DO. (each conlumn corresponds to one file)        */
/* Before executing, make sure that the two input floders only contain required files and on other files are included in the floders.			*/
int main(int argc, const char* argv[]) {
	
	const int seed = 211;															
	string inputDir1 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_m";			// Replace with your path storing the files containing message m of challenging
	string inputDir2 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_dm";		// Replace with your path storing the files containing the decoded message dm (received from the DO)					
	processFiles(inputDir1, inputDir2);


	system("pause");
	return 0;


}
