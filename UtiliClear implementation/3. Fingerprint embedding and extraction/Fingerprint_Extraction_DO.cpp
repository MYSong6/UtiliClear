
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
# include <ctime>
# include <math.h>
#include <assert.h>




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


/* Fingerprint extraction function															*/
/* 'len' represents the bit length of the fingerprint										*/
/* 'sk' represents the DO's secret key														*/
/* 'DB_path' respresents the file path of the original database								*/
/* 'FDB_path' respresents the file path of the fingerprinted database						*/
/* 'Num_group'is the number of groups; 'Num_row' is the number of records					*/
/* 'Num_col' is the number of conlums; 'Num_InsignBits' is the number of insignificant bits */
/* 'Max_ModifyBits' is the number of modifiable bits in each group							*/
/*  The output value is the extracted fingerprint                                           */
string Extract(int len, string sk, string DB_path, string FDB_path,  int Num_group, int Num_row, int Num_col, int Num_InsignBits, int Max_ModifyBits) {
	string fingerprint;
	vector<vector<int>> cf(2, vector<int>(len));
	for (int i = 0; i < len; i++) {
		cf[0][i] = 0;
		cf[1][i] = 0;
	}
	int MOD = Num_row*Num_InsignBits / Num_group / Max_ModifyBits;
	vector<vector<int>> modifed(Num_group, vector<int>(Num_col));
	for (int i = 0; i < Num_group; i++) {
		for (int j = 0; j < Num_col; j++) {
			modifed[i][j] = 0;
		}
	}
	fstream Read_ptr,Read_ptrF;
	Read_ptr.open(DB_path);
	Read_ptrF.open(FDB_path);
	string tuple_str,tuple_strF;
	getline(Read_ptr, tuple_str);
	getline(Read_ptrF, tuple_strF);
	for (int i = 1; i <= Num_row; i++) {
		string tuple_str, tuple_strF;
		getline(Read_ptr, tuple_str);
		getline(Read_ptrF, tuple_strF);
		if (tuple_str.size()==0||tuple_strF.size() == 0) {
			break;
		}
		bool flag = 0;
		int col = 0;
		string temp_str,temp_strF;
		vector<string> vec,vecF;
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
		flag = 0;
		col = 0;
		for (int j = 0; j<tuple_strF.size(); j++) {
			if (tuple_strF[j] == '"'&&flag == 0) {
				flag = 1;
			}
			else if (tuple_strF[j] == '1' || tuple_strF[j] == '0') {
				temp_strF += tuple_strF[j];
			}
			else if (tuple_strF[j] == '"'&&flag == 1) {
				vecF.push_back(temp_strF);
				temp_strF.clear();
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
				string hash_value = Hash(seed_str, seed_str.size() + 1);
				stringstream ss;
				int Rout;
				ss << hash_value.substr(0, 4);
				ss >> hex >> Rout;
				int mod_res = Rout%MOD;
				if (mod_res == 0) {
					
					int x;
					stringstream sss,ssss;
					sss << hash_value.substr(4, 4);
					sss >> hex >> x;
					x = x % 2;
					int l;
					ssss << hash_value.substr(8, 4);
					ssss >> hex >> l;
					
					l = l % len;
					char r = vec[j - 1][vec[j - 1].size() - Num_InsignBits + k];
					char rF = vecF[j - 1][vecF[j - 1].size() - Num_InsignBits + k];
					char B;
					if (r == rF) {
						B = '0';
					}
					else {
						B = '1';
					}
					char fl = '0';
					if ((x == 1 && B == '0') || (x == 0 && B == '1')) {
						cf[1][l]++;
					}
					else {
						cf[0][l]++;
					}

					
				}
			}
		}
	
	}
	Read_ptr.close();
	Read_ptrF.close();
	for (int i = 0; i < len; i++) {
		if (cf[1][i] > cf[0][i]) {
			fingerprint += '1';
		}
		else {
			fingerprint += '0';
		}
	}
	return fingerprint;
}



int main(int argc, const char* argv[]) {
	

	string sk = "MINGYANGSONG";																// Replace with your secret key
	string DB_name = "DB1";																	// Database name
	string Recipient = "Recipient1";														// Recipient name


	string DB_path = "C:/Users/62555/Desktop/Paper8Data/DB1_Encode.txt";					// Replace with your path storing the original database
	string FDB_path = "C:/Users/62555/Desktop/Paper8Data/FDB1_Encode.txt";					// Replace with your path storing the fingerprinted database
	const int finger_len = 128;																// Fingerprint length



	/* Retrieve database information and recipient information */
	DB_Con_Init();
	string select_sql = "SELECT * FROM db_info WHERE DB ='" + DB_name + "';";
	DB_Query(select_sql);
	string RowNum_str = nextRow[1];
	string ColNum_str = nextRow[2];
	int num_row = atoi(RowNum_str.c_str());
	int num_col = atoi(ColNum_str.c_str()) - 1;												// Ignore the last column, the primary keys cannot be modified.


	select_sql.clear();
	select_sql = "SELECT * FROM recipient_parameters WHERE Name ='" + Recipient + "';";
	DB_Query(select_sql);
	string Num_insign = nextRow[1];
	string Num_group = nextRow[2];
	string Num_Modify = nextRow[3];
	string fingerprint = nextRow[4];
	int num_group = atoi(Num_group.c_str());
	int num_modify = atoi(Num_Modify.c_str());
	int num_insign = atoi(Num_insign.c_str());



	/* Fingerprint extraction */
	string fingerprint_E = Extract(finger_len, sk, DB_path, FDB_path, num_group, num_row, num_col, num_insign, num_modify);
    
    int num = 0;
    for(int i=0;i<fingerprint.size();i++){
        if(fingerprint[i]==fingerprint_E[i]){
            num++;
        }
    }
	cout << num << endl;
    cout << "Match ratio:" << double(num)/double(128)<< endl;
    
	system("pause");
	return 0;


}
