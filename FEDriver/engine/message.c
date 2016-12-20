#include "message.h"
#include "..\include\iocommon.h"
#include "..\include\interface.h"
#include "process.h"
#include "key.h"
#include <wdm.h>
#include <stdlib.h>

PFLT_PORT g_pClientPort = NULL ;
PFLT_FILTER gFilterHandle;
FileHeader gInfo={0};
PFLT_PORT g_pServerPort = NULL ; // port for communication between user and kernel
extern UCHAR CryptIndex;

void WriteFile_Port(IN UNICODE_STRING uFileName,IN FileHeader* pstructInf )
{
	HANDLE hFile=NULL;
	IO_STATUS_BLOCK	ioStatus;
	NTSTATUS	ntStatus;
	OBJECT_ATTRIBUTES object_attributes;

	//UNICODE_STRING uFileName=RTL_CONSTANT_STRING(L"\\??\\C:\\port.txt");

	DbgPrint("WriteLog");

	InitializeObjectAttributes(
		&object_attributes,
		&uFileName,
		OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE,
		NULL,
		NULL);

	ntStatus=ZwCreateFile(
		&hFile,
		GENERIC_READ|GENERIC_WRITE,
		&object_attributes,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE|FILE_RANDOM_ACCESS|FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (ntStatus==STATUS_SUCCESS)
	{	
		//PVOID buffer;
		ULONG Length = sizeof(FileHeader);
		ULONG dwPort;
		ANSI_STRING AnsiString1;
		UNICODE_STRING UnicodeString1;


		//buffer = ExAllocatePool(NonPagedPool, 10);
		/*ntStatus=ZwReadFile(
		hFile,
		NULL,
		NULL,
		NULL,
		&ioStatus,
		&(pstructInf->filehead),
		Length,
		NULL,
		NULL);*/
		/*ZwWriteFile(
			hFile,
			NULL,
			NULL,
			NULL,
			&ioStatus,
			&(pstructInf->filehead),
			Length,
			NULL,
			NULL);
			0);*/
		//DbgPrint("%s",buffer);

		//将buffer转换成ULONG
		/*RtlInitAnsiString(&AnsiString1,buffer);	
		RtlAnsiStringToUnicodeString(&UnicodeString1,&AnsiString1,TRUE);
		RtlUnicodeStringToInteger(&UnicodeString1,10,&dwPort)*/;

		//DbgPrint("%d",dwPort);
	} 
	else
	{
		DbgPrint("Open file error");
	}

	ZwClose(hFile);
}

void ReadFile_Port(IN UNICODE_STRING uFileName,OUT MSG_GET_ADD_PROCESS_INFO* pstructInf )
{
	HANDLE hFile=NULL;
	IO_STATUS_BLOCK	ioStatus;
	NTSTATUS	ntStatus;
	OBJECT_ATTRIBUTES object_attributes;

	//UNICODE_STRING uFileName=RTL_CONSTANT_STRING(L"\\??\\C:\\port.txt");

	DbgPrint("ReadLog");

	InitializeObjectAttributes(
		&object_attributes,
		&uFileName,
		OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE,
		NULL,
		NULL);

	ntStatus=ZwCreateFile(
		&hFile,
		GENERIC_READ|GENERIC_WRITE,
		&object_attributes,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE|FILE_RANDOM_ACCESS|FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0);
	if (ntStatus==STATUS_SUCCESS)
	{	
		//PVOID buffer;
		ULONG Length = sizeof(FileHeader);
		ULONG dwPort;
		ANSI_STRING AnsiString1;
		UNICODE_STRING UnicodeString1;

		//buffer = ExAllocatePool(NonPagedPool, 10);
		ntStatus=ZwReadFile(
			hFile,
			NULL,
			NULL,
			NULL,
			&ioStatus,
			&(pstructInf->filehead),
			Length,
			NULL,
			NULL);
		//DbgPrint("%s",buffer);

		//将buffer转换成ULONG
		/*RtlInitAnsiString(&AnsiString1,buffer);	
		RtlAnsiStringToUnicodeString(&UnicodeString1,&AnsiString1,TRUE);
		RtlUnicodeStringToInteger(&UnicodeString1,10,&dwPort)*/;

		//DbgPrint("%d",dwPort);
	} 
	else
	{
		DbgPrint("Open file error");
	}

	ZwClose(hFile);
}

NTSTATUS 
Msg_CreateCommunicationPort(
	  IN PFLT_FILTER pFilter
	  )
{
	NTSTATUS status ;

	UNICODE_STRING uPortName ;
	OBJECT_ATTRIBUTES ob ;

	PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL ;

	/*
	    Build security descriptor
	//*/
	status = FltBuildDefaultSecurityDescriptor(&pSecurityDescriptor, FLT_PORT_ALL_ACCESS) ;
	if (!NT_SUCCESS(status))
	{
		return status ;
	}

	/*
	    Init server port name
	//*/
	RtlInitUnicodeString(&uPortName, SERVER_PORTNAME) ;
	InitializeObjectAttributes(&ob, 
		&uPortName, 
		OBJ_CASE_INSENSITIVE|OBJ_KERNEL_HANDLE, 
		NULL,
		pSecurityDescriptor) ;

	/* 
	    Create a new server port
	//*/
	status = FltCreateCommunicationPort(
		pFilter, 
		&g_pServerPort,
		&ob,
		NULL,
		Msg_ConnectNotifyCallback, 
		Msg_DisconnectNotifyCallback,
		Msg_MessageNotifyCallback,
		1
		) ;

	/*
	    Free security descriptor
	//*/
	FltFreeSecurityDescriptor(pSecurityDescriptor) ;

	return status ;
}

NTSTATUS
Msg_CloseCommunicationPort(
	  IN PFLT_PORT ServerPort
	  ) 
{
	FltCloseCommunicationPort(ServerPort) ;

	return STATUS_SUCCESS ;
}

NTSTATUS
Msg_ConnectNotifyCallback(
	  IN PFLT_PORT ClientPort,
	  IN PVOID ServerPortCookie,
	  IN PVOID ConnectionContext,
	  IN ULONG SizeOfContext,
	  OUT PVOID* ConnectionPortCookie
	  )
{
	PAGED_CODE();

	UNREFERENCED_PARAMETER( ServerPortCookie );
	UNREFERENCED_PARAMETER( ConnectionContext );
	UNREFERENCED_PARAMETER( SizeOfContext);
	UNREFERENCED_PARAMETER( ConnectionPortCookie);

	g_pClientPort = ClientPort ;

	return STATUS_SUCCESS ;
}

VOID
Msg_DisconnectNotifyCallback(
	  IN PVOID ConnectionCookie
	  )
{
	PAGED_CODE() ;

	UNREFERENCED_PARAMETER(ConnectionCookie);

	FltCloseClientPort(gFilterHandle, &g_pClientPort) ;
}

NTSTATUS
Msg_MessageNotifyCallback(
	  IN PVOID PortCookie,
	  IN PVOID InputBuffer,
	  IN ULONG InputBufferLength,
	  OUT PVOID OutputBuffer,
	  IN ULONG OutputBufferLength,
	  OUT PULONG ReturnOutputBufferLength
	  )
{
	NTSTATUS status = STATUS_SUCCESS ;
	PMSG_SEND_TYPE psSendType= (PMSG_SEND_TYPE)InputBuffer ;
	ULONG uType  = psSendType->uSendType ;
	PMSG_SEND_SET_PROCESS_INFO psSendSetProcInfo = (PMSG_SEND_SET_PROCESS_INFO)InputBuffer ;
	char buffer[] = "to user client";//
	ANSI_STRING str2;
	UNICODE_STRING uFileName;

	PAGED_CODE();

	UNREFERENCED_PARAMETER(PortCookie);

	////打印用户发来的信息
	//DbgPrint("用户发来的信息是:%s\n",InputBuffer);

	////返回用户一些信息.
	//*ReturnOutputBufferLength = sizeof(buffer);
	//RtlCopyMemory(OutputBuffer,buffer,* ReturnOutputBufferLength);


	switch (uType)
	{
	case IOCTL_GET_PROCESS_COUNT:
		{
			if (InputBufferLength < sizeof(MSG_SEND_TYPE))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			Ps_GetAllProcessInfo(NULL, OutputBuffer) ;
			*ReturnOutputBufferLength = OutputBufferLength ;
			break ;
		}
	case IOCTL_GET_ALL_PROCESS_INFO:
		{
			ULONG uCount = 0 ;
			
			if (InputBufferLength < sizeof(MSG_SEND_TYPE))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			
			Ps_GetAllProcessInfo(OutputBuffer, &uCount) ;
			*ReturnOutputBufferLength = OutputBufferLength ;
			break ;
		}
	case IOCTL_SET_PROCESS_MONITOR:
		{
			if (InputBufferLength < sizeof(MSG_SEND_SET_PROCESS_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			Ps_SetProcessInfo(InputBuffer) ;
			*ReturnOutputBufferLength = 0 ;
			break ;
		}
	case IOCTL_ADD_PROCESS_INFO:
		{
			if (InputBufferLength < sizeof(MSG_SEND_ADD_PROCESS_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ; 
			}
			Ps_AddProcessInfo(InputBuffer, OutputBuffer) ;
			break ;
		}
	case IOCTL_DEL_PROCESS_INFO:
		{
			if (InputBufferLength < sizeof(MSG_SEND_DEL_PROCESS_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			Ps_DelProcessInfo(InputBuffer, OutputBuffer) ;
			break ;
		}
	case IOCTL_SET_FILEKEY_INFO:
		{
			PMSG_SEND_SET_FILEKEY_INFO psSetFileKeyInfo ;
			extern UCHAR g_szCurFileKey[MAX_KEY_LENGTH] ;
			extern UCHAR g_szCurFileKeyDigest[HASH_SIZE] ;
			extern BOOLEAN g_bInitCurKey ;

			if (InputBufferLength < sizeof(MSG_SEND_SET_FILEKEY_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			
			psSetFileKeyInfo = (PMSG_SEND_SET_FILEKEY_INFO)InputBuffer ;
			RtlCopyMemory(g_szCurFileKey, psSetFileKeyInfo->szKey, MAX_KEY_LENGTH) ;
			RtlCopyMemory(g_szCurFileKeyDigest, psSetFileKeyInfo->szKeyDigest, HASH_SIZE) ;
			g_bInitCurKey = TRUE ;
			*ReturnOutputBufferLength = 0 ;
			break ;
		}
	case IOCTL_SET_MONITOR:
		{
			if (InputBufferLength < sizeof(MSG_SEND_SET_PROCESS_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			Ps_SetMonitored(InputBuffer) ;
			*ReturnOutputBufferLength = 0 ;
			break ;
		}
	case IOCTL_GET_MONITOR:
		{
			if (InputBufferLength < sizeof(MSG_SEND_SET_PROCESS_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			Ps_GetMonitorStatus(OutputBuffer) ;
			*ReturnOutputBufferLength = sizeof(MSG_SEND_SET_PROCESS_INFO) ;
			break ;
		}
	case IOCTL_SET_KEYLIST:
		{
			if (InputBufferLength < sizeof(MSG_SEND_SET_HISKEY_INFO))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			Key_InitKeyList(InputBuffer) ;
			*ReturnOutputBufferLength = 0 ;
			break ;
		}
	case IOCTL_GET_STRUCT:
		{
			if (InputBufferLength < sizeof(MSG_SEND_TYPE))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			if(!psSendSetProcInfo->sProcInfo.filehead.isCancel)
			{
				MSG_GET_ADD_PROCESS_INFO getInfo;
				strcpy(getInfo.filehead.szKeyBuf,"get struct ok!");
				//CryptIndex=psSendSetProcInfo->sProcInfo.filehead.iType;
				DbgPrint("用户发来的信息是:%s,%d\n",psSendSetProcInfo->sProcInfo.filehead.szKeyBuf,psSendSetProcInfo->sProcInfo.filehead.iType);

				//str2 = RTL_CONSTANT_STRING(psSendSetProcInfo->sProcInfo.filehead.szPath);
				//uFileName=RTL_CONSTANT_STRING(psSendSetProcInfo->sProcInfo.filehead.szPath);
				//str2.Buffer = buffer;
				//str2.Length = str2.MaximumLength = strlen(psSendSetProcInfo->sProcInfo.filehead.szPath) * sizeof(WCHAR);

				RtlAnsiStringToUnicodeString(&uFileName, &str2 , TRUE);

				//ReadFile_Port(uFileName,&getInfo);
				*ReturnOutputBufferLength = sizeof(getInfo);
				RtlCopyMemory(OutputBuffer,&getInfo,* ReturnOutputBufferLength);
			}
			break ;
		}
	case IOCTL_SET_FILE:
		{
			if (InputBufferLength < sizeof(MSG_SEND_TYPE))
			{
				return STATUS_BUFFER_TOO_SMALL ;
			}
			if(!psSendSetProcInfo->sProcInfo.filehead.isCancel)
			{
				MSG_GET_ADD_PROCESS_INFO getInfo;
				gInfo=psSendSetProcInfo->sProcInfo.filehead;
				strcpy(getInfo.filehead.szKeyBuf,"set struct ok!");
				CryptIndex=psSendSetProcInfo->sProcInfo.filehead.iType;
				DbgPrint("用户发来的信息是:%s,%d\n",psSendSetProcInfo->sProcInfo.filehead.szKeyBuf,psSendSetProcInfo->sProcInfo.filehead.iType);

				/*str2 = RTL_CONSTANT_STRING(psSendSetProcInfo->sProcInfo.filehead.szPath);
				uFileName=RTL_CONSTANT_STRING(psSendSetProcInfo->sProcInfo.filehead.szPath);
				str2.Buffer = buffer;
				str2.Length = str2.MaximumLength = strlen(psSendSetProcInfo->sProcInfo.filehead.szPath) * sizeof(WCHAR);*/

				/*RtlAnsiStringToUnicodeString(&uFileName, &str2 , TRUE);*/

				//WriteFile_Port(uFileName,&(psSendSetProcInfo->sProcInfo.filehead));
				*ReturnOutputBufferLength = sizeof(getInfo);
				RtlCopyMemory(OutputBuffer,&getInfo,* ReturnOutputBufferLength);
			}
			break ;
		}
	default:
		break ;
	}

	return status ;
}