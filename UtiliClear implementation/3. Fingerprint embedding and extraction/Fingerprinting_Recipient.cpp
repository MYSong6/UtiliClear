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

/* Helper function to send message to the Do */
void Info_Send(string s){
    
	char MSG[MSGSIZE];
	append(s, MSG);
	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sClient == -1) {
		cout << "Client initial fails" << endl;
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

	recv(sClient, MSG, MSGSIZE, 0);

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
    
    string lamda = "975";               // Set the parameter zeta in fingerprint
    string ID = "1199967";              // Set the recipient ID
   

    Socket_init();                      // Initial socket and connect to the DO
    

    Info_Send(ID);
    Info_Send(lamda);
    
	system("pause");
    return 0;
}
