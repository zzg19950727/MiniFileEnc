#include <stdio.h>
#include<string.h>
void setDesKey(unsigned char *key);
int des3_Encrypt(unsigned char *plaintext,int textlong);
int des3_Decrypt(unsigned char *ciphertext, int textlong);

unsigned char desKey[128];
//字节转换成二进制
unsigned char ByteToBit(unsigned char ch, unsigned char bit[8]);

// 二进制转换成字节 
unsigned char BitToByte(unsigned char bit[8],unsigned char *ch);

// 将长度为8的字符串转为二进制位串 
unsigned char Char8ToBit64(unsigned char ch[8],unsigned char bit[64]);

// 将二进制位串转为长度为8的字符串
unsigned char Bit64ToChar8(unsigned char bit[64],unsigned char ch[8]);

// 密钥置换1 
unsigned char DES_PC1_Transform(unsigned char key[64], unsigned char tempbts[56]);

//密钥置换2 
unsigned char DES_PC2_Transform(unsigned char key[56], unsigned char tempbts[48]);

//循环左移 
unsigned char DES_ROL(unsigned char data[56], unsigned char time);

//IP置换
unsigned char DES_IP_Transform(unsigned char data[64]);

// IP逆置换 
unsigned char DES_IP_1_Transform(unsigned char data[64]);

//扩展置换 
unsigned char DES_E_Transform(unsigned char data[48]);

//P置换 
unsigned char DES_P_Transform(unsigned char data[32]);

// 异或 
unsigned char DES_XOR(unsigned char R[48], unsigned char L[48] ,unsigned char count);

// S盒置换 
unsigned char DES_SBOX(unsigned char data[48]);

//交换 
unsigned char DES_Swap(unsigned char left[32], unsigned char right[32]);

//生成子密钥
unsigned char DES_MakeSubKeys(unsigned char key[64],unsigned char subKeys[16][48]);

//加密单个分组 
unsigned char DES_EncryptBlock(unsigned char plainBlock[8], unsigned char subKeys[16][48], unsigned char cipherBlock[8]);

// 解密单个分组
unsigned char DES_DecryptBlock(unsigned char cipherBlock[8], unsigned char subKeys[16][48],unsigned char plainBlock[8]);

//加密文件
unsigned char DES_Encrypt(unsigned char *keyStr,unsigned char *plainFile,unsigned char *cipherFile);

//解密文件 
unsigned char DES_Decrypt(unsigned char *keyStr,unsigned char *cipherFile,unsigned char *plainFile);

