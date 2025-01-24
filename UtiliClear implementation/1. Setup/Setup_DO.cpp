
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



#pragma comment(lib,"ws2_32.lib")
#include <vector>
#include <stdlib.h>
#include <assert.h>
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
#define GMp "232312311797"				//The private parameter p of GM encrytion 
#define GMq "971179711797"				//The private parameter q of GM encryption
#define GMR "17"						//The public parameter xi of GM encryption
#define GMN "225617004017905263169209"  //The public parameter N of GM encryption

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

//Helper function to extract message from received message 
//The symbol '!' marks the end of the message
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

//Helper function to initial soceket and wait for the connected request from the recipient
void Soceket_init() {
	memset(&local, 0, sizeof(local));
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT_LOC);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
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
	MSG[msg.size()] = '!'; //The symbol '!' marks the end of the message
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
		cout << "MySQL Connected." << endl;
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

//Helper function to generate and store parameters of the DO 
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

	element_t g,h;
	element_init_G1(g, pairing);
	element_random(g);
	element_init_G1(h, pairing);
	element_random(h);

	string p_str, q_str, N_str, g_str,h_str;
	N_str = mpz_get_str(NULL, 10, N);
	p_str = mpz_get_str(NULL, 10, p);
	q_str = mpz_get_str(NULL, 10, q);
	char g_char[MSGSIZE], h_char[MSGSIZE];
	element_snprint(g_char, 2 * parame_length, g);
	element_snprint(h_char, 2 * parame_length, h);
	g_str = ExtraG1(g_char);
	h_str = ExtraG1(h_char);

	//Store DO's parameter into the table "do_parameters"
	string INSERT_SQL_str = "INSERT INTO do_parameters values('" + p_str + "','" + q_str + "','" + N_str + "','" + g_str + "','" + h_str + "');";
	const char* INSERT_SQL_char = INSERT_SQL_str.data();
	if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
		printf("SQL insertion fails: %s\n", mysql_error(&ceshi));
		system("pause");
		return;
	}
}


//Helper function to store parameters of the recipient
void Recipient_Parm_Store(string Name,string Num_InsignBits, string Num_Group) {

	string INSERT_SQL_str = "INSERT INTO recipient_parameters values('" + Name + "','" + Num_InsignBits + "','" + Num_Group + "','" + " " + "');";
	const char* INSERT_SQL_char = INSERT_SQL_str.data();
	if (0 != mysql_query(&ceshi, INSERT_SQL_char)) {
		printf("SQL insertion fails : %s\n", mysql_error(&ceshi));
		system("pause");
		return;
	}
}

/* Database onwer Setup */
/* Generate parameters of GM encryption */
/* Generate parameters of Pedersen commitment */
/* Receive parameters from the recipient */
int main(int argc, const char* argv[]) {
	
	
	const int parame_length = 256; //The bit length of parameters.
	string N_str, g_str,h_str;
	DB_Con_Init();
	Soceket_init();
	string select_sql = "SELECT * FROM do_parameters;";
	DB_Query(select_sql);
	if (nextRow == NULL) {	//The first execution of parameter generation
		Param_Init(parame_length);
		DB_Query(select_sql);
	}
	N_str = nextRow[2];
	g_str = nextRow[3];
	h_str = nextRow[4];

	/* Send public parameters */
	Info_Send(N_str);
	Info_Send(g_str);
	Info_Send(h_str);
	string GMN_str = GMN;
	string GMR_str = GMR;
	Info_Send(GMN_str);
	Info_Send(GMR_str);

	/* Receive database name (e.g., DB1, DB2, and DB3) */
	/* Retrieve database information (e.g., the number of columb) */
	string DB_name = Info_Receive();
	select_sql.clear();
	select_sql = "SELECT * FROM db_info WHERE DB='" + DB_name + "';";
	DB_Query(select_sql);
	string Num_row = nextRow[1];
	string Num_col = nextRow[2]; 

	/* Send database information */
	Info_Send(Num_row);
	Info_Send(Num_col);

	/* Receive recipient's parameters */
	string Num_InsignBits_str = Info_Receive();
	string Num_Group_str = Info_Receive();
	string Num_MaxModifyBits_str = Info_Receive();
	string Name= Info_Receive();
	
	/* Store recipient's parameters */
	Recipient_Parm_Store(Name, Num_InsignBits_str, Num_Group_str);
	system("pause");
	return 0;


}