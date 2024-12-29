//
//  main.cpp
//  Client
//
//  Created by 宋明洋 on 2022/1/25.
//
//#ifdef _MSC_VER
//#ifdef _WIN64
//#include <WinSock2.h>
//#elif _WIN32
#include <sys/socket.h>
//#endif
//#endif

#include <fstream>
#include <iostream>
#include <gmpxx.h>
#include <unistd.h>
#include <gmp.h>
#include <stdio.h>
#include <string.h>
#include "/usr/local/include/pbc/pbc.h"
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <ctime>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>



#pragma comment(lib,"ws2_32.lib")
#include <assert.h>


#define GM_PUB_KEY  "gm_pub.key"
#define GM_PRV_KEY  "gm_prv.key"

using namespace std;

string binaryToHex(const string& binaryStr){
    string ret;
    static const char *hex = "0123456789ABCDEF";
    for(auto c:binaryStr){
        ret.push_back(hex[(c>>4)&0xf]);
        ret.push_back(hex[c&0xf]);
    }
    return ret;
}

unsigned char* str2hex(char *str) {
    unsigned char *ret = NULL;
    int str_len = strlen(str);
    int i = 0;
    assert((str_len % 2) == 0);
    ret = (unsigned char *)malloc(str_len / 2);
    for (i = 0; i <str_len; i = i + 2) {
        sscanf(str + i, "%2hhx", &ret[i / 2]);
    }
    return ret;
}

string HexToStr(std::string str){
    std::string hex = str;
    long len = hex.length();
    std::string newString;
    for (long i = 0; i<len; i += 2)
    {
        std::string byte = hex.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    return newString;
}

string unsignedCharToHexString(unsigned char ch) {
    const char hex_chars[] = "0123456789abcdef";
    unsigned int highHalfByte = (ch>> 4) & 0x0f;
    unsigned int lowHalfByte = (ch &0x0f);
    string result;
    result += hex_chars[highHalfByte];
    result += hex_chars[lowHalfByte];
    return result;
}


#define CLOUD_SERVER_ADDRESS "xx.xx.xx.xx"
#define CLOUD_PORT xx
#define MSGSIZE xx
#define QUEUE_SIZE xx
int sClient, sendNo;
struct sockaddr_in addrSer, addrCli;
socklen_t addrlen = sizeof(struct sockaddr);
int parame_length = xx;

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
void append(string s, char* res){
    for(int i=0;i<s.size();i++){
        res[i] = s[i];
    }
    res[s.size()]='!';
}
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
void Info_Send(string s){
    
    char MSG[MSGSIZE];
    append(s,MSG);
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    socklen_t addrlen = sizeof(struct sockaddr);
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    sendNo=send(sClient,MSG, MSGSIZE, 0);
    close(sClient);
    
}

string Info_Receive(){
    char MSG[MSGSIZE];
    sClient = socket(AF_INET,SOCK_STREAM,0);
    if(sClient == -1){
        cout<<"客户端初始化步骤1失败"<<endl;
        return 0;
    }
    bind(sClient,(struct sockaddr*)&addrCli, sizeof(struct sockaddr));
    while( connect(sClient,(struct sockaddr*)&addrSer, addrlen)==-1){
    }
    recv(sClient,MSG, MSGSIZE, 0);
    close(sClient);
    
    string res = InfoExtra(MSG);
    return res;
}

void Socket_init(){
    addrSer.sin_family =AF_INET;
    addrSer.sin_port =  htons(CLOUD_PORT);
    addrSer.sin_addr.s_addr = inet_addr(CLOUD_SERVER_ADDRESS);
}



int main(int argc, const char * argv[]) {
    
    
    
    
    clock_t startTime,endTime;
    const int Num_group = xx;
    const int Num_InsignBits = xx;
    const int Num_MaxModifyBits = xx; //each 50 bits can be modified 3 bits
    string lamda = "xxx";
    string Fingerprint = "xxxxxxxxx";

    
    string DB_str = "DB1";
    
    Socket_init();
    string N_str = Info_Receive();
    string g_str = Info_Receive();
    string GMN_str = Info_Receive();
    string GMR_str = Info_Receive();
    
    mpz_t N;
    mpz_init(N);
    const char* N_char = N_str.data();
    mpz_init_set_str(N,N_char,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    
    element_t g,h;
    const char* g_ch = g_str.data();
    element_init_G1(g,pairing);
    element_init_G1(h,pairing);
    element_set_str(g,g_ch,10);
    element_random(h);
    char h_char[MSGSIZE];
    element_snprint(h_char,2*parame_length,h);
    string h_str = ExtraG1(h_char);
   
    
    
    Info_Send(DB_str);
    string Num_row = Info_Receive();
    string Num_col = Info_Receive();
    
   
    
    string Num_InsignBits_str = to_string(Num_InsignBits);
    string Num_group_str = to_string(Num_group);
    string Num_MaxModifyBits_str = to_string(Num_MaxModifyBits);
    
    Info_Send(h_str);
    Info_Send(Num_InsignBits_str);
    Info_Send(Num_group_str);
    Info_Send(lamda);
    Info_Send(Num_MaxModifyBits_str);
    Info_Send(Fingerprint);
    
    
    string Cloud_Parameter_path_str = "xxxxxx";
    fstream write_ptr;
    write_ptr.open(Cloud_Parameter_path_str,ios::out);
    write_ptr<<N_str<<endl;
    write_ptr<<g_str<<endl;
    write_ptr<<GMN_str<<endl;
    write_ptr<<GMR_str<<endl;
    write_ptr<<h_str<<endl;
    write_ptr<<lamda<<endl;
    write_ptr<<DB_str<<endl;
    write_ptr.close();
        
    
    
    return 0;
}
