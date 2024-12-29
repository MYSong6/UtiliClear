
#ifdef _MSC_VER
#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif
#endif

//#pragma comment(lib,"ws2_32.lib")
# include<iostream>
//#pragma comment(lib,"libssl.lib")
//#pragma comment(lib, "libcrypto.lib")
#include <fstream>
#include "mysql.h"
#include <alibabacloud/oss/OssClient.h>
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




#pragma comment(lib,"ws2_32.lib")
//# include <winsock.h>
//# include <OssClientImpl.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>
//#include <WinSock2.h>
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
using namespace AlibabaCloud::OSS;

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

string readFileIntoString(char * filename)
{
	ifstream ifile(filename);
	
	ostringstream buf;
	char ch;
	while (ifile.get(ch))
		buf.put(ch);
	
	return buf.str();
}

string HexToStr(std::string str)
{
	std::string hex = str;
	long len = hex.length();
	std::string newString;
	for (long i = 0; i< len; i += 2)
	{
		std::string byte = hex.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
		newString.push_back(chr);
	}
	return newString;
}

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

string InfoExtra(char MSG[1024 * 2]) {
	string res = "";
	for (int i = 0; i < MSGSIZE; i++) {
		if (MSG[i] == '!') {
			break;
		}
		else {
			res += MSG[i];
		}
	}
	return res;
}

string unsignedCharArrayToBinaryString(const unsigned char* arr, size_t size) {
	std::string binaryString = "";
	for (size_t i = 0; i < size; ++i) {
		binaryString += std::bitset<8>(arr[i]).to_string();
	}
	return binaryString;
}

void binaryStringToUnsignedCharArray(const std::string& binaryString, unsigned char* arr) {
	size_t length = binaryString.length();
	for (size_t i = 0; i < length / 8; ++i) {
		std::string byteString = binaryString.substr(i * 8, 8);
		arr[i] = std::bitset<8>(byteString).to_ulong();
	}
}

string binaryToHex(const string& binaryStr) {
	string ret;
	static const char *hex = "0123456789ABCDEF";
	for (auto c : binaryStr) {
		ret.push_back(hex[(c >> 4) & 0xf]);
		ret.push_back(hex[c & 0xf]);
	}
	return ret;
}

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



void Soceket_init() {
	memset(&local, 0, sizeof(local));
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);// 任何一个客户端IP都可以连接
	while (::bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR)) == SOCKET_ERROR) {
	}
	nsize = sizeof(SOCKADDR);
}


void Info_Send(string msg) {

	char MSG[MSGSIZE];
	int index = 0;
	for (int i = 0; i <msg.size(); i++) {
		MSG[index] = msg[i];
		index++;
	}
	MSG[msg.size()] = '!';
	while (listen(sListen, 20) == -1) {
	}
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	send(sClient, MSG, MSGSIZE, NULL);
	closesocket(sClient);
}

string Info_Receive() {

	char MSG[MSGSIZE];
	while (listen(sListen, 20) == -1) {
	}
	sClient = accept(sListen, (sockaddr*)&client_user, &nsize);
	recv(sClient, MSG, MSGSIZE, NULL);
	closesocket(sClient);
	string res = InfoExtra(MSG);
	return res;
}
//存放查询sql语句字符串数组
void DB_Con_Init() {
	mysql_init(&ceshi);
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "paper8", 3306, NULL, 0)) {
		cout << "MYSQL!!! " << endl;
	}
}

void DB_Query(string sql) {
	const char* select_sql_char = sql.data();
	if (0 != mysql_query(&ceshi, select_sql_char))
	{
		printf("%s\n", mysql_error(&ceshi));
		mysql_close(&ceshi);                         //关闭连接
		return;
	}
	MYSQL_RES *res;
	res = mysql_store_result(&ceshi);
	nextRow = mysql_fetch_row(res);
}

void Param_Init(int parame_length) {
	clock_t time = clock();
	gmp_randstate_t grt;
	gmp_randinit_default(grt);
	gmp_randseed_ui(grt, time);
	//Generate large prime p
	mpz_t p;
	mpz_init(p);
	mpz_urandomb(p, grt, parame_length);
	mpz_nextprime(p, p);
	//Generate large prime q
	mpz_t q;
	mpz_init(q);
	mpz_urandomb(q, grt, parame_length);
	mpz_nextprime(q, q);
	//Compute the order of G2  (n)
	mpz_t N;
	mpz_init(N);
	mpz_mul(N, p, q);

	pbc_param_t param;
	pbc_param_init_a1_gen(param, N);
	pairing_t pairing;
	pairing_init_pbc_param(pairing, param);

	element_t g;
	element_init_G1(g, pairing);
	element_random(g);

	string p_str, q_str, N_str, g_str;
	N_str = mpz_get_str(NULL, 10, N);
	p_str = mpz_get_str(NULL, 10, p);
	q_str = mpz_get_str(NULL, 10, q);
	char g_char[MSGSIZE];
	element_snprint(g_char, 2 * parame_length, g);
	g_str = ExtraG1(g_char);
	string INSERT_SQL_str = "INSERT INTO cloud_parameters values('" + p_str + "','" + q_str + "','" + N_str + "','" + g_str + "');";
	const char* INSERT_SQL_char = INSERT_SQL_str.data();
	if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
		printf("SQL %s\n", mysql_error(&ceshi));
		system("pause");
		return;
	}
}


void PC_Group_New(string DB_path, map<int, vector<string>> &group_Sign, map<int, vector<string>> &group_InSign, string NumGroup_str, int Num_InsignBits, int Num_row, int Num_col, string lamda_str,vector<vector<int>>& Grouped) {
	int NumGroup = atoi(NumGroup_str.c_str());
	fstream Read_ptr;
	Read_ptr.open(DB_path);
	string tuple_str;
	getline(Read_ptr, tuple_str);
	for (int i = 1; i <= Num_row; i++) {
		tuple_str.clear();
		getline(Read_ptr, tuple_str);
		bool flag = 0;
		string temp_str;
		vector<string> vec;
		int col = 0;
		for (int j = 0; j<tuple_str.size(); j++) {
			//cout<<i<<' ';
			if (tuple_str[j] == '"'&&flag == 0) {
				flag = 1;
			}
			else if (tuple_str[j] == '1' || tuple_str[j] == '0') {
				temp_str += tuple_str[j];
			}
			else if (tuple_str[j] == '"'&&flag == 1) {
				vec.push_back(temp_str);
				temp_str.clear();
				flag = 0;
				col++;
			}
			if (col == Num_col) {
				break;
			}
		}
		for (int j = 1; j <= Num_col; j++) {
			string seed_str = lamda_str + to_string(j) + to_string(i); //ID+attr+lamda
			string hash_value = Hash(seed_str, seed_str.size());
			stringstream ss;
			int Rout;
			ss << hash_value.substr(0, 4);
			ss >> hex >> Rout;
			int mod_res = Rout%NumGroup;

			string Sign_str = vec[j-1].substr(0, vec[j - 1].size() - Num_InsignBits);
			string InSign_str = vec[j - 1].substr(vec[j - 1].size() - Num_InsignBits, Num_InsignBits);
			group_Sign[j][mod_res]+=Sign_str;
			group_InSign[j][mod_res]+=InSign_str;
			Grouped[i - 1][j - 1] = mod_res;
		}
	}
	Read_ptr.close();
}



void PC_Commit_New(map<int, vector<string>> group_Sign, map<int,vector<string>> &Comm, vector<string> &d_vec, string N_str, string g_str, string h_str, int Num_Group, int Num_InsignBits, int Num_col) {
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
	for (int col = 1; col <= Num_col; col++) {
		mpz_t d_t;
		mpz_init(d_t);
		pbc_mpz_random(d_t, N);
		element_t d;
		element_init_Zr(d, pairing);
		element_set_mpz(d, d_t);
		element_pow_zn(gPowd, g, d);
		for (int i = 0; i < Num_Group; i++) {
			string msg_str = group_Sign[col][i];
			if (msg_str.size() == 0) {
				continue;
			}
			//cout << msg_str.size() << endl;
			string Hout = Hash(msg_str, msg_str.size());
			//cout << Hout << endl;
			const char* msg_ptr = Hout.data();
			element_t m;
			element_init_Zr(m, pairing);
			element_set_str(m, msg_ptr, 16);
			//element_printf("m %B\n",m);
			element_t hPowm, comm;
			element_init_G1(hPowm, pairing);
			element_init_G1(comm, pairing);
			element_pow_zn(hPowm, h, m);
			element_mul(comm, gPowd, hPowm);
			char comm_char[MSGSIZE];
			element_snprint(comm_char, MSGSIZE, comm);
			string comm_str = ExtraG1(comm_char);
			Comm[col][i] = comm_str;
		}
		string d_str = mpz_get_str(NULL, 10, d_t);
		d_vec.push_back(d_str);
	}
}


void Fuzzy_Commit_New(vector<string> grouped_InSign, vector<vector<string>> &Comm, int Num_Group) {
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

	for (int i = 0; i < Num_Group; i++) {
		//cout << i << endl;
		string msg_str = grouped_InSign[i];
		//cout << group_f[i].size() << endl;
		if (msg_str.size() == 0) {
			vector<string> c;
			Comm.push_back(c);
			continue;
		}
		vector<string> c = EncGM(n, R, msg_str);
		Comm.push_back(c);
	}
}
void FingerPrinting(string FingerPrint,string sk,string DB_path, string output_path,vector<vector<int>> Grouped, int Num_group, int Num_row, int Num_col, int Num_InsignBits, int Max_ModifyBits) {
	int MOD = Num_row*Num_InsignBits / Num_group/ Max_ModifyBits;
	vector<vector<int>> modifed(Num_group, vector<int>(Num_col));
	for (int i = 0; i < Num_group; i++) {
		for (int j = 0; j < Num_col; j++) {
			modifed[i][j] = 0;
		}
	}
	fstream Read_ptr;
	Read_ptr.open(DB_path);
	string tuple_str;
	getline(Read_ptr, tuple_str);
	fstream write_ptr;
	write_ptr.open(output_path);
	write_ptr << tuple_str << endl;
	for (int i = 1; i <= Num_row; i++) {
		tuple_str.clear();
		getline(Read_ptr, tuple_str);
		bool flag = 0;
		int col = 0;
		string temp_str;
		vector<string> vec;
		for (int j = 0; j<tuple_str.size(); j++) {
			//cout<<i<<' ';
			if (tuple_str[j] == '"'&&flag == 0) {
				flag = 1;
			}
			else if (tuple_str[j] == '1' || tuple_str[j] == '0') {
				temp_str += tuple_str[j];
			}
			else if (tuple_str[j] == '"'&&flag == 1) {
				vec.push_back(temp_str);
				temp_str.clear();
				flag = 0;
				col++;
			}
			if (col == Num_col) {
				break;
			}
		}
		for (int j = 1; j <= Num_col; j++) {
			for (int k = 0; k < Num_InsignBits; k++) {
				string seed_str = sk + to_string(i) + to_string(j) + to_string(k);
				string hash_value = Hash(seed_str+"1", seed_str.size()+1);
				stringstream ss;
				int Rout;
				ss << hash_value.substr(0, 4);
				ss >> hex >> Rout;
				int mod_res = Rout%MOD;
				int group_ord = Grouped[i - 1][j - 1];
				int Num_existModify = modifed[group_ord][j - 1];
				if (mod_res == 0&& Num_existModify<Max_ModifyBits) {//插入指纹
					cout << group_ord << ' ' << j << endl;
					modifed[group_ord][j - 1]++;
					string hash_x = Hash(seed_str + "2", seed_str.size() + 1);
					int x;
					ss << hash_x.substr(0, 4);
					ss >> hex >> x;
					x = x % 2;
					string hash_l = Hash(seed_str + "3", seed_str.size() + 1);
					int l;
					ss << hash_l.substr(0, 4);
					ss >> hex >> l;
					l = l % FingerPrint.size();
					char f = FingerPrint[l];
					char B;
					if ((x == 1 && f == '1') || (x == 0 && f == '0')) {
						B = '0';
					}
					else {
						B = '1';
					}
					char r = vec[j - 1][vec[j - 1].size() - Num_InsignBits + k];
					if (r == B) {
						vec[j - 1][vec[j - 1].size() - Num_InsignBits + k] = '0';
					}
					else {
						vec[j - 1][vec[j - 1].size() - Num_InsignBits + k] = '1';
					}
				}
			}
		}
		char b1 = '"';
		char b2 = ' ';
		for (int j = 0; j < Num_col; j++) {
			write_ptr << b1 << vec[j] << b1 << b2;
		}
		write_ptr << endl;
	}
	Read_ptr.close();
	write_ptr.close();
}



int main(int argc, const char* argv[]) {
	


	
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

	const int parame_length = xxx;
	string sk = "xxxxx";
	string N_str, g_str;
	DB_Con_Init();
	Soceket_init();
	string select_sql = "SELECT * FROM cloud_parameters;";
	DB_Query(select_sql);
	if (nextRow == NULL) {
		Param_Init(parame_length);
		DB_Query(select_sql);
	}
	N_str = nextRow[2];
	g_str = nextRow[3];


	string DB_name = "DB1";
	select_sql.clear();
	select_sql = "SELECT * FROM db_info WHERE DB='" + DB_name + "';";
	DB_Query(select_sql);
	string Num_row = nextRow[1];
	string Num_col = nextRow[2];
	fstream read_userInfo;
	string userInfo_Path = "xxxx";
	read_userInfo.open(userInfo_Path);
	string Num_InsignBits_str;
	string Num_group_str;
	string lamda;
	getline(read_userInfo, Num_InsignBits_str);
	getline(read_userInfo, Num_group_str);
	getline(read_userInfo, lamda);
	int Num_InsignBits = atoi(Num_InsignBits_str.c_str());
	map<int, vector<string>> group_Sign, group_InSign,Comm;
	for (int i = 1; i < atoi(Num_col.c_str()); i++) {
		vector<string> temp(atoi(Num_group_str.c_str()));
		group_Sign[i] = temp;
		group_InSign[i] = temp;
		Comm[i] = temp;
	}
	vector<vector<int>> Grouped(atoi(Num_row.c_str()), vector<int>(atoi(Num_col.c_str())));
	

	string DB_path = "xxxx";
	PC_Group_New(DB_path, group_Sign, group_InSign, Num_group_str, Num_InsignBits, atoi(Num_row.c_str()), atoi(Num_col.c_str()) - 1, lamda, Grouped);
	
	fstream code_write;
	code_write.open("xxx");

	for (int i = 1; i <= atoi(Num_col.c_str()) - 1; i++) {

		string MaskCode_path = "xxxxx";
		string MulAndShuft_path = "xxxx";
		
		fstream read_ptr,read_ptr_new;
		read_ptr.open(MaskCode_path);
		read_ptr_new.open(MulAndShuft_path);
		vector<string> ciph;

		for (int j = 0; j < atoi(Num_row.c_str())*Num_InsignBits; j++) {
			string temp;
			getline(read_ptr_new, temp);
			ciph.push_back(temp);
		}
		
		string deccode = DecGM(p, q, R, n, ciph);
		string res_code;
		int begin = 0;
		for (int j = 0; j < atoi(Num_group_str.c_str()); j++) {
			string maskcode;
			getline(read_ptr, maskcode);
			
			string org_Insignbits = group_InSign[i][j];
			string deccode_groupj = deccode.substr(begin, org_Insignbits.size());
			begin = begin + org_Insignbits.size();
			for (int k = 0; k < org_Insignbits.size(); k++) {
				if (maskcode[k] == org_Insignbits[k]) {
					maskcode[k] = deccode_groupj[k];
				}
				else {
					if (deccode_groupj[k] == '0') {
						maskcode[k] = '1';
					}
					else {
						maskcode[k] = '0';
					}
				}
			}
			unsigned char x_code[255],xx[249];
			binaryStringToUnsignedCharArray(maskcode, x_code);
			int fl = 0;
			rsdecode(x_code, xx, &fl);
			string binary_str = unsignedCharArrayToBinaryString(xx, 249);
			res_code += binary_str.substr(0, org_Insignbits.size());
		}
		
		code_write << res_code << endl;
		
		read_ptr.close();
		
	}

	
	code_write.close();
	system("pause");
	return 0;


}
