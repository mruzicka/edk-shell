/*++
 
Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  MemCommonPart.c
  
Abstract: 

  shared code for shell command "dmem/mem".

Revision History

--*/

#include "MemCommonPart.h"

extern UINT8  STRING_ARRAY_NAME[];

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include STRING_DEFINES_FILE

//
// Global Variables
//
EFI_HII_HANDLE  HiiMemHandle;
EFI_GUID        EfiMemGuid = EFI_MEM_GUID;
SHELL_VAR_CHECK_ITEM    MemCheckList[] = {
  {
    L"-MMIO",
    0x01,
    0x04,
    FlagTypeSingle
  },
  {
    L"-B",
    0x02,
    0,
    FlagTypeSingle
  },
  {
    L"-?",
    0x04,
    0x01,
    FlagTypeSingle
  },
  {
    NULL,
    0,
    0,
    0
  }
};

EFI_STATUS
MemMainProcOld (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++
Routine Description:

  mem [Address] [Size] ;MMIO
    if no Address default address is EFI System Table
    if no size default size is 512;
    if ;MMIO then use memory mapped IO and not system memory
    
Arguments:

  ImageHandle - The image handle
  SystemTable - The system table

Returns:

  EFI_SUCCESS - Success

--*/
{
  EFI_STATUS                      Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINT64                          Address;
  UINTN                           Size;
  UINT8                           *Buffer;
  BOOLEAN                         MMIo;
  UINTN                           Index;
  CHAR16                          *AddressStr;
  CHAR16                          *SizeStr;
  CHAR16                          *Ptr;
  BOOLEAN                         PrtHelp;

  Status  = EFI_SUCCESS;
  PrtHelp = FALSE;
  //
  // Get command arguments
  //
  MMIo                  = FALSE;
  AddressStr            = NULL;
  SizeStr               = NULL;
  for (Index = 1; Index < SI->Argc; Index += 1) {
    Ptr = SI->Argv[Index];
    if (*Ptr == ';') {
      //
      // Shortcut! assume MMIo if ; exists
      //
      if (StriCmp (Ptr, L";MMIO") == 0) {
        MMIo = TRUE;
        continue;
      } else {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiMemHandle, L"mem", Ptr);
        Status = EFI_INVALID_PARAMETER;
        return Status;
      }
    } else if (*Ptr == '-') {
      if (0 == StriCmp (Ptr, L"-b")) {
        EnablePageBreak (DEFAULT_INIT_ROW, DEFAULT_AUTO_LF);
        continue;
      } else if (0 == StriCmp (Ptr, L"-?")) {
        PrtHelp = TRUE;
      } else {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiMemHandle, L"mem", Ptr);
        Status = EFI_INVALID_PARAMETER;
        return Status;
      }
    }

    if (!AddressStr) {
      AddressStr = Ptr;
      continue;
    }

    if (!SizeStr) {
      SizeStr = Ptr;
      continue;
    }
  }

  if (PrtHelp) {
    PrintToken (STRING_TOKEN (STR_MEM_VERBOSEHELP), HiiMemHandle);
    return EFI_SUCCESS;
  }

  Address = (AddressStr) ? Xtoi (AddressStr) : (UINT64) SystemTable;
  Size    = (SizeStr) ? Xtoi (SizeStr) : 512;

  //
  // Get memory data
  //
  PrintToken (STRING_TOKEN (STR_MEM_MEMORY_ADDR), HiiMemHandle, 2 * sizeof (UINTN), Address, Size);
  if (MMIo) {
    Status = BS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_LOC_PROT_ERR_EX), HiiMemHandle, L"mem", L"PciRootBridgeIo");
      return Status;
    }
    //
    // Allocate buffer for memory io read
    //
    Buffer = AllocatePool (Size);
    if (Buffer == NULL) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_OUT_RESOURCE), HiiMemHandle, L"mem");
      Status = EFI_OUT_OF_RESOURCES;
      return Status;
    }

    PciRootBridgeIo->Mem.Read (PciRootBridgeIo, IO_UINT8, Address, Size, Buffer);
  } else {
    Buffer = (UINT8 *) (UINTN) Address;
  }
  //
  // Dump data
  //
  DumpHex (2, (UINTN) Address, Size, Buffer);
  EFIMemStructsPrint (Buffer, Size, Address, NULL);

  if (MMIo) {
    FreePool (Buffer);
  }

  return Status;
}

EFI_STATUS
MemMainProc (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*+++
Routine Description:

  mem [Address] [Size] -MMIO
    if no Address default address is EFI System Table
    if no size default size is 512;
    if -MMIO then use memory mapped IO and not system memory
    
Arguments:

  ImageHandle - The image handle
  SystemTable - The system table
  
Returns:

--*/
{
  EFI_STATUS                      Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINT64                          Address;
  UINTN                           Size;
  UINT8                           *Buffer;
  BOOLEAN                         MMIo;
  CHAR16                          *AddressStr;
  CHAR16                          *SizeStr;
  SHELL_VAR_CHECK_CODE            RetCode;
  CHAR16                          *Useful;
  SHELL_ARG_LIST                  *Item;
  SHELL_VAR_CHECK_PACKAGE         ChkPck;

  Status  = EFI_SUCCESS;
  ZeroMem (&ChkPck, sizeof (SHELL_VAR_CHECK_PACKAGE));

  //
  // Get command arguments
  //
  MMIo                  = FALSE;
  AddressStr            = NULL;
  SizeStr               = NULL;
  Address               = (UINT64) SystemTable;
  Size                  = 512;
  RetCode               = LibCheckVariables (SI, MemCheckList, &ChkPck, &Useful);
  if (VarCheckOk != RetCode) {
    switch (RetCode) {
    case VarCheckUnknown:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiMemHandle, L"mem", Useful);
      break;

    case VarCheckConflict:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_FLAG_CONFLICT), HiiMemHandle, L"mem", Useful);
      break;

    case VarCheckDuplicate:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_DUP_FLAG), HiiMemHandle, L"mem", Useful);
      break;

    default:
      break;
    }

    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-b")) {
    EnablePageBreak (DEFAULT_INIT_ROW, DEFAULT_AUTO_LF);
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-?")) {
    if (ChkPck.ValueCount > 0 ||
        ChkPck.FlagCount > 2 ||
        (2 == ChkPck.FlagCount && !LibCheckVarGetFlag (&ChkPck, L"-b"))
        ) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiMemHandle, L"mem");
      Status = EFI_INVALID_PARAMETER;
    } else {
      PrintToken (STRING_TOKEN (STR_MEM_VERBOSEHELP), HiiMemHandle);
      Status = EFI_SUCCESS;
    }

    goto Done;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-MMIO")) {
    MMIo = TRUE;
  }

  Item = ChkPck.VarList;
  while (NULL != Item) {
    if (!AddressStr) {
      AddressStr  = Item->VarStr;
      Address     = StrToUIntegerBase (AddressStr, 16, &Status);
      if (EFI_ERROR (Status) || Address == 0) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiMemHandle, L"mem", AddressStr);
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      Item = Item->Next;
      continue;
    }

    if (!SizeStr) {
      SizeStr = Item->VarStr;
      Size    = (UINTN) StrToUIntegerBase (SizeStr, 10, &Status);
      if (EFI_ERROR (Status) || Size == 0) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiMemHandle, L"mem", SizeStr);
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      break;
    }
  }
  //
  // Get memory data
  //
  PrintToken (STRING_TOKEN (STR_MEM_NEW_MEMORY_ADDR), HiiMemHandle, 2 * sizeof (UINTN), Address, Size);
  if (MMIo) {
    Status = BS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_LOC_PROT_ERR_EX), HiiMemHandle, L"mem", L"PciRootBridgeIo");
      return Status;
    }
    //
    // Allocate buffer for memory io read
    //
    Buffer = AllocatePool (Size);
    if (Buffer == NULL) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_OUT_RESOURCE), HiiMemHandle, L"mem");
      Status = EFI_OUT_OF_RESOURCES;
      return Status;
    }

    PciRootBridgeIo->Mem.Read (PciRootBridgeIo, IO_UINT8, Address, Size, Buffer);
  } else {
    Buffer = (UINT8 *) (UINTN) Address;
  }
  //
  // Dump data
  //
  DumpHex (2, (UINTN) Address, Size, Buffer);
  EFIMemStructsPrint (Buffer, Size, Address, NULL);

  if (MMIo) {
    FreePool (Buffer);
  }

Done:

  LibCheckVarFreeVarList (&ChkPck);
  return Status;
}
