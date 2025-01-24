
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
# include <windows.h>
# include <math.h>
# include <ctime>
#include <stdlib.h>
#include <assert.h>



#pragma comment(lib,"ws2_32.lib")

#define PORT_LOC 10248              // Replace with your available port number
#define MSGSIZE 2048                // The size of message exchanged between the DO and recipient (maximum 2048)
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

//Helper function to connect local MySQL database
//Replace the MySQL connection setup (i.e., host, user, password, database, and port) in mysql_real_connect with your own configuration based on the MySQL installation settings.
void DB_Con_Init() {
	mysql_init(&ceshi);
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "paper8", 3306, NULL, 0)) {
		cout << "成功连接MYSQL!!! " << endl;
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

/* PC commitment function */
/* Given the message 'input', the parameters (N,g, and g), */
/* it output the commiment to the message.                 */
std::pair<std::string, std::string> PC_COM(const std::string& input, string N_str, string g_str, string h_str) {

	const char* N_ptr = N_str.data();
	const char* g_ptr = g_str.data();
	const char* h_ptr = h_str.data();
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
	mpz_t d_t;
	mpz_init(d_t);
	pbc_mpz_random(d_t, N);
	element_t d;
	element_init_Zr(d, pairing);
	element_set_mpz(d, d_t);
	element_pow_zn(gPowd, g, d);

	string Hout = Hash(input, input.size());
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
		
	string d_str = mpz_get_str(NULL, 10, d_t);
	return{ comm_str, d_str};
}

/* Helper function to read each line from each file in a folder (i.e., inputDirectory),   */
/* and then call PC commitment function to obtain commitment of the message in each line  */
/* The commiments for the messages from the same file are stored in an identical file using the same name as the original input file */
/* The output files are stored in an identical foloder (i.e., outputDirectory).  */
void processFiles(const std::string& inputDirectory, const std::string& outputDirectory) {
    
    //Retrieve the parameters used for commitment.
	string N_str, g_str, h_str;
	DB_Con_Init();
	string select_sql = "SELECT * FROM do_parameters;";
	DB_Query(select_sql);
	N_str = nextRow[2];
	g_str = nextRow[3];
	h_str = nextRow[4];

    // Open the folder inputDirectory
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

			std::ifstream inFile(inputFile);                // Open one file in inputDirectory
			std::ofstream outFile(outputFile);              // Open its corrseponding output file

			if (!inFile.is_open()) {
				std::cerr << "Error opening input file: " << inputFile << std::endl;
				continue;
			}
			if (!outFile.is_open()) {
				std::cerr << "Error opening output file: " << outputFile << std::endl;
				continue;
			}

			std::string line;
			while (std::getline(inFile, line)) {                                // Read one line
				auto result = PC_COM(line, N_str, g_str, h_str);                // Compute the commitment for the message in this line.
				outFile << result.first << "\n" << result.second << "\n";       // Write the commiment results to the output file
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind); 
	
}

/*  The DO computes commitments for grouped significant bits.                                */
/*  The folder 'inputDir' stores the grouped significant bits for each column.               */
/*  The folder 'outputDir' stores the corresponding commitments for each file in 'inputDir'. */
int main(int argc, const char* argv[]) {
	
	
	string inputDir = "C:/Users/62555/Desktop/Paper8Data/Significant_group";		// Replace with your path (folder) storing the grouping results of significant bits
	string outputDir = "C:/Users/62555/Desktop/Paper8Data/Significant_com";			// Replace with your path (folder) storing the commitment reslts of each column

	processFiles(inputDir, outputDir);
	system("pause");
	return 0;


}
