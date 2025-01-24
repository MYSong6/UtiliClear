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

using namespace std;

#define DO_ADDRESS "192.168.10.33"      // Replace with the DO's IP address
#define DO_PORT 10248                   // Replace with the DO's port
#define MSGSIZE 2048
#define QUEUE_SIZE 25

WSADATA wsaData;
SOCKET sListen;
SOCKET sClient;
SOCKADDR_IN server;
SOCKADDR client_user;

/* Helper function to extract message from received message */
/* The symbol '!' marks the end of the message */
string InfoExtra(char MSG[1024*2]){
    string res = "";
    for(int i=0;i<MSGSIZE;i++){
        if(MSG[i]=='!'){
            break;
        }
        else{
            res+=MSG[i];
        }
    }
    return res;
}

/* Helper function to append '!' to the message and convert it to string */
void append(string s, char* res){
    for(int i=0;i<s.size();i++){
        res[i] = s[i];
    }
    res[s.size()]='!';
}

//Helper function to convert high-precision numbers to string
string ExtraZr(char phia_char[MSGSIZE]){
    string phia_str;
    for(int i=0;i<MSGSIZE;i++){
        if(phia_char[i]<='9'&&phia_char[i]>='0'){
            phia_str += phia_char[i];
        }
        else{
            break;
        }
    }
    return phia_str;
    
}

//Helper function to convert high-precision numbers (from Group G) to string
string ExtraG1(char fya_char[MSGSIZE]){
    string fya_str;
    for(int i=0;i<MSGSIZE;i++){
        if((fya_char[i]<='9'&&fya_char[i]>='0')||fya_char[i]==']'||fya_char[i]=='['||fya_char[i]==' '||fya_char[i]==','){
            fya_str+=fya_char[i];
        }
        else{
            break;
        }
    }
    return fya_str;
}


/* Helper function to send message to the Do */
void Info_Send(string s){
    
    char MSG[MSGSIZE];
    append(s,MSG);
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"Client initial fails"<<endl;
        return;
    }
	while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
	}

	send(sClient, MSG, MSGSIZE, NULL);

	closesocket(sClient);
    
}

/* Helper function to receive message from the Do */
string Info_Receive(){
	char MSG[MSGSIZE];
	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sClient == -1) {
		cout << "Client initial fails" << endl;
		return 0;
	}
	while (connect(sClient, (sockaddr*)&server, sizeof(SOCKADDR)) == -1) {
	}

    recv(sClient,MSG, MSGSIZE, 0);

	closesocket(sClient);
    string res = InfoExtra(MSG);
    return res;
}

/* Helper function to initial socket */
void Socket_init(){
	WSAStartup(0x0202, &wsaData);
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server, 0, sizeof(SOCKADDR_IN));
	server.sin_family = AF_INET;
	server.sin_port = htons(DO_PORT);
	server.sin_addr.s_addr = inet_addr(DO_ADDRESS);
}

int main(int argc, const char * argv[]) {
    
    const int Num_group = 110000;       // Set the number of group
    const int Num_InsignBits = 2;       // Set the number of insignificant bits
    const int Num_MaxModifyBits = 128;  // Set the maximum number of modified bits in each group
    string lamda = "975";               // Set the parameter zeta in fingerprint
    string ID = "1199967";              // Set the recipient ID
    string Name ="Recipient1";          // Set the recipient name
    string DB_str = "DB1";              // Set the purchased database

    Socket_init();                      // Initial socket and connect to the DO
    
    /* Receive the public parameters from the DO */
    string N_str = Info_Receive();
    string g_str = Info_Receive();
    string h_str = Info_Receive();
    string GMN_str = Info_Receive();
    string GMR_str = Info_Receive();
    
    /* Send the purchased database to the DO */
    Info_Send(DB_str);
    
    
    /* Receive the database information from the DO */
    string Num_row = Info_Receive();
    string Num_col = Info_Receive();
    
    
    
    /* Send the specific parameter to the DO */
    string Num_InsignBits_str = to_string(Num_InsignBits);
    string Num_group_str = to_string(Num_group);
    string Num_MaxModifyBits_str = to_string(Num_MaxModifyBits);
    Info_Send(Num_InsignBits_str);
    Info_Send(Num_group_str);
    Info_Send(Num_MaxModifyBits_str);
    Info_Send(Name);
   
    /* Replace with your path to store parameters */
    string Parameter_path = "C:/Users/62555/Desktop/Paper8Data/Params1.txt";
	fstream write_ptr;
    write_ptr.open(Parameter_path,ios::out);
    write_ptr<<N_str<<endl;
    write_ptr<<g_str<<endl;
    write_ptr<<GMN_str<<endl;
    write_ptr<<GMR_str<<endl;
    write_ptr<<h_str<<endl;
    write_ptr<<lamda<<endl;
    write_ptr<<DB_str<<endl;
    write_ptr.close();
        
	system("pause");
    return 0;
}
