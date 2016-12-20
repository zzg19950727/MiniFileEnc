#ifndef _RC6_H
#define _RC6_H

#include <stdio.h>
#include<string.h>
#include "windef.h"
//程序数据
// #define w 32    /* word为32bits */
// #define r 20    //加密轮数
// #define P32 0xB7E15163
// #define Q32 0x9E3779B9
// 
// #define bytes   (w / 8)                        /* 定义字节*/
// #define c       ((b + bytes - 1) / bytes)      /* 密钥字数 */
// #define R24     (2 * r + 4)
// #define lgw     5                           /* log2(w)  */
// /* 定义逻辑移位操作运算 */
// #define ROTL(x,y) (((x)<<(y&(w-1))) | ((x)>>(w-(y&(w-1)))))
// #define ROTR(x,y) (((x)>>(y&(w-1))) | ((x)<<(w-(y&(w-1)))))
// 
// UINT S[R24 - 1];                    /* 子密钥组 */

void setRc6Key(PUCHAR K);
INT rc6_Encrypt(PUCHAR  plaintext, INT textlong);
INT rc6_Decrypt(PUCHAR  plaintext, INT textlong);


void rc6_Encrypt16(PUCHAR  plaintext);
void rc6_Decrypt16(PUCHAR  ciphertext);
UINT rc6Key[43];                    /* 子密钥组 */
UINT ROTL(UINT x, UINT y);
UINT ROTR(UINT x, UINT y);

#endif