#include <windows.h>
#include <crtdbg.h>
#include<commctrl.h>
#include <stdio.h>
#include "SafeProcess.h"

#pragma data_seg("SafeProcess")
HWND hTarget=NULL;
HINSTANCE g_hInstance = NULL;
HHOOK  g_cbtHook = NULL;
HWND g_hWnd = NULL;
BOOL bSetWindowLong = FALSE;
WNDPROC g_orgProc =  NULL;
#pragma data_seg()

#define  TASKMGR "Windows ���������"
#define  SAFEPROCESS "HookTaskmgr.exe"
#define DLLEXPORT __declspec(dllexport) 

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
char* GetListText();

BOOL WINAPI DllMain(
					HANDLE hinstDLL,  // handle to the DLL module
					DWORD fdwReason,     // reason for calling function
					LPVOID lpvReserved   // reserved
					)

{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hInstance = (HINSTANCE)hinstDLL;  
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			SafeProcessTerm();
		}
		break;
	}
	return TRUE;
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CHAR szText[256];
	CHAR szWindowText[32];
	HWND hWnd;
	memset(szWindowText, NULL, sizeof(szWindowText));
	strcpy(szWindowText, "�������������");

	switch(nCode)
	{
	case HCBT_ACTIVATE: 
		{
			hWnd = (HWND)wParam;
			GetWindowText(hWnd,szText,256);
			if(strcmp(szText, szWindowText) == 0)
			//if(hWnd==hTarget)  //check if the window activated is Our Targer App
			{
				if(!bSetWindowLong)
				{
					g_hWnd = hWnd;
					g_orgProc=(WNDPROC)SetWindowLong(hWnd,GWL_WNDPROC,(LONG)NewWndProc);
					bSetWindowLong = TRUE;
				}
			}
		}
		break;
	case HCBT_DESTROYWND:
		{
			hWnd = (HWND)wParam;
			GetWindowText(hWnd,szText,256);
			if(strcmp(szText, szWindowText) == 0)
			//if(hWnd==hTarget) 
			{
				if (g_hWnd)
				{
					SetWindowLong(g_hWnd,GWL_WNDPROC,(LONG)g_orgProc);   
					g_hWnd = NULL;
					bSetWindowLong = false;
				}
			}
		}
		break;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);  
}

LRESULT CALLBACK NewWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND: 
		if ((wParam == 0x06) && (HIWORD(wParam) == BN_CLICKED) )
		{ 
			//ȡ��Ŀ�����
			char *ItemBuf = GetListText();

			//��Ŀ����̱Ƚ�
			char sProcess[32];
			memset(sProcess, NULL, sizeof(sProcess));
			strcpy(sProcess, SAFEPROCESS);

			if(strcmp(sProcess, ItemBuf) == 0)
			{
				::SendMessage(hWnd,WM_COMMAND,0x07,0);  
				char szBuffer[600];
				char* pData = szBuffer;
				sprintf(pData,"�޷�����'%s'����!",ItemBuf); 
				MessageBox(hWnd,pData,"��ʾ",MB_ICONWARNING);   
				//::SendMessage( hWnd,WM_CLOSE,0,0);  
				return 0;  
			}
		}
		break; 
	/*case WM_ENABLE: 
		{
			MessageBox(NULL,"����������޷���������!","��ʾ",MB_ICONERROR); 
			return 0;
		}
		break;*/
	/*case WM_CANCELMODE:
		{
			MessageBox(NULL,"����������޷���������!","��ʾ",MB_ICONERROR); 
			return 0;
		}
		break;*/
	default:
		break;
	}
	return CallWindowProc(g_orgProc, hWnd, uMsg, wParam, lParam); 
}

DLLEXPORT int WINAPI SetHandle(HWND HandleofTarget)
{
	hTarget=HandleofTarget;
	g_cbtHook=SetWindowsHookEx(WH_CBT,(HOOKPROC)CBTProc,g_hInstance,GetWindowThreadProcessId(hTarget,NULL));
	return 1;
}//End this function

DLLEXPORT BOOL SafeProcessInit()
{
	if (g_cbtHook == NULL)
	{	
		if (g_hInstance != NULL)
		{
			g_cbtHook = SetWindowsHookEx(WH_CBT,CBTProc, g_hInstance, GetWindowThreadProcessId(hTarget,NULL));
		}
	}
	if (!g_cbtHook)
		return FALSE;

	_ASSERT(g_cbtHook); 
	return TRUE;
}

DLLEXPORT void SafeProcessTerm()
{
	if (g_cbtHook != NULL) 
	{
		if( UnhookWindowsHookEx(g_cbtHook))
		{
			 g_cbtHook = NULL;
		}
	}
}
//ȡ�������������ѡ��
char* GetListText()
{
	static char output[512]; 

	//�����������������
	DWORD PID;
	HANDLE hProcess;

	LVITEM lvitem, *plvitem = NULL;
	char ItemBuf[512],*pItem = NULL;

	HWND hwnd=FindWindow("#32770","Windows ���������");
	hwnd=FindWindowEx(hwnd,0,"#32770",0);
	//�����б�ؼ�
	hwnd=FindWindowEx(hwnd,0,"SysListView32",0);
	if (!hwnd)
	{
		MessageBox(NULL,"Windows ��������� ��δ����","��ʾ��",MB_ICONWARNING);
		return output;
	}
	//ȡ��ѡ�������
	int iItem=SendMessage(hwnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
	if (iItem==-1)
	{
		MessageBox(NULL,"û��ָ��Ŀ����̣�","��ʾ��",MB_ICONWARNING);
		CloseHandle(hwnd);
		return output;
	}

	//�򿪽���
	GetWindowThreadProcessId(hwnd,&PID);
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,false,PID);
	if (!hProcess)
	{
		MessageBox(NULL,"��ȡ���̾������ʧ��","��ʾ��",MB_ICONWARNING);
		CloseHandle(hwnd);
		return output;
	}

	//���������ڴ�
	plvitem=(LVITEM*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
	pItem=(char*)VirtualAllocEx(hProcess, NULL, 512, MEM_COMMIT, PAGE_READWRITE);
	if ((!plvitem)||(!pItem))
	{
		MessageBox(NULL,"�޷������ڴ棡","��ʾ��",MB_ICONWARNING);
		CloseHandle(hwnd);
		CloseHandle(hProcess);
		return output;
	}

	//�õ�ѡ������
	lvitem.cchTextMax=512;
	//lvitem.iSubItem=1;//PID
	lvitem.iSubItem=0; //ProcessName
	lvitem.pszText=pItem;
	WriteProcessMemory(hProcess, plvitem, &lvitem, sizeof(LVITEM), NULL);
	SendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)iItem, (LPARAM)plvitem);
	ReadProcessMemory(hProcess, pItem, ItemBuf, 512, NULL);
	strcpy(output,ItemBuf);

	//�ͷ��ڴ�
	CloseHandle(hwnd);
	CloseHandle(hProcess);
	VirtualFreeEx(hProcess, plvitem, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, pItem, 0, MEM_RELEASE);
	return output;
}