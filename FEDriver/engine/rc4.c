//#include "stdafx.h"
#include"rc4.h"

//程序开始
unsigned char rc4Key[256]; 
/* 初始化函数 */
void rc4_init(unsigned char *key, unsigned long keylong)
{
	int i = 0, j = 0;
	char k[256] = {0};
	unsigned char tmp = 0;
	unsigned char s[256];
	for(i = 0; i < 256; i++)
	{
		s[i] = i;
		k[i] = key[i % keylong];
	}
	for (i = 0; i < 256; i++)
	{
		j = (j + s[i] + k[i]) % 256;
		tmp = s[i];
		s[i] = s[j]; //交换s[i]和s[j]
		s[j] = tmp;
	}
	for(i = 0; i < 256; i++) //用s2[i]暂时保留经过初始化的s[i]，很重要的！！！
	{
		key[i] = s[i];
	}
}

/* 加解密 */
void rc4_Encrypt(unsigned char *plaintext, int textlong)
{
	int i = 0, j = 0, t = 0;
	unsigned long k = 0;
	unsigned char tmp;
	unsigned char *key=rc4Key;
	unsigned char s[256] = {0}; //S-box

	for(i=0;i<256;i++) s[i]=rc4Key[i];

	for(k = 0; k < textlong; k++)
	{
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		tmp = s[i];
		s[i] = s[j]; //交换s[x]和s[y]
		s[j] = tmp;
		t = (s[i] + s[j]) % 256;
		plaintext[k] ^= s[t];
	}
}

void rc4_Decrypt(unsigned char *ciphertext, int textlong)
{
	int i = 0, j = 0, t = 0;
	unsigned long k = 0;
	unsigned char tmp;
	unsigned char *key=rc4Key;

	for(k = 0; k < textlong; k++)
	{
		i = (i + 1) % 256;
		j = (j + key[i]) % 256;
		tmp = key[i];
		key[i] = key[j]; //交换key[x]和key[y]
		key[j] = tmp;
		t = (key[i] + key[j]) % 256;
		ciphertext[k] ^= key[t];
	}
}

void setRc4Key(unsigned char * key)
{
	int i;
	for(i=0;i<256;i++) rc4Key[i]=key[i];
	rc4_init(rc4Key, strlen((const char *)key)); //已经完成了初始化
}

// int main()
// {
// 	char key[256] = {"dfewfewfwef"};
// 	char pData[512] = "这是一个用来加密的数据Data";
// 
// 	int i;
// 	setRc4Key((unsigned char *)key);
// 
// 	printf("已经初始化，现在加密:\n\n");
// 	rc4_Encrypt((unsigned char *)pData, 15); //加密
// 	printf("pData = %s\n\n", pData);
// 	printf("已经加密，现在解密:\n\n");
// 
// 	rc4_Decrypt((unsigned char *)pData, 15); //解密
// 	printf("pData = %s\n\n", pData);
// 	getchar();
// 	return 0;
// }

//程序完