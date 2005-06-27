/*++

Copyright 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  dmpstore.c
  
Abstract:

  Shell app "dmpstore"



Revision History

--*/

#include "EfiShellLib.h"
#include "dmpstore.h"

extern UINT8  STRING_ARRAY_NAME[];

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include STRING_DEFINES_FILE
//
//
//
#define DEBUG_NAME_SIZE 1050

STATIC CHAR16   *AttrType[] = {
  L"invalid",   // 000
  L"invalid",   // 001
  L"BS",        // 010
  L"NV+BS",     // 011
  L"RT+BS",     // 100
  L"NV+RT+BS",  // 101
  L"RT+BS",     // 110
  L"NV+RT+BS",  // 111
};

//
//
//
EFI_STATUS
InitializeDumpStore (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
DumpVariableStore (
  CHAR16 *VarName
  );

//
// Global Variables
//
EFI_HII_HANDLE  HiiHandle;
EFI_GUID        EfiDmpstoreGuid = EFI_DMPSTORE_GUID;
SHELL_VAR_CHECK_ITEM    DmpstoreCheckList[] = {
  {
    L"-b",
    0x01,
    0,
    FlagTypeSingle
  },
  {
    L"-?",
    0x02,
    0,
    FlagTypeSingle
  },
  {
    NULL,
    0,
    0,
    0
  }
};

EFI_BOOTSHELL_CODE(
  EFI_APPLICATION_ENTRY_POINT(InitializeDumpStore)
)

EFI_STATUS
InitializeDumpStore (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

Routine Description:

  Command entry point

Arguments:

  ImageHandle - The image handle
  SystemTable - The system table

Returns:

  EFI_SUCCESS - Success

--*/
{
  CHAR16                  *VarName;
  EFI_STATUS              Status;

  SHELL_VAR_CHECK_CODE    RetCode;
  CHAR16                  *Useful;
  SHELL_VAR_CHECK_PACKAGE ChkPck;

  ZeroMem (&ChkPck, sizeof (SHELL_VAR_CHECK_PACKAGE));
  //
  // We are no being installed as an internal command driver, initialize
  // as an nshell app and run
  //
  EFI_SHELL_APP_INIT (ImageHandle, SystemTable);

  //
  // Register our string package with HII and return the handle to it.
  // If previously registered we will simply receive the handle
  //
  Status = LibInitializeStrings (&HiiHandle, STRING_ARRAY_NAME, &EfiDmpstoreGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarName = NULL;
  Status  = EFI_SUCCESS;

  LibFilterNullArgs ();
  //
  // Check flags
  //
  Useful  = NULL;
  RetCode = LibCheckVariables (SI, DmpstoreCheckList, &ChkPck, &Useful);
  if (VarCheckOk != RetCode) {
    switch (RetCode) {
    case VarCheckDuplicate:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_DUP_FLAG), HiiHandle, L"dmpstore", Useful);
      break;

    case VarCheckUnknown:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiHandle, L"dmpstore", Useful);
      break;

    default:
      break;
    }

    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-b") != NULL) {
    EnablePageBreak (DEFAULT_INIT_ROW, DEFAULT_AUTO_LF);
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-?")) {
    if (ChkPck.ValueCount > 0 ||
        ChkPck.FlagCount > 2 ||
        (2 == ChkPck.FlagCount && !LibCheckVarGetFlag (&ChkPck, L"-b"))
        ) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiHandle, L"dmpstore");
      Status = EFI_INVALID_PARAMETER;
    } else {
      PrintToken (STRING_TOKEN (STR_DMPSTORE_VERBOSEHELP), HiiHandle);
      Status = EFI_SUCCESS;
    }

    goto Done;
  }

  if (ChkPck.ValueCount > 1) {
    PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiHandle, L"dmpstore");
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  if (NULL != ChkPck.VarList) {
    VarName = ChkPck.VarList->VarStr;
  }
  //
  // Dump all variables in store
  //
  if (VarName == NULL) {
    Status = DumpVariableStore (NULL);
    goto Done;
  }
  //
  // Dump one variable in store
  //
  if (VarName != NULL) {
    Status = DumpVariableStore (VarName);
  }
  //
  // Done
  //
Done:
  LibCheckVarFreeVarList (&ChkPck);
  LibUnInitializeStrings ();
  return Status;
}

EFI_STATUS
DumpVariableStore (
  CHAR16 *VarName
  )
{
  EFI_STATUS  Status;
  EFI_GUID    Guid;
  UINT32      Attributes;
  CHAR16      Name[DEBUG_NAME_SIZE / 2];
  UINTN       NameSize;
  CHAR16      Data[DEBUG_NAME_SIZE / 2];
  UINTN       DataSize;
  BOOLEAN     Found;

  Found = FALSE;

  if (VarName != NULL) {
    PrintToken (STRING_TOKEN (STR_DMPSTORE_DUMP_ONE_VAR), HiiHandle, VarName);
  } else {
    PrintToken (STRING_TOKEN (STR_DMPSTORE_DUMP), HiiHandle);
  }

  Name[0] = 0x0000;
  do {
    NameSize  = DEBUG_NAME_SIZE;
    Status    = RT->GetNextVariableName (&NameSize, Name, &Guid);
    if (VarName != NULL) {
      if (!MetaiMatch (Name, VarName)) {
        continue;
      }
    }

    if (!EFI_ERROR (Status)) {
      Found     = TRUE;
      DataSize  = DEBUG_NAME_SIZE;
      Status    = RT->GetVariable (Name, &Guid, &Attributes, &DataSize, Data);
      if (!EFI_ERROR (Status)) {
        //
        // dump variables
        //
        PrintToken (
          STRING_TOKEN (STR_DMPSTORE_VAR),
          HiiHandle,
          AttrType[Attributes & 7],
          &Guid,
          Name,
          DataSize
          );

        if (PrivateDumpHex (2, 0, DataSize, Data)) {
          goto Done;
        }
      }
    }
  } while (!EFI_ERROR (Status));

  if (Found == FALSE) {
    if (VarName != NULL) {
      PrintToken (STRING_TOKEN (STR_DMPSTORE_VAR_NOT_FOUND), HiiHandle, VarName);
    } else {
      PrintToken (STRING_TOKEN (STR_DMPSTORE_VAR_EMPTY), HiiHandle);
    }
  }

Done:
  return EFI_SUCCESS;
}

EFI_STATUS
InitializeDumpStoreGetLineHelp (
  OUT CHAR16                **Str
  )
/*++

Routine Description:

  Get this command's line help

Arguments:

  Str - The line help

Returns:

  EFI_SUCCESS   - Success

--*/
{
  return LibCmdGetStringByToken (STRING_ARRAY_NAME, &EfiDmpstoreGuid, STRING_TOKEN (STR_DMPSTORE_LINEHELP), Str);
}
