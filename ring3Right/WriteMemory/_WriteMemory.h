/********************************************************************
* ��    ����  ���̷�ɱdll
* ��    �ߣ�  ����
* �� �� �ţ�  1.0
* ������ڣ�  20090313
********************************************************************/


#ifndef __WRITEMEMEORYC_H
#define __WRITEMEMEORYC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#ifdef  _ANTIKILL_EXPORT
#define   ANTIKILL_PORT  __declspec(dllexport)
#else
#define   ANTIKILL_PORT  __declspec(dllimport)
#endif

#include "WriteMemory.h"
#endif // __WRITEMEMEORYC_H
