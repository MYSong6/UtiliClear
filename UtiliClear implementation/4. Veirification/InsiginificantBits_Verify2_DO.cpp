
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
#define GMp "232312311797"
#define GMq "971179711797"
#define GMR "17"
#define GMN "225617004017905263169209"


using namespace std;


static unsigned char g[6] =
{
	117, 49, 58, 158, 4, 126 };
unsigned char e2v[256] =
{
	1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
	76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192,
	157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35,
	70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 161,
	95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240,
	253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226,
	217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206,
	129, 31, 62, 124, 248, 237, 199, 147, 59, 118, 236, 197, 151, 51, 102, 204,
	133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132, 21, 42, 84,
	168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115,
	230, 209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
	227, 219, 171, 75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65,
	130, 25, 50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166,
	81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
	18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22,
	44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1 };
unsigned char v2e[256] =
{
	255, 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75,
	4, 100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113,
	5, 138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69,
	29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 166,
	6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 34, 136,
	54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 131, 56, 70, 64,
	30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 84, 250, 133, 186, 61,
	202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 229, 172, 115, 243, 167, 87,
	7, 112, 192, 247, 140, 128, 99, 13, 103, 74, 222, 237, 49, 197, 254, 24,
	227, 165, 153, 119, 38, 184, 180, 124, 17, 68, 146, 217, 35, 32, 137, 46,
	55, 63, 209, 91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190, 97,
	242, 86, 211, 171, 20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162,
	31, 45, 67, 216, 183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246,
	108, 161, 59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90,
	203, 89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215,
	79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175 };

unsigned char gfmul(unsigned char mul1, unsigned char mul2) {
	unsigned char mul3;
	if (mul1 == 0 || mul2 == 0)
		mul3 = 0;
	else
		mul3 = e2v[(v2e[mul1] + v2e[mul2]) % 255];
	return (mul3);
}

unsigned char gfadd(unsigned char add1, unsigned char add2) {
	unsigned char add3;
	add3 = add1 ^ add2;
	return (add3);
}

unsigned char gfinv(unsigned char ivt) {
	unsigned char ivtd;
	if (ivt == 0)
		exit(42);
	ivtd = e2v[255 - v2e[ivt]];
	return (ivtd);
}

unsigned char gfexp(unsigned char mant, unsigned char powr) {
	unsigned char expt;
	if (mant == 0)
		expt = 0;
	else
		expt = e2v[(v2e[mant] * powr) % 255];
	return (expt);
}

void rsencode(unsigned char m[249], unsigned char c[255]) {

	unsigned char r[6], rtmp;

	int i, j;

	for (i = 0; i < 6; i++)
		r[i] = 0;

	for (i = 0; i < 249; i++)
	{
		c[254 - i] = m[i];
		rtmp = gfadd(m[i], r[5]);
		for (j = 5; j > 0; j--)
		{
			r[j] = gfadd(gfmul(rtmp, g[j]), r[j - 1]);
		}
		r[0] = gfmul(rtmp, g[0]);
	}
	for (i = 0; i < 6; i++)
	{
		c[i] = r[i];
	}
}

unsigned char evalpoly(unsigned char p[255], unsigned char x) {

	unsigned char y;
	int i;
	y = 0;

	for (i = 0; i < 255; i++)
	{
		y = gfadd(y, gfmul(p[i], gfexp(x, i)));
	}
	return (y);

}

void syndrome(unsigned char c[255], unsigned char s[7]) {

	extern unsigned char e2v[256];
	int i;
	s[0] = 0;
	for (i = 1; i < 7; i++)
	{
		s[i] = evalpoly(c, e2v[i]);
		s[0] = s[0] | s[i];
	}

}

void errnum(unsigned char s[7], unsigned char* det, int* errs) {


	*det = gfmul(s[2], gfmul(s[4], s[6]));
	*det = gfadd(*det, gfmul(s[2], gfmul(s[5], s[5])));
	*det = gfadd(*det, gfmul(s[6], gfmul(s[3], s[3])));
	*det = gfadd(*det, gfmul(s[4], gfmul(s[4], s[4])));
	*errs = 3;

	if (*det != 0)
		return;

	*det = gfadd(gfmul(s[2], s[4]), gfexp(s[3], 2));
	*errs = 2;
	if (*det != 0)
		return;

	*det = s[1];
	*errs = 1;
	if (*det != 0)
		return;

	*errs = 4;

}

void polysolve(unsigned char polynom[4], unsigned char roots[3], int* numsol) {
	extern unsigned char e2v[256];
	int i, j;
	unsigned char y;

	*numsol = 0;

	for (i = 0; i < 255; i++)
	{
		y = 0;
		for (j = 0; j < 4; j++)
			y = gfadd(y, gfmul(polynom[j], gfexp(e2v[i], j)));
		if (y == 0)
		{
			roots[*numsol] = e2v[i];
			*numsol = *numsol + 1;
		}
	}
}

/* The ECC decoding function								  */
/* It decodes the codeword 'code' to obtain message 'message' */
void rsdecode(unsigned char code[255], unsigned char mesg[249], int* errcode) {

	extern unsigned char v2e[256];
	unsigned char syn[7], deter, z[4], e0, e1, e2, n0, n1, n2, w0, w1, w2,
		x0, x[3];
	int i, sols;

	*errcode = 0;


	for (i = 0; i < 249; i++)
		mesg[i] = code[254 - i];

	syndrome(code, syn);

	if (syn[0] == 0)
		return;


	errnum(syn, &deter, errcode);

	if (*errcode == 4)
		return;



	switch (*errcode)
	{

	case 1:

		x0 = gfmul(syn[2], gfinv(syn[1]));
		w0 = gfmul(gfexp(syn[1], 2), gfinv(syn[2]));
		if (v2e[x0] > 5)
			mesg[254 - v2e[x0]] = gfadd(mesg[254 - v2e[x0]], w0);
		return;

	case 2:

		z[0] = gfmul(gfadd(gfmul(syn[1], syn[3]), gfexp(syn[2], 2)), gfinv(deter));
		z[1] = gfmul(gfadd(gfmul(syn[2], syn[3]), gfmul(syn[1], syn[4])), gfinv(deter));
		z[2] = 1;
		z[3] = 0;

		polysolve(z, x, &sols);
		if (sols != 2)
		{
			*errcode = 4;
			return;
		}

		w0 = gfmul(z[0], syn[1]);
		w1 = gfadd(gfmul(z[0], syn[2]), gfmul(z[1], syn[1]));
		n0 = 254 - v2e[gfinv(x[0])];
		n1 = 254 - v2e[gfinv(x[1])];
		e0 = gfmul(gfadd(w0, gfmul(w1, x[0])), gfinv(z[1]));
		e1 = gfmul(gfadd(w0, gfmul(w1, x[1])), gfinv(z[1]));

		if (n0 < 249)
			mesg[n0] = gfadd(mesg[n0], e0);
		if (n1 < 249)
			mesg[n1] = gfadd(mesg[n1], e1);
		return;

	case 3:

		z[3] = 1;
		z[2] = gfmul(syn[1], gfmul(syn[4], syn[6]));
		z[2] = gfadd(z[2], gfmul(syn[1], gfmul(syn[5], syn[5])));
		z[2] = gfadd(z[2], gfmul(syn[5], gfmul(syn[3], syn[3])));
		z[2] = gfadd(z[2], gfmul(syn[3], gfmul(syn[4], syn[4])));
		z[2] = gfadd(z[2], gfmul(syn[2], gfmul(syn[5], syn[4])));
		z[2] = gfadd(z[2], gfmul(syn[2], gfmul(syn[3], syn[6])));
		z[2] = gfmul(z[2], gfinv(deter));

		z[1] = gfmul(syn[1], gfmul(syn[3], syn[6]));
		z[1] = gfadd(z[1], gfmul(syn[1], gfmul(syn[5], syn[4])));
		z[1] = gfadd(z[1], gfmul(syn[4], gfmul(syn[3], syn[3])));
		z[1] = gfadd(z[1], gfmul(syn[2], gfmul(syn[4], syn[4])));
		z[1] = gfadd(z[1], gfmul(syn[2], gfmul(syn[3], syn[5])));
		z[1] = gfadd(z[1], gfmul(syn[2], gfmul(syn[2], syn[6])));
		z[1] = gfmul(z[1], gfinv(deter));

		z[0] = gfmul(syn[2], gfmul(syn[3], syn[4]));
		z[0] = gfadd(z[0], gfmul(syn[3], gfmul(syn[2], syn[4])));
		z[0] = gfadd(z[0], gfmul(syn[3], gfmul(syn[5], syn[1])));
		z[0] = gfadd(z[0], gfmul(syn[4], gfmul(syn[4], syn[1])));
		z[0] = gfadd(z[0], gfmul(syn[3], gfmul(syn[3], syn[3])));
		z[0] = gfadd(z[0], gfmul(syn[2], gfmul(syn[2], syn[5])));
		z[0] = gfmul(z[0], gfinv(deter));

		polysolve(z, x, &sols);
		if (sols != 3) {
			*errcode = 4;
			return;
		}

		w0 = gfmul(z[0], syn[1]);
		w1 = gfadd(gfmul(z[0], syn[2]), gfmul(z[1], syn[1]));
		w2 = gfadd(gfmul(z[0], syn[3]), gfadd(gfmul(z[1], syn[2]), gfmul(z[2], syn[1])));

		n0 = 254 - v2e[gfinv(x[0])];
		n1 = 254 - v2e[gfinv(x[1])];
		n2 = 254 - v2e[gfinv(x[2])];

		e0 = gfadd(w0, gfadd(gfmul(w1, x[0]), gfmul(w2, gfexp(x[0], 2))));
		e0 = gfmul(e0, gfinv(gfadd(z[1], gfexp(x[0], 2))));
		e1 = gfadd(w0, gfadd(gfmul(w1, x[1]), gfmul(w2, gfexp(x[1], 2))));
		e1 = gfmul(e1, gfinv(gfadd(z[1], gfexp(x[1], 2))));
		e2 = gfadd(w0, gfadd(gfmul(w1, x[2]), gfmul(w2, gfexp(x[2], 2))));
		e2 = gfmul(e2, gfinv(gfadd(z[1], gfexp(x[2], 2))));

		if (n0 < 249)
			mesg[n0] = gfadd(mesg[n0], e0);
		if (n1 < 249)
			mesg[n1] = gfadd(mesg[n1], e1);
		if (n2 < 249)
			mesg[n2] = gfadd(mesg[n2], e2);

		return;

	default:

		*errcode = 4;
		return;

	}

}

// Helper function to convert binary string 'binaryString' to unsigned char array 'arr'
void binaryStringToUnsignedCharArray(const std::string& binaryString, unsigned char* arr) {
	size_t length = binaryString.length();
	for (size_t i = 0; i<length / 8; ++i) {
		std::string byteString = binaryString.substr(i * 8, 8);
		arr[i] = std::bitset<8>(byteString).to_ulong();
	}
}

// Helper function to convert unsigned char array to binary string
string unsignedCharArrayToBinaryString(const unsigned char* arr, size_t size) {
	std::string binaryString = "";
	for (size_t i = 0; i<size; ++i) {
		binaryString += std::bitset<8>(arr[i]).to_string();
	}
	return binaryString;
}

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
/* Helper function to generate random bit-string with fixed length */
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

void processFiles(const std::string& inputDirectory1, const std::string& inputDirectory2, string outputDirectory) {

	vector<string> files1 = getFilesInDirectory(inputDirectory1); // Open the folder inputDirectory1 (containing z)
	vector<string> files2 = getFilesInDirectory(inputDirectory2); // Open the folder inputDirectory2 (containing y)


	if (files1.size() != files2.size()) {						  // Tow floder 'inputDirectory1' and  'inputDirectory2' have different number of files, indicating that there exist mistakes in previous execution operations	 
		cerr << "The number of files in the two folders does not match!" << endl;
		return;
	}


	for (size_t i = 0; i < files1.size(); ++i) {
		ifstream file1(inputDirectory1 + "\\" + files1[i]);		 // Open one file in inputDirectory1 (containing z)
		ifstream file2(inputDirectory2 + "\\" + files2[i]);		 // Open one file in inputDirectory2 (containing y)
		ofstream file3(outputDirectory + "\\" + files1[i]);		 // Open one file in outputDirectory (to store dm)	

		if (!file1.is_open() || !file2.is_open() || !file3.is_open()) {
			cerr << "Failed to open files: " << files1[i] << " and/or " << files2[i] << " and/or " << files1[i] << endl;
			continue;
		}

		string line1, line2;

		while (getline(file1, line1) && getline(file2, line2)) {        // Read one line from file1 and one line from file2
			for (int j = line2.size(); j < line1.size(); j++) {			// Appendix '0' to y to attian length of 2040
				line2 += '0';
			}
			string cw;													
			for (int j = 0; j < line1.size(); j++) {					// line1 XOR line2 (correspond to y XOR z in the paper) to obtain cw
				if (line1[j] == line2[j]) {
					cw += '0';
				}
				else {
					cw += '1';
				}
			}
			/* Decode cw */
			unsigned char x_code[255], xx[249];
			binaryStringToUnsignedCharArray(cw, x_code);
			int fl = 0;
			rsdecode(x_code, xx, &fl);
			string dm = unsignedCharArrayToBinaryString(xx, 249);
			file3 << dm << endl;									// Write the decoded result to file3 in the floder 'outputDirectory'

		}
	}

}





int main(int argc, const char* argv[]) {
	
	string inputDir1 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_z";  // Replace with your floder containing result files (storing z) received from the recipient
	string inputDir2 = "C:/Users/62555/Desktop/Paper8Data/Insignificant_y";  // Replace with your floder containing files that store locked insignificant bits of each column.
	string outputDir = "C:/Users/62555/Desktop/Paper8Data/Insignificant_dm"; // Replace with your floder to store the decoded messages of each column
	processFiles(inputDir1, inputDir2, outputDir);								



	system("pause");
	return 0;


}