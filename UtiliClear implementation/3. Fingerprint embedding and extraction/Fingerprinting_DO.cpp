
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

# include <math.h>

# pragma comment(lib,"ws2_32.lib")

#include <assert.h>
#define PORT_LOC 10248						// Replace with your available port number
#define MSGSIZE 2048						// The size of message exchanged between the DO and recipient (maximum 2048)
WSADATA wsaData;
SOCKET sListen;
SOCKET sClient;
SOCKADDR_IN local; 
SOCKADDR client_user; 
int nsize;
MYSQL ceshi;                                        
MYSQL_ROW nextRow;
using namespace std;


// Helper function to convert high-precision numbers to string
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

// Helper function to extract message from received message
// The symbol '!' marks the end of the message
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

// Helper function to convert Hex string to binary bit-string
string hexToBinary(const std::string& hexStr) {
	std::string binaryStr = "";
	for (size_t i = 0; i < hexStr.length(); ++i) {
		char hexChar = hexStr[i];
		
		switch (hexChar) {
		case '0': binaryStr += "0000"; break;
		case '1': binaryStr += "0001"; break;
		case '2': binaryStr += "0010"; break;
		case '3': binaryStr += "0011"; break;
		case '4': binaryStr += "0100"; break;
		case '5': binaryStr += "0101"; break;
		case '6': binaryStr += "0110"; break;
		case '7': binaryStr += "0111"; break;
		case '8': binaryStr += "1000"; break;
		case '9': binaryStr += "1001"; break;
		case 'A': case 'a': binaryStr += "1010"; break;
		case 'B': case 'b': binaryStr += "1011"; break;
		case 'C': case 'c': binaryStr += "1100"; break;
		case 'D': case 'd': binaryStr += "1101"; break;
		case 'E': case 'e': binaryStr += "1110"; break;
		case 'F': case 'f': binaryStr += "1111"; break;
		default: break; 
		}
	}
	return binaryStr;
}

// Helper function to convert binary bit-string to Hex string
string binaryToHex(const string& binaryStr) {
	string ret;
	static const char *hex = "0123456789ABCDEF";
	for (auto c : binaryStr) {
		ret.push_back(hex[(c >> 4) & 0xf]);
		ret.push_back(hex[c & 0xf]);
	}
	return ret;
}

// Hash function mapping a message 'msg' to a Hex string
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

//Helper function to initial soceket and wait for the connected request from the recipient
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

// Helper function to send message to the recipient
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

// Helper function to receive message to the recipient
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

//Helper function to connect local MySQL database
//Replace the MySQL connection setup (i.e., host, user, password, database, and port) in mysql_real_connect with your own configuration based on the MySQL installation settings.
void DB_Con_Init() {
	mysql_init(&ceshi);
	if (mysql_real_connect(&ceshi, "localhost", "root", "625558889", "paper8", 3306, NULL, 0)) {
		cout << "MySQL connected." << endl;
	}
}

// Helper function to execute SQL query
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



/* Fingerprint embedding function */
/* 'sk' represents the DO's secret key														*/
/* 'DB_path' respresents the file path of the original database								*/
/* 'output_path' respresents the file path to store the fingerprinted database				*/
/* 'Grouped' matrix stores the group order of each attribute value							*/	
/* 'Num_group'is the number of groups; 'Num_row' is the number of records					*/
/* 'Num_col' is the number of conlums; 'Num_InsignBits' is the number of insignificant bits */
/* 'Max_ModifyBits' is the number of modifiable bits in each group							*/
/*  The fingerprinted database is stored in the path 'output_path'                          */
void FingerPrinting(string FingerPrint,string sk,string DB_path, string output_path,vector<int> Grouped, int Num_group, int Num_row, int Num_col, int Num_InsignBits, int Max_ModifyBits) {
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
	getline(Read_ptr, tuple_str);									// Ignore the titile of the table
	ofstream outFile(output_path);
	outFile << tuple_str << endl;									// Write the titile of the table
	for (int i = 1; i <= Num_row; i++) {
		tuple_str.clear();
		getline(Read_ptr, tuple_str);
		bool flag = 0;
		int col = 0;
		string temp_str;
		vector<string> vec;
		for (int j = 0; j<tuple_str.size(); j++) {
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
				string hash_value = Hash(seed_str, seed_str.size()+1);
				stringstream ss;
				int Rout;
				ss << hash_value.substr(0, 4);
				ss >> hex >> Rout;
				int mod_res = Rout%MOD;
				int group_ord = Grouped[i - 1];
				int Num_existModify = modifed[group_ord][j - 1];
				if (mod_res == 0&& Num_existModify<Max_ModifyBits) {
					modifed[group_ord][j - 1]++;
					
					stringstream sss,ssss;
					int x,ll;
					sss << hash_value.substr(4, 4);
					sss >> hex >> x;
					ssss << hash_value.substr(8, 4);
					ssss >> hex >> ll;
					x = x % 2;
					ll = ll% FingerPrint.size();;
					char f = FingerPrint[ll];
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
		char b2 = '\t';
		for (int j = 0; j < Num_col; j++) {
			outFile << b1 << vec[j] << b1 << b2;
		}
		outFile <<'"'<<to_string(i)<<'"'<< endl;
	}
	Read_ptr.close();
}




int main(int argc, const char* argv[]) {
	
	
	string sk = "MINGYANGSONG";														// Replace with your secret key
	string Recipient = "Recipient1";												// Recipient name
	string DB_name = "DB1";															// Database name
	string DB_path = "D:/DB1_Encode.txt";											// Replace with your path storing the original database
	string output_path = "C:/Users/62555/Desktop/Paper8Data/FDB1_Encode.txt";		// Replace with your path to store the fingerprinted database
	


	/* Retrieve database information and recipient information */
	DB_Con_Init();								
	Soceket_init();
	string ID = Info_Receive();
	string xi = Info_Receive();
	string ID_xi = ID + xi;
	string fingerprint = Hash(ID + xi, ID_xi.size());
	string fingerprint_bits = hexToBinary(fingerprint.substr(0, 32));


	string select_sql = "SELECT * FROM db_info WHERE DB ='" + DB_name + "';";
	DB_Query(select_sql);
	string RowNum_str = nextRow[1];
	string ColNum_str = nextRow[2];
	int num_row = atoi(RowNum_str.c_str());						
	int num_col = atoi(ColNum_str.c_str())-1;								      // Ignore the last column, the primary keys cannot be modified.



	// Update recipient information (i.e., add its fingerprint to the database)
	select_sql.clear();
	select_sql = "SELECT * FROM recipient_parameters WHERE Name ='" + Recipient + "';";
	DB_Query(select_sql);
	string Num_insign = nextRow[1];
	string Num_group = nextRow[2];
	string Num_Modify = nextRow[3];
	string delet_sql = "DELETE * FROM recipient_parameters WHERE Name='" + Recipient + "';";
	string INSERT_SQL_str = "INSERT INTO recipient_parameters values('" + Recipient + "','" + Num_insign + "','" + Num_group + "','" + Num_Modify + "','" + fingerprint_bits + "');";
	const char* INSERT_SQL_char = INSERT_SQL_str.data();
	if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
		printf("SQL insertion fails : %s\n", mysql_error(&ceshi));
		system("pause");
	}

	int num_group = atoi(Num_group.c_str());
	int num_modify = atoi(Num_Modify.c_str());
	int num_insign = atoi(Num_insign.c_str());

	int groupsize = (num_row-1) / num_group;
	
	vector<int> Grouped;
	for (int i = 0; i < num_row; i++) {
		vector<int> temp;
		int ord = i / groupsize;
		if (ord >= num_group) {
			ord--;
		}
		Grouped.push_back(ord);
	}

	/* Fingerprint embedding */
	FingerPrinting(fingerprint_bits, sk, DB_path, output_path, Grouped, num_group, num_row, num_col, num_insign, num_modify);


	system("pause");
	return 0;


}
