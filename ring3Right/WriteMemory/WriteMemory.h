#ifndef _WRITEMEMORY_H
#define _WRITEMEMORY_H

class ANTIKILL_PORT CWriteMemory
{
public:
	CWriteMemory()
	{
	}
	virtual ~CWriteMemory()   
	{
	}
protected:
	static BOOL EnableDebugPrivilege(BOOL bEnable);
	
public:
	static BOOL WriteMemory(const DWORD  dwFixData = 0x10);
	static BOOL FreeMemory()
	{
		return CWriteMemory::WriteMemory(0);
	}
	static void SetProtectFlag(DWORD DFlag){CWriteMemory::m_dwFixData = DFlag;}//0x10 �ռ����� 0x40 ����������ϵͳ������ 0 �ָ�
private:
	static DWORD m_dwFixData;
};

#endif