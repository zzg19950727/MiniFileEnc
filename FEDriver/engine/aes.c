// aes-demo.cpp : 定义控制台应用程序的入口点。
//
#include "stdio.h"
#include "aes.h"

UCHAR aesKey[32];
UCHAR block1[256]; //!< Workspace 1.
UCHAR block2[256]; //!< Worksapce 2.
UCHAR tempbuf[256];

PUCHAR powTbl; //!< Final location of exponentiation lookup table.
PUCHAR logTbl; //!< Final location of logarithm lookup table.
PUCHAR sBox; //!< Final location of s-box.
PUCHAR sBoxInv; //!< Final location of inverse s-box.
PUCHAR expandedKey; //!< Final location of expanded key.

void CalcPowLog(PUCHAR powTbl, PUCHAR logTbl)
{
	UCHAR i = 0;
	UCHAR t = 1;
	
	do {
		// Use 0x03 as root for exponentiation and logarithms.
		powTbl[i] = t;
		logTbl[t] = i;
		i++;
		
		// Muliply t by 3 in GF(2^8).
		t ^= (t << 1) ^ (t & 0x80 ? 0x1b : 0);
	}while( t != 1 ); // Cyclic properties ensure that i < 255.
	
	powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}

void CalcSBox( PUCHAR  sBox )
{
	UCHAR i, rot;
	UCHAR temp;
	UCHAR result;
	
	// Fill all entries of sBox[].
	i = 0;
	do {
		//Inverse in GF(2^8).
		if( i > 0 ) 
		{
			temp = powTbl[ 255 - logTbl[i] ];
		} 
		else 
		{
			temp = 0;
		}
		
		// Affine transformation in GF(2).
		result = temp ^ 0x63; // Start with adding a vector in GF(2).
		for( rot = 0; rot < 4; rot++ )
		{
			// Rotate left.
			temp = (temp<<1) | (temp>>7);
			
			// Add rotated byte in GF(2).
			result ^= temp;
		}
		
		// Put result in table.
		sBox[i] = result;
	} while( ++i != 0 );
}

void CalcSBoxInv( PUCHAR  sBox, PUCHAR  sBoxInv )
{
	UCHAR i = 0;
	UCHAR j = 0;
	
	// Iterate through all elements in sBoxInv using  i.
	do {
	// Search through sBox using j.
		do {
			// Check if current j is the inverse of current i.
			if( sBox[ j ] == i )
			{
				// If so, set sBoxInc and indicate search finished.
				sBoxInv[ i ] = j;
				j = 255;
			}
		} while( ++j != 0 );
	} while( ++i != 0 );
}

void CycleLeft( PUCHAR  row )
{
	// Cycle 4 bytes in an array left once.
	UCHAR temp = row[0];
	
	row[0] = row[1];
	row[1] = row[2];
	row[2] = row[3];
	row[3] = temp;
}

void InvMixColumn( PUCHAR  column )
{
	UCHAR r0, r1, r2, r3;
	
	r0 = column[1] ^ column[2] ^ column[3];
	r1 = column[0] ^ column[2] ^ column[3];
	r2 = column[0] ^ column[1] ^ column[3];
	r3 = column[0] ^ column[1] ^ column[2];
	
	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? 0x1b : 0);
	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? 0x1b : 0);
	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? 0x1b : 0);
	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? 0x1b : 0);
	
	r0 ^= column[0] ^ column[1];
	r1 ^= column[1] ^ column[2];
	r2 ^= column[2] ^ column[3];
	r3 ^= column[0] ^ column[3];
	
	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? 0x1b : 0);
	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? 0x1b : 0);
	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? 0x1b : 0);
	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? 0x1b : 0);
	
	r0 ^= column[0] ^ column[2];
	r1 ^= column[1] ^ column[3];
	r2 ^= column[0] ^ column[2];
	r3 ^= column[1] ^ column[3];
	
	column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? 0x1b : 0);
	column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? 0x1b : 0);
	column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? 0x1b : 0);
	column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? 0x1b : 0);
	
	column[0] ^= column[1] ^ column[2] ^ column[3];
	r0 ^= column[0];
	r1 ^= column[0];
	r2 ^= column[0];
	r3 ^= column[0];
	
	column[0] = r0;
	column[1] = r1;
	column[2] = r2;
	column[3] = r3;
}

void SubBytes( PUCHAR  bytes, UCHAR count )
{
	do {
		*bytes = sBox[ *bytes ]; // Substitute every byte in state.
		bytes++;
	} while( --count );
}

void InvSubBytesAndXOR( PUCHAR  bytes, PUCHAR  key, UCHAR count )
{
	do {
		// *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
		*bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
		bytes++;
		key++;
	} while( --count );
}

void InvShiftRows( PUCHAR  state )
{
	UCHAR temp;
	
	// Note: State is arranged column by column.
	
	// Cycle second row right one time.
	temp = state[ 1 + 3*4 ];
	state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
	state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
	state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
	state[ 1 + 0*4 ] = temp;
	
	// Cycle third row right two times.
	temp = state[ 2 + 0*4 ];
	state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
	state[ 2 + 2*4 ] = temp;
	temp = state[ 2 + 1*4 ];
	state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
	state[ 2 + 3*4 ] = temp;
	
	// Cycle fourth row right three times, ie. left once.
	temp = state[ 3 + 0*4 ];
	state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
	state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
	state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
	state[ 3 + 3*4 ] = temp;
}

void InvMixColumns( PUCHAR  state )
{
	InvMixColumn( state + 0*4 );
	InvMixColumn( state + 1*4 );
	InvMixColumn( state + 2*4 );
	InvMixColumn( state + 3*4 );
}

void XORBytes( PUCHAR  bytes1, PUCHAR  bytes2, UCHAR count )
{
	do {
		*bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
		bytes1++;
		bytes2++;
	} while( --count );
}

void CopyBytes( PUCHAR  to, PUCHAR  from, UCHAR count )
{
	do {
		*to = *from;
		to++;
		from++;
	} while( --count );
}

void KeyExpansion( PUCHAR  expandedKey )
{
	UCHAR temp[4];
	UCHAR i;
	UCHAR Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.
	
	PUCHAR  key = aesKey;
	
	// Copy key to start of expanded key.
	i = 32;
	do {
		*expandedKey = *key;
		expandedKey++;
		key++;
	} while( --i );
	
	// Prepare last 4 bytes of key in temp.
	expandedKey -= 4;
	temp[0] = *(expandedKey++);
	temp[1] = *(expandedKey++);
	temp[2] = *(expandedKey++);
	temp[3] = *(expandedKey++);
	
	// Expand key.
	i = 32;
	while( i < 16*(14+1) ) 
	{
		// Are we at the start of a multiple of the key size?
		if( (i % 32) == 0 )
		{
			CycleLeft( temp ); // Cycle left once.
			SubBytes( temp, 4 ); // Substitute each byte.
			XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).
			*Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? 0x1b : 0);
		}
		
		// Keysize larger than 24 bytes, ie. larger that 192 bits?
		#if 32 > 24
		// Are we right past a block size?
		else if( (i % 32) == 16 ) {
		SubBytes( temp, 4 ); // Substitute each byte.
		}
		#endif
		
		// Add bytes in GF(2) one 32 away.
		XORBytes( temp, expandedKey - 32, 4 );
		
		// Copy result to current 4 bytes.
		*(expandedKey++) = temp[ 0 ];
		*(expandedKey++) = temp[ 1 ];
		*(expandedKey++) = temp[ 2 ];
		*(expandedKey++) = temp[ 3 ];
		
		i += 4; // Next 4 bytes.
	}
}

void InvCipher( PUCHAR  block, PUCHAR  expandedKey )
{
	UCHAR round = 14-1;
	expandedKey += 16 * 14;
	
	XORBytes( block, expandedKey, 16 );
	expandedKey -= 16;
	
	do {
		InvShiftRows( block );
		InvSubBytesAndXOR( block, expandedKey, 16 );
		expandedKey -= 16;
		InvMixColumns( block );
	} while( --round );
	
	InvShiftRows( block );
	InvSubBytesAndXOR( block, expandedKey, 16 );
}

void aesDecInit(void)
{
	powTbl = block1;
	logTbl = block2;
	CalcPowLog( powTbl, logTbl );
	
	sBox = tempbuf;
	CalcSBox( sBox );
	
	expandedKey = block1;
	KeyExpansion( expandedKey );
	
	sBoxInv = block2; // Must be block2.
	CalcSBoxInv( sBox, sBoxInv );
}

UCHAR Multiply( UCHAR num, UCHAR factor )
{
	UCHAR mask = 1;
	UCHAR result = 0;
	
	while( mask != 0 ) 
	{
	// Check bit of factor given by mask.
		if( mask & factor ) 
		{
		  // Add current multiple of num in GF(2).
		  result ^= num;
		}
	
		// Shift mask to indicate next bit.
		mask <<= 1;
		
		// Double num.
		num = (num << 1) ^ (num & 0x80 ? 0x1b : 0);
	}
	
	return result;
}

UCHAR DotProduct( PUCHAR  vector1, PUCHAR  vector2 )
{
	UCHAR result = 0;
	
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1++, *vector2++ );
	result ^= Multiply( *vector1  , *vector2   );
	
	return result;
}

void MixColumn( PUCHAR  column )
{
	UCHAR row[8] = {0x02, 0x03, 0x01, 0x01, 0x02, 0x03, 0x01, 0x01}; 
	// Prepare first row of matrix twice, to eliminate need for cycling.
	
	UCHAR result[4];
	
	// Take dot products of each matrix row and the column vector.
	result[0] = DotProduct( row+0, column );
	result[1] = DotProduct( row+3, column );
	result[2] = DotProduct( row+2, column );
	result[3] = DotProduct( row+1, column );
	
	// Copy temporary result to original column.
	column[0] = result[0];
	column[1] = result[1];
	column[2] = result[2];
	column[3] = result[3];
}

void MixColumns( PUCHAR  state )
{
	MixColumn( state + 0*4 );
	MixColumn( state + 1*4 );
	MixColumn( state + 2*4 );
	MixColumn( state + 3*4 );
}

void ShiftRows( PUCHAR  state )
{
	UCHAR temp;
	
	// Note: State is arranged column by column.
	
	// Cycle second row left one time.
	temp = state[ 1 + 0*4 ];
	state[ 1 + 0*4 ] = state[ 1 + 1*4 ];
	state[ 1 + 1*4 ] = state[ 1 + 2*4 ];
	state[ 1 + 2*4 ] = state[ 1 + 3*4 ];
	state[ 1 + 3*4 ] = temp;
	
	// Cycle third row left two times.
	temp = state[ 2 + 0*4 ];
	state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
	state[ 2 + 2*4 ] = temp;
	temp = state[ 2 + 1*4 ];
	state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
	state[ 2 + 3*4 ] = temp;
	
	// Cycle fourth row left three times, ie. right once.
	temp = state[ 3 + 3*4 ];
	state[ 3 + 3*4 ] = state[ 3 + 2*4 ];
	state[ 3 + 2*4 ] = state[ 3 + 1*4 ];
	state[ 3 + 1*4 ] = state[ 3 + 0*4 ];
	state[ 3 + 0*4 ] = temp;
}

void Cipher( PUCHAR  block, PUCHAR  expandedKey )
{
	UCHAR round = 14-1;
	
	XORBytes( block, expandedKey, 16 );
	expandedKey += 16;
	
	do {
		SubBytes( block, 16 );
		ShiftRows( block );
		MixColumns( block );
		XORBytes( block, expandedKey, 16 );
		expandedKey += 16;
	} while( --round );
	
	SubBytes( block, 16 );
	ShiftRows( block );
	XORBytes( block, expandedKey, 16 );
}

void aesEncInit(void)
{
	powTbl = block1;
	logTbl = tempbuf;
	CalcPowLog( powTbl, logTbl );
	
	sBox = block2;
	CalcSBox( sBox );
	
	expandedKey = block1;
	KeyExpansion( expandedKey );
}

void aes_Encrypt16( PUCHAR  plaintext)
{
	UCHAR cipherText[1024];
	memset(cipherText,0x00,sizeof(cipherText));
	aesEncInit();
	XORBytes( cipherText, plaintext, 16 );
	Cipher( cipherText, expandedKey );
	CopyBytes( plaintext, cipherText, 16 );
}

void aes_Decrypt16( PUCHAR  ciphertext)
{
	UCHAR temp[ 16 ];
	UCHAR plainText[1024];
	memset(plainText,0x00,sizeof(plainText));
	aesDecInit();
	CopyBytes( temp, ciphertext, 16 );
	InvCipher(ciphertext, expandedKey );
	XORBytes( plainText, ciphertext, 16 );
	CopyBytes( ciphertext, plainText, 16 );
}

INT aes_Encrypt( PUCHAR  plaintext, INT textlong)
{
	INT turn,i,templong;
	UCHAR temptext[17]="";
	UCHAR emptytext[17]="";
	INT *pi;

	turn=(textlong+15)/16-1;
	templong=textlong;
	for (i=0;i<=turn;i++) 
	{
		if (templong>16) 
		{
			templong=templong-16;
		     memmove(temptext, plaintext+i*16, 16);
		     aes_Encrypt16(temptext);
			 memmove(plaintext+i*16, temptext, 16);
			 memmove(temptext, emptytext, 16);
		}
		else
		{
			memmove(temptext, plaintext+i*16, templong);
			aes_Encrypt16(temptext);
			memmove(plaintext+i*16, temptext, 16);
			memmove(temptext, emptytext, 16);
         }
	}
	pi =(INT *)(plaintext + i*16);
	pi[0]=textlong;
	return 16*(turn + 2);
}

INT aes_Decrypt( PUCHAR  ciphertext, INT textlong)
{
	INT turn,i;
	UCHAR temptext[17]="";
	INT *pi;

	if (textlong % 16 !=0) return 0; 
	turn=textlong/16 - 2;
	for (i=0;i<=turn;i++) 
	{
		    memmove(temptext, ciphertext+i*16, 16);
			aes_Decrypt16(temptext);
			memmove(ciphertext+i*16, temptext, 16);
	}
	pi =(INT *)(ciphertext + textlong-16);
	/*for (i=pi[0];i<textlong;i++)
	{
	   ciphertext[i]=0;
	}*/
	return pi[0];
}

void setAesKey(PUCHAR  key)
{
	INT i;
	for(i=0;i<32;i++) aesKey[i]=key[i];
}

//INT main()
//{
//	UCHAR dat[1024]="fwefweofweoowefpwefpwfwepfpwfwpwpfwepfewpfpwepfwepfewpfpewfpew";
//	UCHAR key[1024]="12213213212341241234231423187692";
//	INT record;
//	
//// 	INT key2[16]={-87,35,-66,-124,-31,108,-42,-82,82,-112,73,-15,-15,-69,-23,-21};
//// 
//// 	for (INT i=0;i<16;i++)
//// 	{
////        key[i]=key2[i];
//// 	}
//	setAesKey(key);
//	record=aes_Encrypt(dat,39/*strlen((char *)dat)*/);//AES加密，数组dat里面的新内容就是加密后的数据。
//
//	
//    printf("After Encrypt:\n");
//	printf("%s\n%d\n",dat,record);
//
//    setAesKey(key);
//	record=aes_Decrypt(dat,record/*strlen((char *)dat)*/);
//    printf("After Decrypt:\n");
//	printf("%s\n%d\n",dat,record);
//
//	getchar();
//	return 0;
//}




