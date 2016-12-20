#include "string.h"
#include "windef.h"
#ifndef _AES_DEMO_H_
#define _AES_DEMO_H_
//³ÌÐòÊý¾Ý
// #define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
// #define BLOCKSIZE 16 //!< Block size in number of bytes.
// 
// #define KEYBITS 256 //!< Use AES256.
// #define ROUNDS 14 //!< Number of rounds.
// #define KEYLENGTH 32 //!< Key length in number of bytes.
// 
// #define EXPANDED_KEY_SIZE (BLOCKSIZE * (ROUNDS+1)) //!< 176, 208 or 240 bytes.

// UCHAR aesKey[32]=
// {
// 	0xd0, 0x94, 0x3f, 0x8c, 0x29, 0x76, 0x15, 0xd8,
// 	0x20, 0x40, 0xe3, 0x27, 0x45, 0xd8, 0x48, 0xad,
// 	0xea, 0x8b, 0x2a, 0x73, 0x16, 0xe9, 0xb0, 0x49,
// 	0x45, 0xb3, 0x39, 0x28, 0x0a, 0xc3, 0x28, 0x3c,
// };

  void setAesKey(PUCHAR  key);
  INT aes_Encrypt( PUCHAR  plaintext, INT textlong);
  INT aes_Decrypt( PUCHAR  ciphertext, INT textlong);

	void aes_Encrypt16( PUCHAR  plaintext);
	void aes_Decrypt16( PUCHAR  ciphertext);
	void CalcPowLog(PUCHAR powTbl, PUCHAR logTbl);

	void CalcSBox( PUCHAR  sBox );

	void CalcSBoxInv( PUCHAR  sBox, PUCHAR  sBoxInv );

	void CycleLeft( PUCHAR  row );

	void InvMixColumn( PUCHAR  column );

	void SubBytes( PUCHAR  bytes, UCHAR count );

	void InvSubBytesAndXOR( PUCHAR  bytes, PUCHAR  key, UCHAR count );

	void InvShiftRows( PUCHAR  state );

	void InvMixColumns( PUCHAR  state );

	void XORBytes( PUCHAR  bytes1, PUCHAR  bytes2, UCHAR count );

	void CopyBytes( PUCHAR  to, PUCHAR  from, UCHAR count );

	void KeyExpansion( PUCHAR  expandedKey );

	void InvCipher( PUCHAR  block, PUCHAR  expandedKey );

	void aesDecInit(void);

	UCHAR Multiply( UCHAR num, UCHAR factor );

	UCHAR DotProduct( PUCHAR  vector1, PUCHAR  vector2 );

	void MixColumn( PUCHAR  column );

	void MixColumns( PUCHAR  state );

	void ShiftRows( PUCHAR  state );

	void Cipher( PUCHAR  block, PUCHAR  expandedKey );

  	void aesEncInit(void);


#endif