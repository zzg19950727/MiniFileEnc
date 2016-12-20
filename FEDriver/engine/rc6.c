#include "rc6.h"
 
UINT ROTL(UINT x, UINT y)
{
   return (((x)<<(y&(32-1))) | ((x)>>(32-(y&(32-1)))));
}

UINT ROTR(UINT x, UINT y)
{
	return (((x)>>(y&(32-1))) | ((x)<<(32-(y&(32-1)))));
}

void setRc6Key(PUCHAR  K)
{
    INT b=32;
	INT c=((b + 3) / 4);
	INT i, j, s, v;
    UINT L[(32 + 4 - 1) / 4]; /* Big enough for max b */
    UINT A, B;

    L[c - 1] = 0;
    for (i = b - 1; i >= 0; i--)
        L[i / 4] = (L[i / 4] << 8) + K[i];

    rc6Key[0] = 0xB7E15163;
    for (i = 1; i <= 2 * 20 + 3; i++)
        rc6Key[i] = rc6Key[i - 1] + 0x9E3779B9;

    A = B = i = j = 0;
    v = 44;
    if (c > v) v = c;
    v *= 3;

    for (s = 1; s <= v; s++)
    {
        A = rc6Key[i] = ROTL(rc6Key[i] + A + B, 3);
        B = L[j] = ROTL(L[j] + A + B, A + B);
        i = (i + 1) % 44;
        j = (j + 1) % c;
    }
}

void rc6_Encrypt16(PUCHAR  plaintext)
{
	PUINT pt;
	UINT A, B, C, D, t, u, x;
    INT i;

	pt = (PUINT )plaintext;

    A = pt[0];
    B = pt[1];
    C = pt[2];
    D = pt[3];
    B += rc6Key[0];
    D += rc6Key[1];
    for (i = 2; i <= 2 * 20; i += 2)
    {
        t = ROTL(B * (2 * B + 1), 5);
        u = ROTL(D * (2 * D + 1), 5);
        A = ROTL(A ^ t, u) + rc6Key[i];
        C = ROTL(C ^ u, t) + rc6Key[i + 1];
        x = A;
        A = B;
        B = C;
        C = D;
        D = x;
    }
    A += rc6Key[2 * 20 + 2];
    C += rc6Key[2 * 20 + 3];
    pt[0] = A;
    pt[1] = B;
    pt[2] = C;
    pt[3] = D;
}

void rc6_Decrypt16(PUCHAR ciphertext)
{
    
	PUINT ct;
	UINT A, B, C, D, t, u, x;
    INT i;

	ct=(PUINT )ciphertext;
    A = ct[0];
    B = ct[1];
    C = ct[2];
    D = ct[3];
    C -= rc6Key[2 * 20 + 3];
    A -= rc6Key[2 * 20 + 2];
    for (i = 2 * 20; i >= 2; i -= 2)
    {
        x = D;
        D = C;
        C = B;
        B = A;
        A = x;
        u = ROTL(D * (2 * D + 1), 5);
        t = ROTL(B * (2 * B + 1), 5);
        C = ROTR(C - rc6Key[i + 1], t) ^ u;
        A = ROTR(A - rc6Key[i], u) ^ t;
    }
    D -= rc6Key[1];
    B -= rc6Key[0];
    ct[0] = A;
    ct[1] = B;
    ct[2] = C;
    ct[3] = D;
}

INT rc6_Encrypt(PUCHAR  plaintext, INT textlong)
{
	INT i,turn,templong;
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
			rc6_Encrypt16(temptext);
			memmove(plaintext+i*16, temptext, 16);
			memmove(temptext, emptytext, 16);
		}
		else
		{
			memmove(temptext, plaintext+i*16, templong);
			rc6_Encrypt16(temptext);
			memmove(plaintext+i*16, temptext, 16);
			memmove(temptext, emptytext, 16);
		}
	}
	pi =(INT *)(plaintext + i*16);
	pi[0]=textlong;
	return 16*(turn + 2);
}
INT rc6_Decrypt(PUCHAR  ciphertext, INT textlong)
{
	INT turn,i,mark;
	UCHAR temptext[17]="";
    UCHAR emptytext[17]="";

	turn=textlong/16 - 2;
	mark=((INT*)(ciphertext+textlong-16))[0];
	for (i=0;i<=turn;i++) 
	{
		memmove(temptext, ciphertext+i*16, 16);
		rc6_Decrypt16(temptext);
		memmove(ciphertext+i*16, temptext, strlen((char *)temptext));
		memmove(temptext, emptytext, 16);
	}
	ciphertext[mark]='\0';
	return mark;
	
}

//INT main()
//{
//	UCHAR tData[1024]="abcdefgpwefewfwfweefwsfewbfewfewfewfewgwewfwefewveregregerregfreger";
//	UCHAR key[32]="abcdefg";
//	INT record;
//
//	setRc6Key(key);
//	record=rc6_Encrypt(tData,33);
//	printf("ÃÜÎÄ: %s\n",tData);
///*	printf("%d\n",strlen((char *)tData));*/
//	rc6_Decrypt(tData, record);
//	printf("Ã÷ÎÄ: %s\n",tData);
//
//    getchar();
//    return 0;
//    
//}