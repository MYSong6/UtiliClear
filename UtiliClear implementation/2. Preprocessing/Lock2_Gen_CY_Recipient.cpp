
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
# include <random>
# include <algorithm>

#pragma comment(lib,"ws2_32.lib")


#define MSGSIZE 2048
WSADATA wsaData;
SOCKET sListen;
SOCKET sClient;
SOCKADDR_IN local;
SOCKADDR client_user; 
int nsize;
MYSQL ceshi;                                    
MYSQL_ROW nextRow;

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

/* Helper function to embed random bit-string to the GM ciphertext                                                */
/* It encrypt 'random_str' and then perform the Hadamard product bewteen the ciphertext of 'random_str' and GMCip */
vector<string> BuyerSideCXOR(vector<string> GMCip, string random_str, mpz_t N, mpz_t R) {
	vector<string> res = EncGM(N, R, random_str);
	vector<string> c1Mulc2;
	for (int i = 0; i<res.size(); i++) {
		mpz_t c1, c2, c3;
		mpz_init(c1);
		mpz_init(c2);
		mpz_init(c3);
		const char* c1_ptr = GMCip[i].data();
		const char* c2_ptr = res[i].data();
		mpz_init_set_str(c1, c1_ptr, 10);
		mpz_init_set_str(c2, c2_ptr, 10);
		mpz_mul(c3, c1, c2);
		mpz_mod(c3, c3, N);
		string c3_str = mpz_get_str(NULL, 10, c3);
		c1Mulc2.push_back(c3_str);
	}
	return c1Mulc2;
}

/* Helper function to read each file from a folder (i.e., directory)   */
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

/* Helper function to generate random bit-string with fixed length   */
string generate_random_binary_string(long long length) {
	string result;
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, 1);
	for (int i = 0; i < length; ++i) {
		result += to_string(distrib(gen));
	}
	return result;
}


/* Helper function to generate the Cy                                                                                          */
/* The floder 'inputDirectory' contains files of GM ciphertext Cx (each conlum corresponds to a file)                          */
/* The function reads each file in a folder (i.e., inputDirectory) and then embeds perturbation to the ciphetrtext             */
/* The perturbed ciphertexts from the same file are stored in an identical file using the same name as the original input file */
/* The output files containing perturbed ciphertexts are stored in an identical foloder (i.e., outputDirectory1).              */
/* The output files containing the random bit-stings are stored in an identical foloder (i.e., outputDirectory2).              */
void Generate_CY(const std::string& inputDirectory, const std::string& outputDirectory1, const std::string& outputDirectory2, int num_group, const int shuf_seed, string Par_path) {


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


	const char* n_ptr = GMN_str.data();
	const char* R_ptr = GMR_str.data();

	mpz_t p, q, R, n;
	mpz_init(n);
	mpz_init(R);
	mpz_init_set_str(n, n_ptr, 10);
	mpz_init_set_str(R, R_ptr, 10);


	vector<string> files1 = getFilesInDirectory(inputDirectory);        // Open the folder inputDirectory1
	mt19937_64 eng(shuf_seed);
	

	for (size_t i = 0; i < files1.size(); ++i) {
		ifstream file1(inputDirectory + "\\" + files1[i]);               // Open one file in inputDirectory1
		ofstream file2(outputDirectory1 + "\\" + files1[i]);             // Open one file in outputDirectory1
		ofstream file3(outputDirectory2+ "\\" + files1[i]);              // Open one file in outputDirectory2
		for (int j = 0; j < num_group; j++) {
			vector<string> GMCipher;
			string line1;
			while (getline(file1, line1)) {                              // Read one group
				if (line1 == "!!") {                                     // "!!" marks the end of one group
					break;
				}
				GMCipher.push_back(line1);
			}
			string randstr = generate_random_binary_string(GMCipher.size());    // Generate a random bit-string u
			vector<string> res = BuyerSideCXOR(GMCipher, randstr, n, R);        // Cx Hadamard product Cu
			shuffle(res.begin(), res.end(),eng);                                // Shuffle the result of Hadamard product
			for (int j = 0; j < res.size(); j++) {
				file2 << res[j] << endl;
			}
			file2 << "!!" << endl;                                       // "!!" marks the end of one group
			file3 << randstr << endl;
		}
	}	
	
}


/*  The recipient embeds perturbation to the GM ciphertexts for grouped insignificant bits.               */
/*  The folder 'inputDir' stores the the GM ciphertexts (Cx) for grouped insignificant bits.              */
/*  The folder 'outputDir1' stores the corresponding pertrubed GM ciphertext for each file in 'inputDir'. */
/*  The folder 'outputDir2' stores the random bit-strings used for each file in 'inputDir'.               */
int main(int argc, const char* argv[]) {
	
	

	const int Num_group = 30;
	const int shuftSeed = 211;
	string inputDir1 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_cx";		// Replace with your path storing the Cx reslts of each column
	string outputDir1 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_cy";       // Replace with your path to store the Cy reslts of each column
	string outputDir2 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_u";        // Replace with your path to store the u reslts of each column
	string Par_path = "C:/Users/62555/Desktop/Paper8Data/Params.txt";               // Replace with your path storing the recipient's parameters.

	Generate_CY(inputDir1, outputDir1, outputDir2, Num_group, shuftSeed,Par_path);
	


	system("pause");
	return 0;


}
