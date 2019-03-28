// CodeGear C++Builder
// Copyright (c) 1995, 2015 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'FastMM4.pas' rev: 30.00 (Windows)

#ifndef Fastmm4HPP
#define Fastmm4HPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member 
#pragma pack(push,8)
#include <System.hpp>
#include <SysInit.hpp>

//-- user supplied -----------------------------------------------------------

#define PatchBCBTerminate

#define EnableMemoryLeakReporting

#define DetectMMOperationsAfterUninstall

#define CheckCppObjectTypeEnabled

namespace Fastmm4
{
//-- forward type declarations -----------------------------------------------
struct TSmallBlockTypeState;
struct TMemoryManagerState;
struct TMemoryManagerUsageSummary;
struct TRegisteredMemoryLeak;
//-- type declarations -------------------------------------------------------
struct DECLSPEC_DRECORD TSmallBlockTypeState
{
public:
	unsigned InternalBlockSize;
	unsigned UseableBlockSize;
	NativeUInt AllocatedBlockCount;
	NativeUInt ReservedAddressSpace;
};


typedef System::StaticArray<TSmallBlockTypeState, 56> TSmallBlockTypeStates;

struct DECLSPEC_DRECORD TMemoryManagerState
{
public:
	TSmallBlockTypeStates SmallBlockTypeStates;
	unsigned AllocatedMediumBlockCount;
	NativeUInt TotalAllocatedMediumBlockSize;
	NativeUInt ReservedMediumBlockAddressSpace;
	unsigned AllocatedLargeBlockCount;
	NativeUInt TotalAllocatedLargeBlockSize;
	NativeUInt ReservedLargeBlockAddressSpace;
};


struct DECLSPEC_DRECORD TMemoryManagerUsageSummary
{
public:
	NativeUInt AllocatedBytes;
	NativeUInt OverheadBytes;
	double EfficiencyPercentage;
};


enum DECLSPEC_DENUM TChunkStatus : unsigned char { csUnallocated, csAllocated, csReserved, csSysAllocated, csSysReserved };

typedef System::StaticArray<TChunkStatus, 65536> TMemoryMap;

struct DECLSPEC_DRECORD TRegisteredMemoryLeak
{
public:
	void *LeakAddress;
	System::TClass LeakedClass;
	void *LeakedCppTypeIdPtr;
	NativeInt LeakSize;
	int LeakCount;
};


typedef System::DynamicArray<TRegisteredMemoryLeak> TRegisteredMemoryLeaks;

enum DECLSPEC_DENUM TStringDataType : unsigned char { stUnknown, stAnsiString, stUnicodeString };

typedef void __fastcall (*TWalkAllocatedBlocksCallback)(void * APBlock, NativeInt ABlockSize, void * AUserData);

typedef unsigned __fastcall (*TGetCppVirtObjSizeByTypeIdPtrFunc)(void * APointer);

typedef void * __fastcall (*TGetCppVirtObjTypeIdPtrFunc)(void * APointer, unsigned ASize);

typedef char * __fastcall (*TGetCppVirtObjTypeNameFunc)(void * APointer, unsigned ASize);

typedef char * __fastcall (*TGetCppVirtObjTypeNameByTypeIdPtrFunc)(void * APointer);

typedef char * __fastcall (*TGetCppVirtObjTypeNameByVTablePtrFunc)(void * AVTablePtr, unsigned AVTablePtrOffset);

//-- var, const, procedure ---------------------------------------------------
#define FastMMVersion L"4.991"
static const System::Int8 NumSmallBlockTypes = System::Int8(0x38);
extern DELPHI_PACKAGE bool FullDebugModeScanMemoryPoolBeforeEveryOperation;
extern DELPHI_PACKAGE bool FullDebugModeRegisterAllAllocsAsExpectedMemoryLeak;
extern DELPHI_PACKAGE bool SuppressMessageBoxes;
extern DELPHI_PACKAGE int *pCppDebugHook;
extern DELPHI_PACKAGE TGetCppVirtObjSizeByTypeIdPtrFunc GetCppVirtObjSizeByTypeIdPtrFunc;
extern DELPHI_PACKAGE TGetCppVirtObjTypeIdPtrFunc GetCppVirtObjTypeIdPtrFunc;
extern DELPHI_PACKAGE TGetCppVirtObjTypeNameFunc GetCppVirtObjTypeNameFunc;
extern DELPHI_PACKAGE TGetCppVirtObjTypeNameByTypeIdPtrFunc GetCppVirtObjTypeNameByTypeIdPtrFunc;
extern DELPHI_PACKAGE TGetCppVirtObjTypeNameByVTablePtrFunc GetCppVirtObjTypeNameByVTablePtrFunc;
extern DELPHI_PACKAGE System::TClass __fastcall DetectClassInstance(void * APointer);
extern DELPHI_PACKAGE void * __fastcall FastGetMem(NativeInt ASize);
extern DELPHI_PACKAGE int __fastcall FastFreeMem(void * APointer);
extern DELPHI_PACKAGE void * __fastcall FastReallocMem(void * APointer, NativeInt ANewSize);
extern DELPHI_PACKAGE void * __fastcall FastAllocMem(NativeInt ASize);
extern DELPHI_PACKAGE bool __fastcall RegisterExpectedMemoryLeak(void * ALeakedPointer)/* overload */;
extern DELPHI_PACKAGE bool __fastcall RegisterExpectedMemoryLeak(System::TClass ALeakedObjectClass, int ACount = 0x1)/* overload */;
extern DELPHI_PACKAGE bool __fastcall RegisterExpectedMemoryLeak(void * ALeakedCppVirtObjTypeIdPtr, int ACount)/* overload */;
extern DELPHI_PACKAGE bool __fastcall RegisterExpectedMemoryLeak(NativeInt ALeakedBlockSize, int ACount = 0x1)/* overload */;
extern DELPHI_PACKAGE bool __fastcall UnregisterExpectedMemoryLeak(void * ALeakedPointer)/* overload */;
extern DELPHI_PACKAGE bool __fastcall UnregisterExpectedMemoryLeak(System::TClass ALeakedObjectClass, int ACount = 0x1)/* overload */;
extern DELPHI_PACKAGE bool __fastcall UnregisterExpectedMemoryLeak(void * ALeakedCppVirtObjTypeIdPtr, int ACount)/* overload */;
extern DELPHI_PACKAGE bool __fastcall UnregisterExpectedMemoryLeak(NativeInt ALeakedBlockSize, int ACount = 0x1)/* overload */;
extern DELPHI_PACKAGE TRegisteredMemoryLeaks __fastcall GetRegisteredMemoryLeaks(void);
extern DELPHI_PACKAGE TStringDataType __fastcall DetectStringData(void * APMemoryBlock, NativeInt AAvailableSpaceInBlock);
extern DELPHI_PACKAGE void __fastcall WalkAllocatedBlocks(TWalkAllocatedBlocksCallback ACallBack, void * AUserData);
extern DELPHI_PACKAGE bool __fastcall LogMemoryManagerStateToFile(const System::UnicodeString AFileName, const System::UnicodeString AAdditionalDetails = System::UnicodeString());
extern DELPHI_PACKAGE void __fastcall GetMemoryManagerState(TMemoryManagerState &AMemoryManagerState);
extern DELPHI_PACKAGE void __fastcall GetMemoryManagerUsageSummary(TMemoryManagerUsageSummary &AMemoryManagerUsageSummary);
extern DELPHI_PACKAGE void __fastcall GetMemoryMap(TMemoryMap &AMemoryMap);
extern DELPHI_PACKAGE System::THeapStatus __fastcall FastGetHeapStatus(void);
extern DELPHI_PACKAGE void __fastcall FreeAllMemory(void);
extern DELPHI_PACKAGE bool __fastcall CheckCanInstallMemoryManager(void);
extern DELPHI_PACKAGE void __fastcall InitializeMemoryManager(void);
extern DELPHI_PACKAGE void __fastcall InstallMemoryManager(void);
extern DELPHI_PACKAGE void __fastcall FinalizeMemoryManager(void);
extern DELPHI_PACKAGE void __fastcall RunInitializationCode(void);
}	/* namespace Fastmm4 */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE) && !defined(NO_USING_NAMESPACE_FASTMM4)
using namespace Fastmm4;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Fastmm4HPP
