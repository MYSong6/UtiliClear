
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
# include <windows.h>
# include <math.h>
# include <ctime>
#pragma comment(lib,"ws2_32.lib")

#include <stdlib.h>
#include <assert.h>

#define MSGSIZE 2048                                    // The size of message exchanged between the DO and recipient (maximum 2048)
WSADATA wsaData;
SOCKET sListen;
SOCKET sClient;
SOCKADDR_IN local;
SOCKADDR client_user; 
int nsize;
MYSQL ceshi;                                       
MYSQL_ROW nextRow;

using namespace std;

//Helper function to convert high-precision numbers to string
string ExtraG1(char g_char[MSGSIZE]) {
	string msg_str;
	for (int i = 0; i < MSGSIZE; i++) {
		if ((g_char[i] <= '9'&&g_char[i] >= '0') || g_char[i] == ']' || g_char[i] == '[' || g_char[i] == ',' || g_char[i] == ' ') {
			msg_str += g_char[i];
		}
		else {
			break;
		}
	}
	return msg_str;
}

//Helper function to convert bit-string to Hex-string
string binaryToHex(const string& binaryStr) {
	string ret;
	static const char *hex = "0123456789ABCDEF";
	for (auto c : binaryStr) {
		ret.push_back(hex[(c >> 4) & 0xf]);
		ret.push_back(hex[c & 0xf]);
	}
	return ret;
}

//Hash function that maps a string to Hex hash value
string Hash(string msg, int size) {
	char * DataChar = new char[size + 1];
	for (int j = 0; j <size; j++) {
		DataChar[j] = msg[j];
	}
	DataChar[size] = '\0';
	unsigned char temp2[64];
	SHA512_CTX ctx;
	SHA512_Init(&ctx);
	SHA512_Update(&ctx, DataChar, strlen(DataChar));
	SHA512_Final(temp2, &ctx);
	string TempHash;
	delete[] DataChar;
	for (int i = 0; i < 64; i++) {
		TempHash += temp2[i];
	}
	string HashValue16 = binaryToHex(TempHash);
	return HashValue16;
}

/* PC commitment verifying function */
/* Given the disclosed message 'input1', the parameters (N,g, and g), */
/* and the commitment (input2,input3), it output verification result. */
int PC_COM_Veri(const std::string& input1, string &input2,string &input3, string N_str, string g_str, string h_str) {
	

	const char* N_ptr = N_str.data();
	const char* g_ptr = g_str.data();
	const char* h_ptr = h_str.data();
	const char* d_ptr = input3.data();
	mpz_t N;
	mpz_init(N);
	mpz_init_set_str(N, N_ptr, 10);
	pbc_param_t param;
	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);
	element_t g, h;
	element_init_G1(g, pairing);
	element_init_G1(h, pairing);
	element_set_str(g, g_ptr, 10);
	element_set_str(h, h_ptr, 10);


	element_t gPowd;
	element_init_G1(gPowd, pairing);
	element_t d;
	element_init_Zr(d, pairing);
	element_set_str(d, d_ptr,10);
	element_pow_zn(gPowd, g, d);

	string Hout = Hash(input1, input1.size());
	const char* msg_ptr = Hout.data();
	element_t m;
	element_init_Zr(m, pairing);
	element_set_str(m, msg_ptr, 16);		
	element_t hPowm, comm;
	element_init_G1(hPowm, pairing);
	element_init_G1(comm, pairing);
	element_pow_zn(hPowm, h, m);
	element_mul(comm, gPowd, hPowm);
	char comm_char[MSGSIZE];
	element_snprint(comm_char, MSGSIZE, comm);
	string comm_str = ExtraG1(comm_char);
	
	if (comm_str == input2) {
		
		return 1;
	}
    else{
        return 0;
    }
}

/* Helper function to read each file from a folder (i.e., directory),   */
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
/* Helper function to two files and verify significant bits */
/* inputDirectory1 stores the grouped significant bits of the recipient, and inputDirectory2 stores the commitment recived from the DO. */
/* Par_path is the parameter storing file of the DO.                                                                                    */
/* It read one line from  inputDirectory1 (one group) and two lines from inputDirectory2 (the commiment corresponding to the group),    */
/* then it call the  PC_COM_Veri function to verify the group.                                                                          */
void processFiles(const std::string& inputDirectory1, const std::string& inputDirectory2, string  Par_path) {


    //Retrieve the parameters used for commitment verification.
	fstream Read_ptr;
	string N_str, g_str, GMN_str, GMR_str, h_str, lamda_str;
	Read_ptr.open(Par_path);
	getline(Read_ptr, N_str);
	getline(Read_ptr, g_str);
	getline(Read_ptr, GMN_str);
	getline(Read_ptr, GMR_str);
	getline(Read_ptr, h_str);
	getline(Read_ptr, lamda_str);
	Read_ptr.close();

	vector<string> files1 = getFilesInDirectory(inputDirectory1);       // Open the folder inputDirectory1
	vector<string> files2 = getFilesInDirectory(inputDirectory2);       // Open the folder inputDirectory2

	
	if (files1.size() != files2.size()) {
		cerr << "The number of files in the two folders does not match!" << endl;
		return;
	}

	
	for (size_t i = 0; i < files1.size(); ++i) {
		ifstream file1(inputDirectory1 + "\\" + files1[i]);             // Open one file in inputDirectory1
		ifstream file2(inputDirectory2 + "\\" + files2[i]);             // Open one file in inputDirectory2
		cout << files1[i] << " " <<files2[i] << endl;
		if (!file1.is_open() || !file2.is_open()) {
			cerr << "Failed to open files: " << files1[i] << " and/or " << files2[i] << endl;
			continue;
		}

		string line1, line2, line3;

		
		while (getline(file1, line1) ) {                                         // Read one line from file1
			
			int res=0;
			if (getline(file2, line2)&&getline(file2, line3)) {                  // Read two lines (the corresponding commitmemt to the message from file1) from file2
				
				res = PC_COM_Veri(line1, line2, line3, N_str, g_str, h_str);     // Verify the commitment
			}
			else {
				cerr << "Failed to read a line from " << files2[i] << endl;
			}
			if (res == 0) {
				cout << "Signifincatin bit verification fails!" << endl;
				return;
			}
		}

	}
	cout << "Signifincatin bit verification successes!" << endl;


}

/*  The recipient verify commitments for grouped significant bits.                            */
/*  The folder 'inputDir1' stores the grouped significant bits for each column.               */
/*  The folder 'inputDir2' stores the corresponding commitments for each file in 'inputDir1'. */
/*  The file 'Par_path' stores the recipient's parameters.                                    */
int main(int argc, const char* argv[]) {
	
	
	string inputDir1 = "C:/Users/62555/Desktop/Paper8Data/Significant_group";	// Replace with your path storing the grouping results of significant bits
	string inputDir2 = "C:/Users/62555/Desktop/Paper8Data/Significant_com";		// Replace with your path storing the commitment reslts of each column
	string Par_path = "C:/Users/62555/Desktop/Paper8Data/Params.txt";			// Replace with your path storing the recipient's parameters

	processFiles(inputDir1, inputDir2, Par_path);
	system("pause");
	return 0;


}
