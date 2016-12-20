/*++

Copyright (c) 1999-2002  Microsoft Corporation

Module Name:

    scanUser.c

Abstract:

    This file contains the implementation for the main function of the
    user application piece of scanner.  This function is responsible for
    actually scanning file contents.

Environment:

    User mode

--*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winioctl.h>
#include <string.h>
#include <crtdbg.h>
#include <assert.h>
#include <fltuser.h>
#include <dontuse.h>
#include "..\include\interface.h"
#pragma comment(lib, "fltLib.lib")

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
    HANDLE hPort;
   
    HRESULT hr;
   
    ULONG hRes,dwReturned; 

    MSG_SEND_ADD_PROCESS_INFO sSendAddProcInfo;
    MSG_GET_ADD_PROCESS_INFO  sGetAddProcInfo;
	USERCOMMAND_MESSAGE data;
	USERCOMMAND_MESSAGE_REPLAY dataReplay;

	/*char*CStr;
	size_t len;
	size_t converted = 0;
	wchar_t*WStr;*/
	char bufTest[1024];
	int type;

	/*char InBuffer[5] = "test";
	char OutBuffer[MAX_PATH] = {0};
	DWORD bytesReturned = 0;*/

    UNREFERENCED_PARAMETER(argc) ;
    UNREFERENCED_PARAMETER(argv) ;


    //
    //  Open a commuication channel to the filter
    //
	system("sc start FEDriver");
    printf( "Scanner: Connecting to the filter ...\n" );

    hr = FilterConnectCommunicationPort( SERVER_PORTNAME,
                                         0,
                                         NULL,
                                         0,
                                         NULL,
                                         &hPort );

    if (IS_ERROR( hr )) {

        printf( "ERROR: Connecting to filter port: 0x%08x\n", hr );
        return 2;
    }
	printf("请输入密钥:");
	scanf("%s",&bufTest);
	printf("请输入加密方式:");
	scanf("%d",&type);
   
   
    memset(&sSendAddProcInfo, 0, sizeof(sSendAddProcInfo));
    sSendAddProcInfo.sSendType.uSendType = IOCTL_ADD_PROCESS_INFO;
    sSendAddProcInfo.sProcInfo.bMonitor = TRUE;
    RtlCopyMemory(sSendAddProcInfo.sProcInfo.szProcessName, "system", strlen("system"));
    memset(&sGetAddProcInfo, 0, sizeof(MSG_GET_ADD_PROCESS_INFO));
    hRes = FilterSendMessage(hPort, &sSendAddProcInfo, sizeof(MSG_SEND_ADD_PROCESS_INFO), &sGetAddProcInfo, sizeof(MSG_GET_ADD_PROCESS_INFO), &dwReturned);
   
    memset(&sSendAddProcInfo, 0, sizeof(sSendAddProcInfo));
    sSendAddProcInfo.sSendType.uSendType = IOCTL_ADD_PROCESS_INFO;
    sSendAddProcInfo.sProcInfo.bMonitor = TRUE;
    RtlCopyMemory(sSendAddProcInfo.sProcInfo.szProcessName, "explorer.exe", strlen("explorer.exe"));
    memset(&sGetAddProcInfo, 0, sizeof(MSG_GET_ADD_PROCESS_INFO));
    hRes = FilterSendMessage(hPort, &sSendAddProcInfo, sizeof(MSG_SEND_ADD_PROCESS_INFO), &sGetAddProcInfo, sizeof(MSG_GET_ADD_PROCESS_INFO), &dwReturned);

	memset(&sSendAddProcInfo, 0, sizeof(sSendAddProcInfo));
    sSendAddProcInfo.sSendType.uSendType = IOCTL_ADD_PROCESS_INFO;
    sSendAddProcInfo.sProcInfo.bMonitor = TRUE;
    RtlCopyMemory(sSendAddProcInfo.sProcInfo.szProcessName, "notepad.exe", strlen("notepad.exe"));
    memset(&sGetAddProcInfo, 0, sizeof(MSG_GET_ADD_PROCESS_INFO));
    hRes = FilterSendMessage(hPort, &sSendAddProcInfo, sizeof(MSG_SEND_ADD_PROCESS_INFO), &sGetAddProcInfo, sizeof(MSG_GET_ADD_PROCESS_INFO), &dwReturned);

	memset(&sSendAddProcInfo, 0, sizeof(sSendAddProcInfo));
    sSendAddProcInfo.sSendType.uSendType = IOCTL_GET_STRUCT;
    sSendAddProcInfo.sProcInfo.bMonitor = TRUE;
	sSendAddProcInfo.sProcInfo.filehead.iType=type;
	sSendAddProcInfo.sProcInfo.filehead.isCancel=0;
	strcpy(sSendAddProcInfo.sProcInfo.filehead.szKeyBuf,bufTest);
    memset(&sGetAddProcInfo, 0, sizeof(MSG_GET_ADD_PROCESS_INFO));
    hRes = FilterSendMessage(hPort, &sSendAddProcInfo, sizeof(MSG_SEND_ADD_PROCESS_INFO), &sGetAddProcInfo, sizeof(MSG_GET_ADD_PROCESS_INFO), &dwReturned);
	
	memset(&sSendAddProcInfo, 0, sizeof(sSendAddProcInfo));
	sSendAddProcInfo.sSendType.uSendType = IOCTL_SET_FILE;
	sSendAddProcInfo.sProcInfo.bMonitor = TRUE;
	sSendAddProcInfo.sProcInfo.filehead.iType=type;
	sSendAddProcInfo.sProcInfo.filehead.isCancel=0;
	strcpy(sSendAddProcInfo.sProcInfo.filehead.szKeyBuf,bufTest);
	memset(&sGetAddProcInfo, 0, sizeof(MSG_GET_ADD_PROCESS_INFO));
	hRes = FilterSendMessage(hPort, &sSendAddProcInfo, sizeof(MSG_SEND_ADD_PROCESS_INFO), &sGetAddProcInfo, sizeof(MSG_GET_ADD_PROCESS_INFO), &dwReturned);

	if (IS_ERROR( hRes )) {
		OutputDebugString(L"FilterSendMessage fail!\n");
		CloseHandle( hPort );
		return hRes;
	} else {
		OutputDebugString(L"FilterSendMessage ok!\n");
	}
	/*CStr = sGetAddProcInfo.filehead.szKeyBuf;
	len = strlen(CStr) + 1;
	WStr=(wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&converted, WStr, len, CStr, _TRUNCATE);

    OutputDebugString(L"从内核发来的信息是:");
    OutputDebugString(WStr);
    OutputDebugString(L"\n");*/

	printf("从内核发来的信息是:");
	printf("%s\n",sGetAddProcInfo.filehead.szKeyBuf);

	while(TRUE)
	{
		//HRESULT hr= FilterGetMessage(hPort,(PFILTER_MESSAGE_HEADER)data,sizeof(USERCOMMAND_MESSAGE),NULL);
		hr= FilterGetMessage(hPort,(PFILTER_MESSAGE_HEADER)&data,sizeof(USERCOMMAND_MESSAGE),NULL);
		//printf("FilterGetMessage:%s\n",data.cmdMessage.filehead.szKeyBuf);

		/*if (hr==S_OK)
		{
		*/
			//返回数据
			/*printf("请输入密钥:");
			scanf("%s",&bufTest);
			printf("请输入加密方式:");
			scanf("%d",&type);
			*/
			dataReplay.replayHeader.MessageId=data.messageHeader.MessageId;
			dataReplay.cmdMessage.filehead.isCancel=0;
			strcpy(dataReplay.cmdMessage.filehead.szKeyBuf,bufTest);
			dataReplay.cmdMessage.filehead.iType=type;
			//hr=FilterReplyMessage(hPort,(PFILTER_REPLY_HEADER)dataReplay,sizeof(COMMAND_MESSAGE)+sizeof(FILTER_REPLY_HEADER));
			hr=FilterReplyMessage(hPort,(PFILTER_MESSAGE_HEADER)&dataReplay,sizeof(COMMAND_MESSAGE)+sizeof(FILTER_REPLY_HEADER));

		/*}*/
	}

    CloseHandle( hPort );
	//BUFFER buf;
	
    /*hRes = FilterSendMessage(hPort, InBuffer, strlen(InBuffer), OutBuffer, sizeof(OutBuffer), &bytesReturned);
    if (IS_ERROR( hRes )) {
        OutputDebugString(L"FilterSendMessage fail!\n");
        CloseHandle( hPort );
        return hRes;
    } else {
        OutputDebugString(L"FilterSendMessage ok!\n");
    }

    OutputDebugString(L"从内核发来的信息是:");
    OutputDebugString(OutBuffer);
    OutputDebugString(L"\n");

    CloseHandle( hPort );*/



	//USERCOMMAND_MESSAGE data;
 //   USERCOMMAND_MESSAGE_REPLAY dataReplay;

 //   while(TRUE)
 //   {
 //       data->cmdMessage.ulValue=10;
 //       HRESULT hr= FilterGetMessage(g_hPort,(PFILTER_MESSAGE_HEADER)data,sizeof(USERCOMMAND_MESSAGE),NULL);


 //       if (hr==S_OK)
 //       {
 //           CString    str;
 //           str.Format(_T("%d"),data->cmdMessage.ulValue);

 //           dataReplay->replayHeader.MessageId=data->messageHeader.MessageId;

 //           //返回数据
 //           dataReplay->cmdMessage.ulValue=10;
 //           hr=FilterReplyMessage(g_hPort,(PFILTER_REPLY_HEADER)dataReplay,sizeof(COMMAND_MESSAGE)+sizeof(FILTER_REPLY_HEADER));


 //       }
 //   }

	system("pause");
    return hr;
}

