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
#include <algorithm>
#include <random>


using namespace std;


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
int parame_length = 512;

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

vector<string> BuyerSideCommit(vector<string> GMCip,string random_str,mpz_t GMN,mpz_t GMR){
    vector<string> res = EncGM(GMN,GMR,random_str);
    vector<string> c1Mulc2;
    for(int i=0;i<res.size();i++){
        mpz_t c1,c2,c3;
        mpz_init(c1);
        mpz_init(c2);
        mpz_init(c3);
        const char* c1_ptr = GMCip[i].data();
        const char* c2_ptr = res[i].data();
        mpz_init_set_str(c1,c1_ptr,10);
        mpz_init_set_str(c2,c2_ptr,10);
        mpz_mul(c3,c1,c2);
        mpz_mod(c3,c3,GMN);
        string c3_str = mpz_get_str(NULL,10,c3);
        c1Mulc2.push_back(c3_str);
    }
    return c1Mulc2;
}

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

vector<vector<string>> Read_TXTDB(string path, int NumRow, int NumCol){
    vector<vector<string>> res(NumCol, vector<string>(NumRow));
    fstream Read_ptr;
    Read_ptr.open(path);
    string tuple_str;
    getline(Read_ptr,tuple_str);
    for(int row=0;row<NumRow;row++){
        getline(Read_ptr,tuple_str);
        bool flag = 0;
        string temp_str;
        int col = 0;
        for(int i=0;i<tuple_str.size();i++){
            //cout<<i<<' ';
            if(tuple_str[i]=='"'&&flag==0){
                flag = 1;
            }
            else if(tuple_str[i]=='1'||tuple_str[i]=='0'){
                temp_str+=tuple_str[i];
            }
            else if(tuple_str[i]=='"'&&flag==1){
                res[col][row]=temp_str;
                temp_str.clear();
                col++;
                flag = 0;
            }
            if(col==NumCol-1){
                break;
            }
        }
        
    }
    Read_ptr.close();
    return res;
    
}

int main(int argc, const char * argv[]) {
    

    
    string DB_path = "xx";
    vector<vector<string>> DB = Read_TXTDB(DB_path,12960,8);
    //cout<<DB[0].size()<<endl;
    
    
    const int Num_group = xx;
    const int Num_InsignBits = xx;
    const int Num_MaxModifyBits = xx; //each 50 bits can be modified 3 bits
    string lamda = "xx";
    const int NumCol=xx;
    const int ShuftSeed = xx;
    
    
    string Par_path = "xxxx";
    fstream Read_ptr,Read_ptr1;
    string N_str, g_str,GMN_str,GMR_str;
    Read_ptr.open(Par_path);
    getline(Read_ptr,N_str);
    getline(Read_ptr,g_str);
    getline(Read_ptr,GMN_str);
    getline(Read_ptr,GMR_str);
    Read_ptr.close();
    
   
    mpz_t N;
    mpz_init(N);
    const char* N_char = N_str.data();
    mpz_init_set_str(N,N_char,10);
    pbc_param_t param;
    pbc_param_init_a1_gen(param,N);
    pairing_t pairing;
    pairing_init_pbc_param(pairing,param);
    
    const char* GMN_ptr = GMN_str.data();
    const char* GMR_ptr = GMR_str.data();

    mpz_t GMR, GMN;
    mpz_init(GMN);
    mpz_init(GMR);
    mpz_init_set_str(GMN, GMN_ptr, 10);
    mpz_init_set_str(GMR, GMR_ptr, 10);
    
    element_t g;
    const char* g_ch = g_str.data();
    element_init_G1(g,pairing);
    element_set_str(g,g_ch,10);
    
    
    for(int i=1;i<=NumCol;i++){
       
        string Ciph_path = "xxxx";
        string Len_path = "xxxx";
        string Ciph_str;
        string Len_str;
        Read_ptr1.open(Len_path);
        Read_ptr.open(Ciph_path);
        
        string WPath_str = "xxxxx";
        fstream write_ptr;
        write_ptr.open( WPath_str,ios::out);
        
        string Random_str_path = "xxxx";
        fstream write_ptr1;
        write_ptr1.open(Random_str_path,ios::out);
        for(int j=0;j<Num_group;j++){
            
            vector<string> GMCipher;
            getline(Read_ptr1,Len_str);
            for(int k=0;k<atoi(Len_str.c_str());k++){
                getline(Read_ptr,Ciph_str);
                GMCipher.push_back(Ciph_str);
            }
            string randstr = generate_random_binary_string(atoi(Len_str.c_str()));
            vector<string> res = BuyerSideCommit(GMCipher,randstr,GMN,GMR);
            mt19937_64 eng(ShuftSeed);
            shuffle(res.begin(), res.end(), eng);
           
            
           
            for(int k=0;k<atoi(Len_str.c_str());k++){
                write_ptr<<res[k]<<endl;
            }
            write_ptr1<<randstr<<endl;
            
        }
        Read_ptr.close();
        Read_ptr1.close();
        write_ptr.close();
        write_ptr1.close();
        
    }
    
    
    
    return 0;
}
