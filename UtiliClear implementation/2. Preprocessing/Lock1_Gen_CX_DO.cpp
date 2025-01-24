
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
# include <ctime>
# include <math.h>
# include <assert.h>



#pragma comment(lib,"ws2_32.lib")


#define PORT_LOC 10248
#define MSGSIZE 2048
WSADATA wsaData;
SOCKET sListen;
SOCKET sClient;
SOCKADDR_IN local;
SOCKADDR client_user; 
int nsize;
MYSQL ceshi;
MYSQL_ROW nextRow;
#define GMp "232312311797"                  // The private parameter p of GM encrytion
#define GMq "971179711797"                  // The private parameter q of GM encryption
#define GMR "17"                            // The public parameter xi of GM encryption
#define GMN "225617004017905263169209"      // The public parameter N of GM encryption


using namespace std;


// The function of GM encryption
vector<string> EncGM(mpz_t N, mpz_t R, string msg) {
	vector<string> res;
	string one_str = "1";
	string zero_str = "0";
	const char* one_ptr = one_str.data();
	const char* zero_ptr = zero_str.data();
	mpz_t one, zero;
	mpz_init(one);
	mpz_init(zero);
	mpz_init_set_str(one, one_ptr, 10);
	mpz_init_set_str(zero, zero_ptr, 10);
	for (int i = 0; i < msg.size(); i++) {
		gmp_randstate_t state;
		mpz_t r, gcd;
		mpz_init(r);
		mpz_init(gcd);
		while (1) {
			mpz_random(r, 32);
			mpz_mod(r, r, N);
			mpz_gcd(gcd, r, N);
			string gcd_str = mpz_get_str(NULL, 10, gcd);
			if (gcd_str == "1") {
				break;
			}
		}


		if (msg[i] == '1') {
			mpz_t c;
			mpz_init(c);
			mpz_mul(c, r, r);
			mpz_mul(c, c, R);
			mpz_mod(c, c, N);
			string re = mpz_get_str(NULL, 10, c);
			res.push_back(re);
		}
		else {
			mpz_t c;
			mpz_init(c);
			mpz_mul(c, r, r);
			mpz_mod(c, c, N);
			string re = mpz_get_str(NULL, 10, c);
			res.push_back(re);
		}
	}
	return res;
}

// The function of GM decryption
string DecGM(mpz_t p, mpz_t q, mpz_t R, mpz_t N, vector<string> Cip) {
	string res;
	for (int i = 0; i < Cip.size(); i++) {
		mpz_t c, cmodp, gcd1, gcd2;
		mpz_init(c);
		mpz_init(gcd1);
		mpz_init(gcd2);
		const char* c_ptr = Cip[i].data();
		mpz_set_str(c, c_ptr, 10);
		if ((mpz_legendre(c, p) == 1) || (mpz_legendre(c, q) == 1)) {
			res += "0";
		}
		else {
			res += "1";
		}
	}
	return res;
}


// Helper function to convert high-precision numbers (from Group G) to string
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

/* Helper function to encrypt groups of insignificant bits into GM ciphertext                                                         */
/* The floder 'inputDirectory' contains files of grouped insignificant bits (each column corresponds to a file)                      */
/* The function read each line from each file in a folder (i.e., inputDirectory) and then compute the GM ciphertext of each line      */
/* The ciphertexts for the messages from the same file are stored in an identical file using the same name as the original input file */
/* All the output files are stored in an identical foloder (i.e., outputDirectory).                                                   */
void Generate_CX(const std::string& inputDirectory, const std::string& outputDirectory) {


	string GMp_str = GMp;
	string GMq_str = GMq;
	string GMR_str = GMR;
	const char* p_ptr = GMp_str.data();
	const char* q_ptr = GMq_str.data();
	const char* R_ptr = GMR_str.data();

	mpz_t p, q, R, n;
	mpz_init(q);
	mpz_init(p);
	mpz_init(n);
	mpz_init(R);
	mpz_init_set_str(p, p_ptr, 10);
	mpz_init_set_str(q, q_ptr, 10);
	mpz_init_set_str(R, R_ptr, 10);
	mpz_mul(n, p, q);


	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((inputDirectory + "\\*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cerr << "Error opening directory: " << inputDirectory << std::endl;
		return;
	}

	do {
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string inputFile = inputDirectory + "\\" + findFileData.cFileName;
			std::string outputFile = outputDirectory + "\\" + findFileData.cFileName;

			std::ifstream inFile(inputFile);                                                // Open one file in inputDirectory
			std::ofstream outFile(outputFile);                                              // Open its corrseponding output file

			if (!inFile.is_open()) {
				std::cerr << "Error opening input file: " << inputFile << std::endl;
				continue;
			}
			if (!outFile.is_open()) {
				std::cerr << "Error opening output file: " << outputFile << std::endl;
				continue;
			}

			std::string line;
			while (std::getline(inFile, line)) {                                            // Read one line

				vector<string> result = EncGM(n, R, line);                                  // Compute the GM cipheretxt for the message in this line.
				for (int i = 0; i < result.size(); i++) {                                   // Write the GM cipheretxt to the output file
					outFile << result[i] << endl;
				}
				outFile << "!!" << endl;                                                    // We use the symbol "!!" to mark the end of a group
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);

}

/*  The DO computes GM ciphertexts for grouped insignificant bits.                             */
/*  The folder 'inputDir' stores the grouped insignificant bits for each column.              */
/*  The folder 'outputDir' stores the corresponding GM ciphertext for each file in 'inputDir'. */
int main(int argc, const char* argv[]) {
	
	
	
	string inputDir = "C:/Users/62555/Desktop/Paper8Data/Insignificant_group";		// Replace with your path storing the grouping results of insignificant bits
	string outputDir = "C:/Users/62555/Desktop/Paper8Data/Insignificant_cx";		// Replace with your path storing the C_x reslts of each column

	Generate_CX(inputDir, outputDir);

	system("pause");
	return 0;


}
