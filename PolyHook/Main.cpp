// PolyHook.cpp : Defines the entry point for the console application.
//
#include <tchar.h>
#include "PolyHook.h"
#define PLH_SHOW_DEBUG_MESSAGES 1 //To print messages even in release

typedef int(__stdcall* tMessageBoxA)(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
tMessageBoxA oMessageBoxA;

typedef void(__stdcall* tVirtNoParams)(DWORD_PTR pThis);
tVirtNoParams oVirtNoParams;

typedef void(__stdcall* tGetCurrentThreadId)();
tGetCurrentThreadId oGetCurrentThreadID;

typedef int(__stdcall* tVEH)(int intparam);
tVEH oVEHTest;

PLH::VEHHook* VEHHook;

DWORD __stdcall hkGetCurrentThreadId()
{
	printf("Called hkGetCurrentThreadID\n");
	return 1337;
}

int __stdcall hkMessageBoxA(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	printf("In Hook\n");
	return oMessageBoxA(hWnd, lpText, lpCaption, uType);
}

void __stdcall hkVirtNoParams(DWORD_PTR pThis)
{
	printf("hk Virt Called\n");
	return oVirtNoParams(pThis);
}

__declspec(noinline) int __stdcall VEHTest(int param)
{
	printf("VEHFunc %d\n",param);
	return 3;
}

__declspec(noinline) int __stdcall hkVEHTest(int param)
{
	printf("hkVEH %d\n",param);
	auto ProtectionObject = VEHHook->GetProtectionObject();

	return oVEHTest(param);
}

class VirtualTest
{
public:
	virtual void NoParamVirt()
	{
		volatile int x = 0;
		printf("Called Virt\n");
	}
	virtual void BS()
	{
		volatile int y = 0;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	///X86/x64 Detour Example
	//PLH::Detour* Hook = new PLH::Detour();
	//Hook->SetupHook((BYTE*)&MessageBoxA,(BYTE*) &hkMessageBoxA); //can cast to byte* to
	//Hook->Hook();
	//oMessageBoxA = Hook->GetOriginal<tMessageBoxA>();
	//MessageBoxA(NULL, "Message", "Sample", MB_OK);
	//Hook->UnHook();
	//MessageBoxA(NULL, "Message", "Sample", MB_OK);

	///x86/x64 IAT Hook Example
	PLH::IATHook* Hook = new PLH::IATHook();
	Hook->SetupHook("kernel32.dll", "GetCurrentThreadId", (BYTE*)&hkGetCurrentThreadId);
	Hook->Hook();
	oGetCurrentThreadID = Hook->GetOriginal<tGetCurrentThreadId>();
	printf("Thread ID:%d \n", GetCurrentThreadId());
	Hook->UnHook();
	printf("Real Thread ID:%d\n", GetCurrentThreadId());

	///x86/x64 VFuncDetour Example
	/*VirtualTest* ClassToHook = new VirtualTest();
	PLH::VFuncDetour* VirtHook = new PLH::VFuncDetour();
	VirtHook->SetupHook(*(BYTE***)ClassToHook, 0, (BYTE*)&hkVirtNoParams);
	VirtHook->Hook();
	oVirtNoParams = VirtHook->GetOriginal<tVirtNoParams>();
	ClassToHook->NoParamVirt();
	VirtHook->UnHook();
	ClassToHook->NoParamVirt();*/

	///x86/x64 VFuncSwap Example
	/*PLH::VFuncSwap* VirtHook = new PLH::VFuncSwap();
	VirtHook->SetupHook(*(BYTE***)ClassToHook, 0, (BYTE*)&hkVirtNoParams);
	VirtHook->Hook();
	oVirtNoParams = VirtHook->GetOriginal<tVirtNoParams>();
	ClassToHook->NoParamVirt();
	VirtHook->UnHook();
	ClassToHook->NoParamVirt();*/

	///x86/x64 VTableSwap Example
	//PLH::VTableSwap* VTableHook = new PLH::VTableSwap();
	//VTableHook->SetupHook((BYTE*)ClassToHook, 0, (BYTE*)&hkVirtNoParams);
	//VTableHook->Hook();
	//oVirtNoParams = VTableHook->GetOriginal<tVirtNoParams>();
	//ClassToHook->NoParamVirt();
	//VTableHook->UnHook();
	//ClassToHook->NoParamVirt();

	/*!!!!IMPORTANT!!!!!: Since this demo is small it's possible for internal methods to be on the same memory page
	as the VEHTest function. If that happens the GUARD_PAGE type method will fail with an unexpected exception. 
	If this method is used in larger applications this risk is incredibly small, to the point where it should not
	be worried about. You CANNOT run this demo under a debugger when using VEH type
	*/
	///x86/x64 VEH Example (GUARD_PAGE and INT3_BP)
	/*VEHHook = new PLH::VEHHook();
	VEHHook->SetupHook((BYTE*)&VEHTest, (BYTE*)&hkVEHTest, PLH::VEHHook::VEHMethod::INT3_BP);
	VEHHook->Hook();
	oVEHTest = VEHHook->GetOriginal<tVEH>();
	VEHTest(3);
	VEHHook->UnHook();
	VEHTest(1);
	VEHHook->PrintError(VEHHook->GetLastError());*/

	Sleep(100000);
	return 0;
}
