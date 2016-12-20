//#include "stdafx.h"
#include<stdio.h>
#include<string.h>

/* 初始化函数 */
void rc4_init(unsigned char *key, unsigned long keylong);

/* 加解密 */
void rc4_Encrypt(unsigned char *plaintext, int textlong);

void rc4_Decrypt(unsigned char *ciphertext, int textlong);

void setRc4Key(unsigned char * key);

