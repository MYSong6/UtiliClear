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
#include <sstream>

using namespace std;

static unsigned char g[6] =
{
  117, 49, 58, 158, 4, 126};

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
  44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1};
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
79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175};

unsigned char
gfmul (unsigned char mul1,unsigned char mul2){
  unsigned char mul3;
  if (mul1 == 0 || mul2 == 0)
    mul3 = 0;
  else
    mul3 = e2v[(v2e[mul1] + v2e[mul2]) % 255];
  return (mul3);
}


/* Add two field elements.  Subtraction and addition are equivalent */

unsigned char
gfadd (unsigned char add1, unsigned char add2){
  unsigned char add3;
  add3 = add1 ^ add2;
  return (add3);
}


/* Invert a field element, for division */

unsigned char gfinv (unsigned char ivt){
  unsigned char ivtd;
  if (ivt == 0)
    exit (42);
  ivtd = e2v[255 - v2e[ivt]];
  return (ivtd);
}


/* Exponentiation.  Convert to exponential notation, mod 255 */

unsigned char
gfexp (unsigned char mant, unsigned char powr){
  unsigned char expt;
  if (mant == 0)
    expt = 0;
  else
    expt = e2v[(v2e[mant] * powr) % 255];
  return (expt);
}


void rsencode (unsigned char m[249], unsigned char c[255]){

  unsigned char r[6], rtmp;

  int i, j;

  for (i = 0; i < 6; i++)
    r[i] = 0;

  for (i = 0; i < 249; i++)
    {
      c[254 - i] = m[i];
      rtmp = gfadd (m[i], r[5]);
      for (j = 5; j > 0; j--)
    {
      r[j] = gfadd (gfmul (rtmp, g[j]), r[j - 1]);
    }
      r[0] = gfmul (rtmp, g[0]);
    }
  for (i = 0; i < 6; i++)
    {
      c[i] = r[i];
    }
}


/* Polynomial Evaluator, used to determine the Syndrome Vector.  This is
   relatively straightforward, and there are faster algorithms.
*/

unsigned char evalpoly (unsigned char p[255], unsigned char x){

  unsigned char y;
  int i;
  y = 0;

  for (i = 0; i < 255; i++)
    {
      y = gfadd (y, gfmul (p[i], gfexp (x, i)));
    }
  return (y);

}


/* Determine the Syndrome Vector.  Note that in s[0] we return the OR of
   all of the syndromes; this allows for an easy check for the no - error
   condition.
*/

void syndrome (unsigned char c[255], unsigned char s[7]){

  extern unsigned char e2v[256];
  int i;
  s[0] = 0;
  for (i = 1; i < 7; i++)
    {
      s[i] = evalpoly (c, e2v[i]);
      s[0] = s[0] | s[i];
    }

}


/* Determine the number of errors in a block.  Since we have to find the
   determinant of the S[] matrix in order to determine singularity, we
   also return the determinant to be used by the Cramer's Rule correction
   algorithm.
*/

void errnum ( unsigned char s[7],  unsigned char* det, int* errs){


  *det = gfmul (s[2], gfmul (s[4], s[6]));
  *det = gfadd (*det, gfmul (s[2], gfmul (s[5], s[5])));
  *det = gfadd (*det, gfmul (s[6], gfmul (s[3], s[3])));
  *det = gfadd (*det, gfmul (s[4], gfmul (s[4], s[4])));
  *errs = 3;

  if (*det != 0)
    return;

  *det = gfadd (gfmul (s[2], s[4]), gfexp (s[3], 2));
  *errs = 2;
  if (*det != 0)
    return;

  *det = s[1];
  *errs = 1;
  if (*det != 0)
    return;

  *errs = 4;

}

void polysolve ( unsigned char polynom[4], unsigned char roots[3], int* numsol){
  extern unsigned char e2v[256];
  int i, j;
  unsigned char y;

  *numsol = 0;

  for (i = 0; i < 255; i++)
    {
      y = 0;
      for (j = 0; j < 4; j++)
    y = gfadd (y, gfmul (polynom[j], gfexp (e2v[i], j)));
      if (y == 0)
    {
      roots[*numsol] = e2v[i];
      *numsol = *numsol + 1;
    }
    }
}


/* Full impementation of the three error correcting Peterson decoder.  For
   t<6, it is faster than Massey - Berlekamp.  It is also somewhat more
   intuitive.
*/


void rsdecode (unsigned char code[255], unsigned char mesg[249], int* errcode){

  extern unsigned char v2e[256];
  unsigned char syn[7], deter, z[4], e0, e1, e2, n0, n1, n2, w0, w1, w2,
    x0, x[3];
  int i, sols;

  *errcode = 0;

  /* First, get the message out of the code, so that even if we can't correct
       it, we return an estimate.
    */

  for (i = 0; i < 249; i++)
    mesg[i] = code[254 - i];

  syndrome (code, syn);

  if (syn[0] == 0)
    return;

  /* We now know we have at least one error.  If there are no errors detected,
       we assume that something funny is going on, and so return with errcode 4,
       else pass the number of errors back via errcode.
    */

  errnum (syn, &deter, errcode);

  if (*errcode == 4)
    return;

  /* Having obtained the syndrome, the number of errors, and the determinant,
       we now proceed to correct the block.  If we do not find exactly the
       number of solutions equal to the number of errors, we have exceeded our
       error capacity, and return with the block uncorrected, and errcode 4.
    */

  switch (*errcode)
    {

    case 1:

      x0 = gfmul (syn[2], gfinv (syn[1]));
      w0 = gfmul (gfexp (syn[1], 2), gfinv (syn[2]));
      if (v2e[x0] > 5)
    mesg[254 - v2e[x0]] = gfadd (mesg[254 - v2e[x0]], w0);
      return;

    case 2:

      z[0] = gfmul (gfadd (gfmul (syn[1], syn[3]), gfexp (syn[2], 2)), gfinv (deter));
      z[1] = gfmul (gfadd (gfmul (syn[2], syn[3]), gfmul (syn[1], syn[4])), gfinv (deter));
      z[2] = 1;
      z[3] = 0;

      polysolve (z, x, &sols);
      if (sols != 2)
    {
      *errcode = 4;
      return;
    }

      w0 = gfmul (z[0], syn[1]);
      w1 = gfadd (gfmul (z[0], syn[2]), gfmul (z[1], syn[1]));
      n0 = 254 - v2e[gfinv (x[0])];
      n1 = 254 - v2e[gfinv (x[1])];
      e0 = gfmul (gfadd (w0, gfmul (w1, x[0])), gfinv (z[1]));
      e1 = gfmul (gfadd (w0, gfmul (w1, x[1])), gfinv (z[1]));

      if (n0 < 249)
    mesg[n0] = gfadd (mesg[n0], e0);
      if (n1 < 249)
    mesg[n1] = gfadd (mesg[n1], e1);
      return;

    case 3:

      z[3] = 1;
      z[2] = gfmul (syn[1], gfmul (syn[4], syn[6]));
      z[2] = gfadd (z[2], gfmul (syn[1], gfmul (syn[5], syn[5])));
      z[2] = gfadd (z[2], gfmul (syn[5], gfmul (syn[3], syn[3])));
      z[2] = gfadd (z[2], gfmul (syn[3], gfmul (syn[4], syn[4])));
      z[2] = gfadd (z[2], gfmul (syn[2], gfmul (syn[5], syn[4])));
      z[2] = gfadd (z[2], gfmul (syn[2], gfmul (syn[3], syn[6])));
      z[2] = gfmul (z[2], gfinv (deter));

      z[1] = gfmul (syn[1], gfmul (syn[3], syn[6]));
      z[1] = gfadd (z[1], gfmul (syn[1], gfmul (syn[5], syn[4])));
      z[1] = gfadd (z[1], gfmul (syn[4], gfmul (syn[3], syn[3])));
      z[1] = gfadd (z[1], gfmul (syn[2], gfmul (syn[4], syn[4])));
      z[1] = gfadd (z[1], gfmul (syn[2], gfmul (syn[3], syn[5])));
      z[1] = gfadd (z[1], gfmul (syn[2], gfmul (syn[2], syn[6])));
      z[1] = gfmul (z[1], gfinv (deter));

      z[0] = gfmul (syn[2], gfmul (syn[3], syn[4]));
      z[0] = gfadd (z[0], gfmul (syn[3], gfmul (syn[2], syn[4])));
      z[0] = gfadd (z[0], gfmul (syn[3], gfmul (syn[5], syn[1])));
      z[0] = gfadd (z[0], gfmul (syn[4], gfmul (syn[4], syn[1])));
      z[0] = gfadd (z[0], gfmul (syn[3], gfmul (syn[3], syn[3])));
      z[0] = gfadd (z[0], gfmul (syn[2], gfmul (syn[2], syn[5])));
      z[0] = gfmul (z[0], gfinv (deter));

      polysolve (z, x, &sols);
      if (sols != 3){
          *errcode = 4;
          return;
      }

      w0 = gfmul (z[0], syn[1]);
      w1 = gfadd (gfmul (z[0], syn[2]), gfmul (z[1], syn[1]));
      w2 = gfadd (gfmul (z[0], syn[3]), gfadd (gfmul (z[1], syn[2]), gfmul (z[2], syn[1])));

      n0 = 254 - v2e[gfinv (x[0])];
      n1 = 254 - v2e[gfinv (x[1])];
      n2 = 254 - v2e[gfinv (x[2])];

      e0 = gfadd (w0, gfadd (gfmul (w1, x[0]), gfmul (w2, gfexp (x[0], 2))));
      e0 = gfmul (e0, gfinv (gfadd (z[1], gfexp (x[0], 2))));
      e1 = gfadd (w0, gfadd (gfmul (w1, x[1]), gfmul (w2, gfexp (x[1], 2))));
      e1 = gfmul (e1, gfinv (gfadd (z[1], gfexp (x[1], 2))));
      e2 = gfadd (w0, gfadd (gfmul (w1, x[2]), gfmul (w2, gfexp (x[2], 2))));
      e2 = gfmul (e2, gfinv (gfadd (z[1], gfexp (x[2], 2))));

      if (n0 < 249)
    mesg[n0] = gfadd (mesg[n0], e0);
      if (n1 < 249)
    mesg[n1] = gfadd (mesg[n1], e1);
      if (n2 < 249)
    mesg[n2] = gfadd (mesg[n2], e2);

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
    //cout<<tuple_str.size()<<endl;
    for(int row=0;row<NumRow;row++){
        tuple_str.clear();
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
            else if(tuple_str[i]!=' '){
                cout<<row<<' '<<i<<endl;
            }
            if(col==NumCol){
                break;
            }
        }
        
    }
    Read_ptr.close();
    return res;
}
string Hash(string msg, int size) {
    char * DataChar = new char[size + 1];
    for (int j = 0; j<size; j++) {
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
    for (int i = 0; i<64; i++) {
        TempHash += temp2[i];
    }
    string HashValue16 = binaryToHex(TempHash);
    return HashValue16;
}

void binaryStringToUnsignedCharArray(const std::string& binaryString, unsigned char* arr) {
    size_t length = binaryString.length();
    for (size_t i = 0; i<length / 8; ++i) {
        std::string byteString = binaryString.substr(i * 8, 8);
        arr[i] = std::bitset<8>(byteString).to_ulong();
    }
}
string unsignedCharArrayToBinaryString(const unsigned char* arr, size_t size) {
    std::string binaryString = "";
    for (size_t i = 0; i<size; ++i) {
        binaryString += std::bitset<8>(arr[i]).to_string();
    }
    return binaryString;
}

string output(vector<char> data, string x){
    
    unsigned char xx[249], x_coded[255];
    int orNum = x.size();
    for (int i = orNum; i<1992; i++) {
        x += '0';
    }
    binaryStringToUnsignedCharArray(x, xx);
    rsencode(xx,x_coded);
    string x_code_str = unsignedCharArrayToBinaryString(x_coded, 255);
    return x_code_str;

}
string Mask_Code(vector<char> data, string x) {
    string res;
    unsigned char xx[249], x_coded[255];
    int orNum = x.size();
    for (int i = orNum; i<1992; i++) {
        x += '0';
    }
    binaryStringToUnsignedCharArray(x, xx);
    rsencode(xx,x_coded);
    string x_code_str = unsignedCharArrayToBinaryString(x_coded, 255);

    for (int i = 0; i<data.size(); i++) {
        if (data[i] != x_code_str[i]) {
            res += '1';
        }
        else {
            res += '0';
        }
    }
    for(int i=data.size();i<x_code_str.size();i++){
        res+=x_code_str[i];
    }
    return res;
}

int main(int argc, const char * argv[]) {
    
   
    
    const int Num_group = xx;
    const int Num_InsignBits = xx;
    const int Num_MaxModifyBits = xx; //each 50 bits can be modified 3 bits
    const int NumCol=xx;
    const int NumRow = xx;
    const int ShuftSeed = xx;
    
    string DB_path = "xxx";
    
    vector<vector<string>> DB = Read_TXTDB(DB_path,NumRow,NumCol);
    
    string Par_path = "xxx";
    fstream Read_ptr;
    string N_str, g_str,GMN_str,GMR_str,h_str,lamda_str;
    Read_ptr.open(Par_path);
    getline(Read_ptr,N_str);
    getline(Read_ptr,g_str);
    getline(Read_ptr,GMN_str);
    getline(Read_ptr,GMR_str);
    getline(Read_ptr,h_str);
    getline(Read_ptr,lamda_str);
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
    
    element_t g,h;
    const char* g_ch = g_str.data();
    const char* h_ch = h_str.data();
    element_init_G1(g,pairing);
    element_set_str(g,g_ch,10);
    element_init_G1(h,pairing);
    element_set_str(h,h_ch,10);

   
    
    fstream write_code;
    string code_path = "xxxx";
    write_code.open(code_path,ios::out);
    
    for(int i=1;i<=NumCol;i++){
        startTime = clock();
        
        string code_str;
    
        bool flag = 1;
        vector<string> Grouped(Num_group);
        vector<string> Grouped_InSign(Num_group);
        for(int j=1;j<=NumRow;j++){
            string seed_str = lamda_str+to_string(i)+to_string(j);
            int Rout;
            stringstream ss;
            string hash = Hash(seed_str,seed_str.size());
            ss<<hash.substr(0,4);
            ss>>hex>>Rout;
            int mod_res = Rout%Num_group;
            
            Grouped[mod_res]+=DB[i-1][j-1].substr(0,DB[i-1][j-1].size()-Num_InsignBits);
            Grouped_InSign[mod_res]+=DB[i-1][j-1].substr(DB[i-1][j-1].size()-Num_InsignBits,Num_InsignBits);
            
        }
       
        string PC_path ="xxxxx";
        Read_ptr.open(PC_path);
        string d_str;
        getline(Read_ptr,d_str);
        element_t d,gpowd,hpowm,comm,m,org_pcc;
        element_init_Zr(d,pairing);
        element_init_Zr(m,pairing);
        element_init_G1(gpowd,pairing);
        element_init_G1(hpowm,pairing);
        element_init_G1(comm,pairing);
        element_init_G1(org_pcc,pairing);
        
        const char* d_ptr = d_str.data();
        mpz_t d_t;
        mpz_init(d_t);
        mpz_init_set_str(d_t,d_ptr,10);
        element_set_mpz(d,d_t);
        element_pow_zn(gpowd,g,d);
        
        
        for(int j=0;j<Num_group;j++){
            string PCC_str;
            getline(Read_ptr,PCC_str);
            if(Grouped[j].size()==0){
                continue;
            }
            string Hout = Hash(Grouped[j],Grouped[j].size());
            
            const char* Hout_ptr =Hout.data();
            const char* pcc_ptr =PCC_str.data();
            element_set_str(m,Hout_ptr,16);
            element_pow_zn(hpowm,h,m);
            
            element_mul(comm,gpowd,hpowm);
            
            element_set_str(org_pcc,pcc_ptr,10);
            
            if(element_cmp(org_pcc,comm)){
                flag = 0;
                break;
            }
        }
        Read_ptr.close();
        
        
        if(flag==1){
            cout<<"Attr_"<<i<<" SignBits PC Proof Susseed!"<<endl;
        }
        else{
            cout<<"Attr_"<<i<<" SignBits PC Proof Failed!"<<endl;
        }
        
        
        string Len_path = "xxx";
        string Len_str,rand_str;
        Read_ptr.open(Len_path);
        
        string Random_str_path = "xxx";
        fstream Read_ptr1;
        Read_ptr1.open(Random_str_path);
        
        string MaskCode_str_path = "xxx";
        fstream write_ptr;
        write_ptr.open(MaskCode_str_path,ios::out);
        
        int begin = 0;
        for(int j=0;j<Num_group;j++){
            Len_str.clear();
            rand_str.clear();
            getline(Read_ptr,Len_str);
            getline(Read_ptr1,rand_str);
            
           
            if(atoi(Len_str.c_str())!=Grouped_InSign[j].size()){
                cout<<"Attr_"<<i<<" InSignBits PC Proof Failed!"<<endl;
                break;
            }
            vector<char> res;
            for(int k=0;k<atoi(Len_str.c_str());k++){
                if(rand_str[k]==Grouped_InSign[j][k]){
                    res.push_back('0');
                }
                else{
                    res.push_back('1');
                }
            }
            mt19937_64 eng(ShuftSeed);
            shuffle(res.begin(), res.end(), eng);
            string groupj_code = generate_random_binary_string(atoi(Len_str.c_str()));
            string masked_code = Mask_Code(res,groupj_code);
            begin = begin +atoi(Len_str.c_str());
            
            write_ptr<<masked_code<<endl;
            code_str+=groupj_code;
           
            
        }
        write_ptr.close();
        Read_ptr.close();
        Read_ptr1.close();
        
       
        write_code<<code_str<<endl;
        
        
    }
    write_code.close();
   
    return 0;
}
