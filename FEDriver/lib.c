#include "lib.h"
#include "ctx.h"

#include "..\include\interface.h"
//AES aes;
extern FileHeader gInfo;

NTSTATUS
File_EncryptBuffer(
    __in PVOID buffer,
    __in ULONG Length,
    __in PCHAR PassWord,
    __inout PUSHORT CryptIndex,
    __in ULONG ByteOffset
    )
{
    PCHAR buf;
    UINT64 i;
	UINT64 n=0;
	//CHAR key[1024] = {"dfewfewfwef"};
	CHAR key[256] ={0};
	strcpy(key,gInfo.szKeyBuf);
	DbgPrint("File_EncryptBuffer:key=%s\n",key);

    UNREFERENCED_PARAMETER(PassWord);

    buf = (PCHAR)buffer;
	
	/*while(buf[n]!='\0')
	{
		n++;
	}
	DbgPrint("2:len=%d,n=%d,buf=%s\n",Length,n,buf);
	*/
	n = Length;
    ASSERT(NULL != CryptIndex);

    for(i=0;i<n;i++)
	{
		buf[i] += ENC_BUFF_KEY_BIT;
	}
		

    return STATUS_SUCCESS;
}

NTSTATUS
File_DecryptBuffer(
    __in PVOID buffer,
    __in ULONG Length,
    __in PCHAR PassWord,
    __inout PUSHORT CryptIndex,
    __in ULONG ByteOffset
)
{
    PCHAR buf;
    UINT64 i;
	UINT64 n=0;
    CHAR buf2[10];
	CHAR key[1024] ={0};
	strcpy(key,gInfo.szKeyBuf);
	//DbgPrint("File_DecryptBuffer:key=%s\n",key);
	UNREFERENCED_PARAMETER(PassWord);

    buf = (PCHAR)buffer;
	n = Length;
	//DbgPrint("3:len=%d,buf=%s\n",Length,buf);
	
	for(i=0;i<Length;i++)
	{
			buf[i] -= ENC_BUFF_KEY_BIT;
	}
	
    return STATUS_SUCCESS;
}

