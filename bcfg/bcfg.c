/*++

Copyright (c) 2010 - 2013, Intel Corporation.
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  bcfg.c

Abstract:

  EFI Shell command 'bcfg'

Revision History

--*/

#include "bcfg.h"

#define COMMAND_NAME    L"bcfg"

#define COPY_SIZE_SAME  ((UINTN) -1)

extern UINT8  STRING_ARRAY_NAME[];

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include STRING_DEFINES_FILE

//
// Global Variables
//
STATIC EFI_HII_HANDLE HiiHandle;
EFI_GUID EfiBcfgGuid = EFI_BCFG_GUID;
SHELL_VAR_CHECK_ITEM  BcfgCheckList[] = {
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
    L"-v",
    0x04,
    0,
    FlagTypeSingle
  },
  {
    L"-opt",
    0x08,
    0,
    FlagTypeSingle
  },
  {
    NULL,
    0,
    0,
    (SHELL_VAR_CHECK_FLAG_TYPE) 0
  }
};

//
// Function Declarations
//
EFI_STATUS
EFIAPI
InitializeBcfg (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

//
// Entry point
//
EFI_BOOTSHELL_CODE (
  EFI_APPLICATION_ENTRY_POINT (InitializeBcfg)
)

VOID *
AllocateCopyPool (
  IN VOID   *Buffer,
  IN UINTN  OldAllocationSize,
  IN UINTN  NewAllocationSize
  )
/*++

Routine Description:

  Copies a buffer to a newly allocated buffer.
  Allocates the number bytes specified by NewAllocationSize, copies OldAllocationSize
  bytes from Buffer to the newly allocated buffer, and returns a pointer to the
  allocated buffer.
  If NewAllocationSize is 0, then a valid buffer of 0 size is returned.  If there
  is not enough memory remaining to satisfy the request, then NULL is returned.
  If Buffer is NULL, then ASSERT ().

Arguments:

  Buffer                The buffer to copy to the allocated buffer.
  OldAllocationSize     The number of bytes to copy from the Buffer.
  NewAllocationSize     The number of bytes to allocate.

Returns:

  A pointer to the allocated buffer or NULL if allocation fails.

--*/
{
  VOID  *Memory;

  ASSERT (Buffer != NULL);
  ASSERT (NewAllocationSize >= OldAllocationSize);

  if (OldAllocationSize == COPY_SIZE_SAME) {
    OldAllocationSize = NewAllocationSize;
  }

  Memory = AllocatePool (NewAllocationSize);
  if (Memory != NULL) {
    CopyMem (Memory, Buffer, OldAllocationSize);
  }

  return Memory;
}

EFI_STATUS
StrToGuid (
  IN     CHAR16    *StringGuid,
  IN OUT EFI_GUID  *Guid
  )
/*++

Routine Description:

  Converts a string representation of a guid to a Guid value.

Arguments:

  StringGuid     The pointer to the string of a guid.
  Guid           The pointer to the GUID structure to populate.

Returns:

  EFI_SUCCESS               The conversion was successful.
  EFI_INVALID_PARAMETER     The guid string was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS  Status;
  CHAR16      *TempStringGuid;
  EFI_GUID    TempGuid;
  UINT64      Intermediate;
  UINTN       LoopVar;

  ASSERT (Guid != NULL);

  if (StringGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TempStringGuid = StrDuplicate (StringGuid);
  if (TempStringGuid == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  StringGuid = TempStringGuid;

  // Trim the guid string
  for (; *StringGuid == L' '; StringGuid++) {
    ;
  }
  StrTrimRight (StringGuid, L' ');

  if (StrLen (StringGuid) != 36) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  if (StringGuid[8] != L'-') {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  StringGuid[8] = L'\0';
  Intermediate = StrToUIntegerBase (StringGuid, 16, &Status);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  TempGuid.Data1 = (UINT32) Intermediate;

  // advance to the second group
  StringGuid += 9;

  if (StringGuid[4] != L'-') {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  StringGuid[4] = L'\0';
  Intermediate = StrToUIntegerBase (StringGuid, 16, &Status);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  TempGuid.Data2 = (UINT16) Intermediate;

  // advance to the third group
  StringGuid += 5;

  if (StringGuid[4] != L'-') {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  StringGuid[4] = L'\0';
  Intermediate = StrToUIntegerBase (StringGuid, 16, &Status);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  TempGuid.Data3 = (UINT16) Intermediate;

  // advance to the fourth group
  StringGuid += 5;

  if (StringGuid[4] != L'-') {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  StringGuid[4] = L'\0';
  Intermediate = StrToUIntegerBase (StringGuid, 16, &Status);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  Intermediate <<= 48;

  // advance to the fifth and final group
  StringGuid += 5;

  Intermediate |= StrToUIntegerBase (StringGuid, 16, &Status);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  for (LoopVar = sizeof (Intermediate); LoopVar > 0;) {
    TempGuid.Data4[--LoopVar] = (UINT8) Intermediate;
    Intermediate >>= 8;
  }

  CopyMem (Guid, &TempGuid, sizeof (TempGuid));

Done:
  FreePool (TempStringGuid);

  return Status;
}

EFI_STATUS
BcfgDisplayDump (
  IN BCFG_OPERATION_TARGET  Target,
  IN BOOLEAN                VerboseOutput,
  IN UINT16                 *CurrentOrder,
  IN UINTN                  OrderCount
  )
/*++

Routine Description:

  Dumps the Bcfg information.

Arguments:

  Target            The target of the operation.
  VerboseOutput     TRUE for extra output.  FALSE otherwise.
  CurrentOrder      The pointer to the current order of items.
  OrderCount        How many to dump.

Returns:

  EFI_SUCCESS               The dump was successful.
  EFI_INVALID_PARAMETER     A parameter was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS          Status;
  UINTN               LoopVar;
  LOAD_OPTION_HEADER  *LoadOption;
  UINTN               OptionDataSize;
  CHAR16              *DevPathString;
  UINT8               *OptionalDataPtr;
  UINT8               *OptionalDataEndPtr;
  CHAR16              VariableName[12];

  if (OrderCount == 0) {
    PrintToken (STRING_TOKEN (STR_BCFG_NONE), HiiHandle, COMMAND_NAME);
    return EFI_SUCCESS;
  }

  for (LoopVar = 0; LoopVar < OrderCount; LoopVar++) {
    LoadOption     = NULL;
    OptionDataSize = 0;
    SPrint (VariableName, sizeof (VariableName), L"%s%04x", Target == BcfgTargetBootOrder?L"Boot":L"Driver", CurrentOrder[LoopVar]);

    while (GrowBuffer (&Status, (VOID **) &LoadOption, OptionDataSize)) {
      Status = RT->GetVariable (
                     VariableName,
                     &gEfiGlobalVariableGuid,
                     NULL,
                     &OptionDataSize,
                     LoadOption);
    }
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_VAR_READ_FAIL), HiiHandle, COMMAND_NAME, VariableName, Status);
      if (Status == EFI_NOT_FOUND) {
        continue;
      }
      return Status;
    }

    if (LoadOption->FilePathListLength != 0) {
      DevPathString = LibDevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL*) (LoadOption->Description+StrLen (LoadOption->Description)+1));
    } else {
      DevPathString = NULL;
    }

    PrintToken (
      STRING_TOKEN (STR_BCFG_LOAD_OPTION),
      HiiHandle,
      LoopVar,
      VariableName,
      LoadOption->Description,
      DevPathString,
      (sizeof (*LoadOption) + StrLen (LoadOption->Description)*sizeof (LoadOption->Description) + LoadOption->FilePathListLength <= OptionDataSize)?L'N':L'Y'
      );
    if (VerboseOutput) {
      OptionalDataPtr = ((UINT8*) (LoadOption->Description+StrLen (LoadOption->Description)+1)) + LoadOption->FilePathListLength;
      OptionalDataEndPtr = ((UINT8*) LoadOption) + OptionDataSize;
      while (OptionalDataPtr < OptionalDataEndPtr) {
        Print (L"%02x", *OptionalDataPtr++);
      }
      Print (L"\n");
    }

    if (LoadOption != NULL) {
      FreePool (LoadOption);
    }
    if (DevPathString != NULL) {
      FreePool (DevPathString);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetDevicePathForDriverHandle (
  IN  EFI_HANDLE                Handle,
  OUT EFI_DEVICE_PATH_PROTOCOL  **FilePath
  )
/*++

Routine Description:

  Populates the device path protocol parameter based on Handle.

Arguments:

  Handle       A driver handle.
  FilePath     On a sucessful return the device path to the handle.

Returns:

  EFI_SUCCESS     The device path was sucessfully returned.
  other value     An error from BS->HandleProtocol.

--*/
{
  EFI_STATUS                 Status;
  EFI_LOADED_IMAGE_PROTOCOL  *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL   *ImageDevicePath;

  ASSERT (FilePath != NULL);

  Status = BS->HandleProtocol (
                 Handle,
                 &gEfiLoadedImageProtocolGuid,
                 (VOID**) &LoadedImage
                 );
  if (!EFI_ERROR (Status)) {
    ASSERT (LoadedImage != NULL);
    Status = BS->HandleProtocol (
                   LoadedImage->DeviceHandle,
                   &gEfiDevicePathProtocolGuid,
                   (VOID**) &ImageDevicePath
                   );
    if (!EFI_ERROR (Status)) {
      ASSERT (ImageDevicePath != NULL);
      *FilePath = AppendDevicePath (ImageDevicePath, LoadedImage->FilePath);
    }
  }

  return Status;
}

INTN
CompareUINT16 (
  IN UINT16  *L,
  IN UINT16  *R
  )
/*++

Routine Description:

  Compares two UINT16 values.

Arguments:

  L     Pointer to the first UINT16 value to compare.
  R     Pointer to the second UINT16 value to comapre.

Returns:

  1      If *L > *R.
  -1     If *L < *R.
  0      If *L == *R.

--*/
{
  ASSERT (L);
  ASSERT (R);

  if (*L > *R) {
    return 1;
  }
  if (*L < *R) {
    return -1;
  }

  return 0;
}

EFI_STATUS
BcfgAdd (
  IN BCFG_OPERATION_TARGET  Target,
  IN UINTN                  Position,
  IN BCFG_OPERATION_TYPE    Type,
  IN CHAR16                 *File,
  IN UINTN                  HandleNumber,
  IN EFI_GUID               *Guid,
  IN CHAR16                 *Desc,
  IN UINT16                 *CurrentOrder,
  IN UINTN                  OrderCount
  )
/*++

Routine Description:

  Adds a boot or driver option.

Arguments:

  Target           The info on the option to add.
  Position         The position to add the option at.
  Type             The type of target (file/handle/guid) to add.
  File             The file to make the target.
  HandleNumber     The handle number to make the target.
  Guid             The firmware guid to make the target.
  Desc             The description text.
  CurrentOrder     The pointer to the current order of items.
  OrderCount       The number if items in CurrentOrder.

Returns:

  EFI_SUCCESS               The operation was successful.
  EFI_INVALID_PARAMETER     A parameter was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS                Result;
  EFI_STATUS                Status;
  EFI_LIST_ENTRY            FileList;
  SHELL_FILE_ARG            *Arg;
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  LOAD_OPTION_HEADER        *LoadOption;
  UINTN                     DescLen;
  UINTN                     FilePathSize;
  UINTN                     LoopVar;
  UINTN                     TargetLocation;
  UINT16                    *NewOrder;
  UINTN                     DriverBindingHandleCount;
  UINTN                     ParentControllerHandleCount;
  UINTN                     ChildControllerHandleCount;
  CHAR16                    VariableName[12];

  if (Type == BcfgTypeAdd) {
    if (File == NULL || HandleNumber != 0) {
      return EFI_INVALID_PARAMETER;
    }
  } else if (Type == BcfgTypeAddh) {
    if (HandleNumber == 0 || File != NULL) {
      return EFI_INVALID_PARAMETER;
    }
  } else if (Type == BcfgTypeAddg) {
    if (HandleNumber != 0 || File != NULL) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    ASSERT (FALSE);
  }
  if (Desc == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Position > OrderCount) {
    Position = OrderCount;
  }

  InitializeListHead (&FileList);
  FilePath           = NULL;

  if (Type == BcfgTypeAdd) {
    //
    // Get file info
    //
    Status = ShellFileMetaArg (File, &FileList);
    if (EFI_ERROR (Status) || IsListEmpty (&FileList)) {
      //
      // If filename matched nothing, fail
      //
      PrintToken (STRING_TOKEN (STR_BCFG_FILE_OPEN_FAIL), HiiHandle, COMMAND_NAME, File, EFI_ERROR (Status)?Status:EFI_NOT_FOUND);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }
    if (FileList.Flink->Flink != &FileList) {
      //
      // If filename expanded to multiple names, fail
      //
      PrintToken (STRING_TOKEN (STR_BCFG_FILE_MULTI), HiiHandle, COMMAND_NAME, File);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    Arg = CR (FileList.Flink, SHELL_FILE_ARG, Link, SHELL_FILE_ARG_SIGNATURE);
    if (EFI_ERROR (Arg->Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_FILE_OPEN_FAIL), HiiHandle, COMMAND_NAME, File, Arg->Status);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    //
    // Build FilePath of the filename
    //
    FilePath = SE2->NameToPath (Arg->FullName);
    if (FilePath == NULL) {
      PrintToken (STRING_TOKEN (STR_BCFG_FILE_DP), HiiHandle, COMMAND_NAME, Arg->FullName);
      Result = EFI_UNSUPPORTED;
      goto Done;
    }
  } else if (Type == BcfgTypeAddh) {
    Handle = ShellHandleFromIndex (HandleNumber);
    if (Handle == NULL) {
      PrintToken (STRING_TOKEN (STR_BCFG_ARG_PROBLEM), HiiHandle, COMMAND_NAME, L"<handle>");
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    LibGetManagingDriverBindingHandles (
      Handle,
      &DriverBindingHandleCount,
      NULL
      );

    LibGetParentControllerHandles (
      Handle,
      &ParentControllerHandleCount,
      NULL);

    LibGetChildControllerHandles (
      Handle,
      &ChildControllerHandleCount,
      NULL);

    Status = BS->HandleProtocol (
                   Handle,
                   &gEfiDevicePathProtocolGuid,
                   (VOID**) &FilePath
                   );

    if (Target == BcfgTargetBootOrder) {
      //
      // Check that the handle points to a real controller.
      //
      if ((DriverBindingHandleCount == 0 &&
           ParentControllerHandleCount == 0 &&
           ChildControllerHandleCount == 0) ||
          EFI_ERROR (Status)
          ) {
        PrintToken (STRING_TOKEN (STR_BCFG_HANDLE), HiiHandle, COMMAND_NAME, HandleNumber);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Duplicate the file path so that we don't free a system owned copy later
      FilePath = DuplicateDevicePath (FilePath);
      if (FilePath == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
      }
    } else {
      //
      // Check that the handle points to a driver, not a controller.
      //
      if (DriverBindingHandleCount > 0 ||
          ParentControllerHandleCount > 0 ||
          ChildControllerHandleCount > 0 ||
          !EFI_ERROR (Status)
          ) {
        PrintToken (STRING_TOKEN (STR_BCFG_ARG_PROBLEM), HiiHandle, COMMAND_NAME, L"<handle>");
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Get the FilePath from the loaded image information.
      //
      Status = GetDevicePathForDriverHandle (Handle, &FilePath);
    }
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_HANDLE_DP), HiiHandle, COMMAND_NAME, HandleNumber, Status);
      Result = Status;
      goto Done;
    }
  } else if (Type == BcfgTypeAddg) {
    FilePath = AllocatePool (sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH) + sizeof (EFI_DEVICE_PATH_PROTOCOL));
    if (FilePath == NULL) {
      Result = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    InitializeFwVolDevicepathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*) FilePath, Guid);

    SetDevicePathEndNode ((EFI_DEVICE_PATH_PROTOCOL*) (((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*) FilePath) + 1));
  } else {
    ASSERT (FALSE);
  }

  //
  // Find a free target
  //
  NewOrder = AllocateCopyPool (CurrentOrder, COPY_SIZE_SAME, OrderCount * sizeof (*CurrentOrder));
  if (NewOrder != NULL) {
    QSort (NewOrder, OrderCount, sizeof (*NewOrder), (INTN (*) (VOID*, VOID*)) CompareUINT16);
    TargetLocation = (UINTN) -1;
    for (LoopVar = 0; LoopVar < OrderCount; LoopVar++) {
      if (TargetLocation == NewOrder[LoopVar]) {
        // Skip duplicate entries.
        continue;
      }
      if (TargetLocation+1 == NewOrder[LoopVar]) {
        // Continue as there is no hole in the sequence.
        TargetLocation++;
        continue;
      }
      // Break as there is a hole in the sequence.
      break;
    }
    FreePool (NewOrder);
    TargetLocation++;
  } else {
    TargetLocation = 0x10000;
  }
  if (TargetLocation > 0xFFFF) {
    PrintToken (STRING_TOKEN (STR_BCFG_TARGET_NF), HiiHandle, COMMAND_NAME);
    Result = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  //
  // Add the option
  //
  ASSERT (FilePath != NULL);

  DescLen = StrLen (Desc);
  FilePathSize = DevicePathSize (FilePath);

  SPrint (VariableName, sizeof (VariableName), L"%s%04x", Target == BcfgTargetBootOrder?L"Boot":L"Driver", TargetLocation);

  LoadOption = AllocateZeroPool (sizeof (*LoadOption) + DescLen * sizeof (*Desc) + FilePathSize);
  if (LoadOption != NULL) {
    LoadOption->Attributes         = LOAD_OPTION_ACTIVE;
    LoadOption->FilePathListLength = (UINT16) FilePathSize;
    CopyMem (LoadOption->Description, Desc, (DescLen+1)*sizeof (*Desc));
    CopyMem (LoadOption->Description+DescLen+1, FilePath, FilePathSize);

    Status = RT->SetVariable (
                   VariableName,
                   &gEfiGlobalVariableGuid,
                   EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                   sizeof (*LoadOption) + DescLen * sizeof (*Desc) + FilePathSize,
                   LoadOption
                   );

    FreePool (LoadOption);
  } else {
    Status = EFI_OUT_OF_RESOURCES;
  }
  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, VariableName, Status);
    Result = Status;
    goto Done;
  }

  NewOrder = AllocateZeroPool ((OrderCount+1) * sizeof (*CurrentOrder));
  if (NewOrder != NULL) {
    //
    // Insert target into order list
    //
    CopyMem (NewOrder, CurrentOrder, Position * sizeof (*CurrentOrder));
    NewOrder[Position] = (UINT16) TargetLocation;
    CopyMem (NewOrder+Position+1, CurrentOrder+Position, (OrderCount-Position) * sizeof (*CurrentOrder));

    Status = RT->SetVariable (
                   Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder",
                   &gEfiGlobalVariableGuid,
                   EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                   (OrderCount+1) * sizeof (*NewOrder),
                   NewOrder
                   );

    FreePool (NewOrder);
  } else {
    Status = EFI_OUT_OF_RESOURCES;
  }
  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder", Status);
    Result = Status;
    goto Done;
  }

  PrintToken (STRING_TOKEN (STR_BCFG_ADDED_AT_POS), HiiHandle, VariableName, Position);

  Result = EFI_SUCCESS;

Done:
  if (FilePath) {
    FreePool (FilePath);
  }
  ShellFreeFileList (&FileList);

  return Result;
}

EFI_STATUS
BcfgMove (
  IN BCFG_OPERATION_TARGET  Target,
  IN UINT16                 OldLocation,
  IN UINT16                 NewLocation,
  IN UINT16                 *CurrentOrder,
  IN UINTN                  OrderCount
  )
/*++

Routine Description:

  Moves a boot or driver option to another location.

Arguments:

  Target           The target to move.
  OldLocation      The current location of the Target.
  NewLocation      The desired location of the Target.
  CurrentOrder     The pointer to the current order of items.
  OrderCount       The number if items in CurrentOrder.

Returns:

  EFI_SUCCESS               The operation was successful.
  EFI_INVALID_PARAMETER     A parameter was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS  Status;
  UINT16      MovedOption;

  MovedOption = CurrentOrder[OldLocation];
  if (OldLocation < NewLocation) {
    // Shift intermediate options towards the start
    CopyMem (CurrentOrder+OldLocation, CurrentOrder+OldLocation+1, (NewLocation-OldLocation) * sizeof (*CurrentOrder));
  } else {
    // Shift intermediate options towards the end
    CopyMem (CurrentOrder+NewLocation+1, CurrentOrder+NewLocation, (OldLocation-NewLocation) * sizeof (*CurrentOrder));
  }
  CurrentOrder[NewLocation] = MovedOption;

  Status = RT->SetVariable (
                 Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                 OrderCount * sizeof (*CurrentOrder),
                 CurrentOrder);
  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder", Status);
  }

  return Status;
}

EFI_STATUS
BcfgSet (
  IN BCFG_OPERATION_TARGET  Target,
  IN UINT16                 OptionIndex,
  IN UINT16                 TargetLocation,
  IN UINT16                 *CurrentOrder,
  IN UINTN                  OrderCount
  )
/*++

Routine Description:

  Sets a boot option to point to the specified Boot#### or Driver#### variable.

Arguments:

  Target            The target to act upon.
  OptionIndex       The option to set.
  TargetLocation    The index of the variable to point the option to.
  CurrentOrder      The pointer to the current order of items.
  OrderCount        The number if items in CurrentOrder.

Returns:

  EFI_SUCCESS               The operation was successful.
  EFI_INVALID_PARAMETER     A parameter was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS  Status;
  CHAR16      VariableName[12];
  UINTN       Size;
  UINT16      *NewOrder;

  SPrint (VariableName, sizeof (VariableName), L"%s%04x", Target == BcfgTargetBootOrder?L"Boot":L"Driver", TargetLocation);
  Size = 0;
  Status = RT->GetVariable (
                 VariableName,
                 &gEfiGlobalVariableGuid,
                 NULL,
                 &Size,
                 NULL
                 );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      PrintToken (STRING_TOKEN (STR_BCFG_VAR_NF), HiiHandle, COMMAND_NAME, VariableName);
      return EFI_INVALID_PARAMETER;
    } else if (Status != EFI_BUFFER_TOO_SMALL) {
      PrintToken (STRING_TOKEN (STR_BCFG_VAR_READ_FAIL), HiiHandle, COMMAND_NAME, VariableName, Status);
      return Status;
    }
  }

  if (OptionIndex >= OrderCount) {
    OptionIndex = OrderCount;
    NewOrder = AllocateCopyPool (CurrentOrder, OrderCount * sizeof (*CurrentOrder), (OrderCount+1) * sizeof (*CurrentOrder));
    OrderCount++;
  } else {
    NewOrder = CurrentOrder;
  }
  NewOrder[OptionIndex] = TargetLocation;

  Status = RT->SetVariable (
                 Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                 OrderCount * sizeof (*NewOrder),
                 NewOrder);
  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder", Status);
  } else {
    PrintToken (STRING_TOKEN (STR_BCFG_SET_TO), HiiHandle, TargetLocation, VariableName);
  }

  if (NewOrder != CurrentOrder) {
    FreePool (NewOrder);
  }

  return Status;
}

EFI_STATUS
BcfgRemove (
  IN BCFG_OPERATION_TARGET  Target,
  IN UINT16                 Location,
  IN UINT16                 *CurrentOrder,
  IN UINTN                  OrderCount
  )
/*++

Routine Description:

  Removes a boot or driver option.

Arguments:

  Target           The target item to move.
  Location         The current location of the Target.
  CurrentOrder     The pointer to the current order of items.
  OrderCount       The number if items in CurrentOrder.

Returns:

  EFI_SUCCESS               The operation was successful.
  EFI_INVALID_PARAMETER     A parameter was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS  Status;
  CHAR16      VariableName[12];

  SPrint (VariableName, sizeof (VariableName), L"%s%04x", Target == BcfgTargetBootOrder?L"Boot":L"Driver", CurrentOrder[Location]);

  Status = RT->SetVariable (
                 VariableName,
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                 0,
                 NULL
                 );
  if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_UNSET_FAIL), HiiHandle, COMMAND_NAME, VariableName, Status);
    return Status;
  }

  // Discard the option from the Boot/Driver Order
  OrderCount--;
  CopyMem (CurrentOrder+Location, CurrentOrder+Location+1, (OrderCount - Location) * sizeof (*CurrentOrder));

  Status = RT->SetVariable (
                 Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                 OrderCount * sizeof (*CurrentOrder),
                 CurrentOrder
                 );
  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder", Status);
  }

  return Status;
}

EFI_STATUS
BcfgReadFileContent (
  IN  CHAR16  *File,
  OUT VOID    **Data OPTIONAL,
  OUT UINTN   *Size
  )
/*++

Routine Description:

  Reads the entire content of a file.

Arguments:

  File     The file to read.
  Data     Pointer to a variable which will receive pointer to an allocated buffer containing the file data.
  Size     Pointer to a variable which will receive the number of bytes read.

Returns:

  EFI_SUCESS                The file content has been read successfully.
  EFI_INVALID_PARAMETER     The File parameter does not appoint an existing file.
  other value               An error occured.

--*/
{
  EFI_STATUS      Result;
  EFI_STATUS      Status;
  EFI_LIST_ENTRY  FileList;
  SHELL_FILE_ARG  *Arg;
  VOID            *Buffer;

  ASSERT (Size != NULL);

  InitializeListHead (&FileList);

  //
  // Find & open the file.
  //
  Status = ShellFileMetaArgNoWildCard (File, &FileList);
  if (EFI_ERROR (Status)) {
    Result = Status == EFI_NOT_FOUND ? EFI_INVALID_PARAMETER : Status;
    goto Done;
  }
  if (IsListEmpty (&FileList)) {
    Result = EFI_INVALID_PARAMETER;
    goto Done;
  }

  Arg = CR (FileList.Flink, SHELL_FILE_ARG, Link, SHELL_FILE_ARG_SIGNATURE);
  if (EFI_ERROR (Arg->Status)) {
    Result = Arg->Status == EFI_NO_MAPPING || Arg->Status == EFI_NOT_FOUND ? EFI_INVALID_PARAMETER : Arg->Status;
    goto Done;
  }

  ASSERT (Arg->Info != NULL);

  //
  // Check that it is really a file (and not a directory).
  //
  if (Arg->Info->Attribute & EFI_FILE_DIRECTORY) {
    Result = EFI_NOT_FOUND;
    goto Done;
  }

  *Size = Arg->Info->FileSize;

  Buffer = AllocatePool (*Size);
  if (Buffer == NULL) {
    Result = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  //
  // Read the file's content.
  //
  Status = LibReadFile (Arg->Handle, Size, Buffer);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    Result = Status;
    goto Done;
  } else {
    if (Data != NULL) {
      *Data = Buffer;
    } else {
      FreePool (Buffer);
    }
  }

  Result = EFI_SUCCESS;

Done:
  ShellFreeFileList (&FileList);

  return Result;
}

EFI_STATUS
UpdateOptionalData (
  IN CHAR16  *VariableName,
  IN VOID    *OptionalData,
  IN UINTN   OptionalDataSize
  )
/*++

Routine Description:

  Writes optional data of a boot or driver option.
  If the optional data already exists it is overwritten.

Arguments:

  VariableName         The variable to update.
  OptionalData         The buffer for the optional data.
  OptionalDataSize     The size in bytes of the optional data.

Returns:

  EFI_SUCCESS     The optional data was updated sucessfully.
  other value     An error occured.

--*/
{
  EFI_STATUS          Status;
  LOAD_OPTION_HEADER  *OriginalData;
  UINTN               OriginalSize;
  UINTN               OriginalDataSize;
  LOAD_OPTION_HEADER  *NewData;
  UINTN               NewSize;

  ASSERT (VariableName != NULL);

  OriginalData = NULL;
  OriginalSize = 0;

  while (GrowBuffer (&Status, (VOID **) &OriginalData, OriginalSize)) {
    Status = RT->GetVariable (
                   VariableName,
                   &gEfiGlobalVariableGuid,
                   NULL,
                   &OriginalSize,
                   OriginalData
                   );
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (OriginalData != NULL);
  ASSERT (OriginalSize >= sizeof (LOAD_OPTION_HEADER));

  //
  // Rewrite the old optional data.
  //
  OriginalDataSize = sizeof (*OriginalData)
                     + StrLen (OriginalData->Description) * sizeof (OriginalData->Description)
                     + OriginalData->FilePathListLength;
  NewSize = OriginalDataSize + OptionalDataSize;
  if (NewSize > OriginalSize) {
    NewData = AllocateCopyPool (OriginalData, OriginalDataSize, NewSize);
    FreePool (OriginalData);
    if (NewData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    NewData = OriginalData;
  }
  CopyMem (((UINT8*) NewData)+OriginalDataSize, OptionalData, OptionalDataSize);

  //
  // Put the data back under the variable.
  //
  Status = RT->SetVariable (
                 VariableName,
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                 NewSize,
                 NewData
                 );

  FreePool (NewData);

  return Status;
}

EFI_STATUS
GetBootOptionCrc (
  IN  CHAR16  *VariableName,
  OUT UINT32  *Crc
  )
/*++

Routine Description:

  Computes the CRC of a boot variable.

Arguments:

  VariableName     The variable to compute the CRC of.
  Crc              The CRC value to return.

Returns:

  EFI_SUCCESS     The CRC was computed sucessfully.
  other value     An error occured.

--*/
{
  EFI_STATUS  Status;
  VOID        *Buffer;
  UINTN       BufferSize;

  ASSERT (VariableName != NULL);
  ASSERT (Crc != NULL);

  Buffer     = NULL;
  BufferSize = 0;

  //
  // Get the data Buffer
  //
  while (GrowBuffer (&Status, &Buffer, BufferSize)) {
    Status = RT->GetVariable (
                   VariableName,
                   &gEfiGlobalVariableGuid,
                   NULL,
                   &BufferSize,
                   Buffer
                   );
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get the CRC computed
  //
  Status = BS->CalculateCrc32 (Buffer, BufferSize, Crc);

  FreePool (Buffer);

  return Status;
}

EFI_STATUS
BcfgAddOpt (
  IN BCFG_OPERATION_TARGET  Target,
  IN UINT16                 OptionIndex,
  IN SHELL_ARG_LIST         *Param,
  IN UINT16                 *CurrentOrder,
  IN UINTN                  OrderCount
  )
/*++

Routine Description:

  Adds optional data to an option.

Arguments:

  Target           The target of the operation.
  OptionIndex      The index of the variable to add the optional data to.
  Pram             The linked list of additional parameters.
  CurrentOrder     The pointer to the current order of items.
  OrderCount       The number of items in CurrentOrder.

Returns:

  EFI_SUCCESS               The operation was successful.
  EFI_INVALID_PARAMETER     A parameter was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS      Result;
  EFI_STATUS      Status;
  VOID            *Data;
  UINTN           Size;
  SHELL_ARG_LIST  *Item;
  EFI_KEY_OPTION  KeyOption;
  EFI_INPUT_KEY   *Key;
  UINTN           LoopCounter;
  UINT64          Intermediate;
  UINT16          KeyIndex;
  CHAR16          VariableName[12];

  ASSERT (Param != NULL);

  Data           = NULL;
  ZeroMem (&KeyOption, sizeof (KeyOption));

  //
  // Determine whether we have file name, data or a hot-key
  //
  if (GetNextArg (Param) == NULL) {
    //
    // We have only one parameter, i.e. a filename or data
    //
    Status = BcfgReadFileContent (Param->VarStr, &Data, &Size);
    if (Status == EFI_INVALID_PARAMETER) {
      Size = StrSize (Param->VarStr);
      Data = AllocateCopyPool (Param->VarStr, COPY_SIZE_SAME, Size);
      if (Data == NULL) {
        Result = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
    } else if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_FILE_READ_FAIL), HiiHandle, COMMAND_NAME, Param->VarStr, Status);
      Result = Status;
      goto Done;
    }

    SPrint (VariableName,
            sizeof (VariableName),
            L"%s%04x",
            Target == BcfgTargetBootOrder?L"Boot":L"Driver",
            CurrentOrder[OptionIndex]
            );

    Status = UpdateOptionalData (VariableName, Data, Size);
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, VariableName, Status);
      Result = Status;
      goto Done;
    }
  } else {
    if (Target != BcfgTargetBootOrder) {
      PrintToken (STRING_TOKEN (STR_BCFG_BOOT_ONLY), HiiHandle, COMMAND_NAME);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    Intermediate = StrToUInteger (Param->VarStr, &Status);
    if (EFI_ERROR (Status) || (((UINT32) Intermediate) != Intermediate)) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, Param->VarStr);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }
    KeyOption.KeyData.PackedValue = (UINT32) Intermediate;

    Data = AllocateCopyPool (&KeyOption, sizeof (KeyOption), sizeof (KeyOption) + KeyOption.KeyData.Options.InputKeyCount * sizeof (EFI_INPUT_KEY));
    if (Data == NULL) {
      PrintToken (STRING_TOKEN (STR_BCFG_NO_MEM), HiiHandle, COMMAND_NAME);
      Result = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Item = Param;
    for (LoopCounter = KeyOption.KeyData.Options.InputKeyCount,
         Key = (EFI_INPUT_KEY*) (((EFI_KEY_OPTION*) Data) + 1);
         LoopCounter > 0;
         LoopCounter--, Key++
         ) {
      //
      // ScanCode
      //
      Item = GetNextArg (Item);
      if (Item == NULL) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      Intermediate = StrToUInteger (Item->VarStr, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, Item->VarStr);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      Key->ScanCode    = (UINT16) Intermediate;

      //
      // UnicodeChar
      //
      Item = GetNextArg (Item);
      if (Item == NULL) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      Intermediate = StrToUInteger (Item->VarStr, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, Item->VarStr);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      Key->UnicodeChar = (UINT16) Intermediate;
    }

    //
    // Now do the BootOption / BootOptionCrc
    //
    ASSERT (OptionIndex < OrderCount);
    ((EFI_KEY_OPTION*) Data)->BootOption = CurrentOrder[OptionIndex];
    SPrint (VariableName, sizeof (VariableName), L"Boot%04x", ((EFI_KEY_OPTION*) Data)->BootOption);
    Status = GetBootOptionCrc (VariableName, &(((EFI_KEY_OPTION*) Data)->BootOptionCrc));
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_ARG_PROBLEM), HiiHandle, COMMAND_NAME, L"Option Index");
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    for (KeyIndex = 0; KeyIndex <= 0xFFFF; KeyIndex++) {
      SPrint (VariableName, sizeof (VariableName), L"Key%04x", KeyIndex);
      Size = 0;
      Status = RT->GetVariable (
                     VariableName,
                     &gEfiGlobalVariableGuid,
                     NULL,
                     &Size,
                     NULL
                     );
      if (Status == EFI_NOT_FOUND) {
        break;
      }
    }
    if (KeyIndex > 0xFFFF) {
      PrintToken (STRING_TOKEN (STR_BCFG_HOTKEY_NF), HiiHandle, COMMAND_NAME);
      Result = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Status = RT->SetVariable (
                   VariableName,
                   &gEfiGlobalVariableGuid,
                   EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                   sizeof (EFI_KEY_OPTION) + (sizeof (EFI_INPUT_KEY) * KeyOption.KeyData.Options.InputKeyCount),
                   Data
                   );
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_BCFG_VAR_SET_FAIL), HiiHandle, COMMAND_NAME, VariableName, Status);
      Result = Status;
      goto Done;
    }
  }

  Result = EFI_SUCCESS;

Done:
  if (Data) {
    FreePool (Data);
  }

  return Result;
}

VOID
InitBcfgStruct (
  IN BGFG_OPERATION  *Struct
  )
/*++

Routine Description:

  Initializes the BCFG operation structure.

Arguments:

  Struct     The stuct to initialize.

--*/
{
  ASSERT (Struct != NULL);
  Struct->Target      = BcfgTargetMax;
  Struct->Type        = BcfgTypeMax;
  Struct->Number1     = 0;
  Struct->Number2     = 0;
  Struct->HandleIndex = 0;
  Struct->FileName    = NULL;
  ZeroMem (&(Struct->Guid), sizeof (Struct->Guid));
  Struct->Description = NULL;
  Struct->Order       = NULL;
}

EFI_STATUS
EFIAPI
InitializeBcfg (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  The entry point of the 'bcfg' command. Parses command line arguments and
  calls internal functions to perform the requested action.

Arguments:

  ImageHandle     Handle to the Image.
  SystemTable     Pointer to the System Table.

Returns:

  EFI_SUCCESS               The command was executed successfully.
  EFI_INVALID_PARAMETER     A command line argument was invalid.
  other value               An error occured.

--*/
{
  EFI_STATUS               Status;
  EFI_STATUS               Result;
  BGFG_OPERATION           CurrentOperation;
  CHAR16                   *CurrentParam;
  UINTN                    ParamNumber;
  UINTN                    OrderCount;
  UINT64                   Intermediate;

  SHELL_VAR_CHECK_CODE     RetCode;
  CHAR16                   *Useful;
  SHELL_ARG_LIST           *Item;
  SHELL_VAR_CHECK_PACKAGE  ChkPck;

  //
  // Initialize app
  //
  ZeroMem (&ChkPck, sizeof (SHELL_VAR_CHECK_PACKAGE));
  InitBcfgStruct (&CurrentOperation);

  EFI_SHELL_APP_INIT (ImageHandle, SystemTable);

  //
  // Enable tab key which can pause the output
  //
  EnableOutputTabPause ();

  Status = LibInitializeStrings (&HiiHandle, STRING_ARRAY_NAME, &EfiBcfgGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!EFI_PROPER_VERSION (0, 99)) {
    PrintToken (
      STRING_TOKEN (STR_SHELLENV_GNC_COMMAND_NOT_SUPPORT),
      HiiHandle,
      COMMAND_NAME,
      EFI_VERSION_0_99
      );
    Result = EFI_UNSUPPORTED;
    goto Done;
  }

  RetCode = LibCheckVariables (SI, BcfgCheckList, &ChkPck, &Useful);
  if (RetCode != VarCheckOk) {
    switch (RetCode) {
      case VarCheckDuplicate:
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_DUP_FLAG), HiiHandle, COMMAND_NAME, Useful);
        break;
      case VarCheckUnknown:
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiHandle, COMMAND_NAME, Useful);
        break;
      case VarCheckLackValue:
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_LACK_ARG), HiiHandle, COMMAND_NAME, Useful);
        break;
      default:
        break;
      case VarCheckOutOfMem:
        Result = EFI_OUT_OF_RESOURCES;
        goto Done;
    }

    Result = EFI_INVALID_PARAMETER;
    goto Done;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-b") != NULL) {
    EnablePageBreak (DEFAULT_INIT_ROW, DEFAULT_AUTO_LF);
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-?") != NULL) {
    //
    // Display help.
    //
    if (ChkPck.ValueCount > 0 ||
        ChkPck.FlagCount > 2 ||
        (2 == ChkPck.FlagCount && !LibCheckVarGetFlag (&ChkPck, L"-b"))
        ) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiHandle, COMMAND_NAME);
      Result = EFI_INVALID_PARAMETER;
    } else {
      PrintToken (STRING_TOKEN (STR_BCFG_VERBOSE_HELP), HiiHandle);
      Result = EFI_SUCCESS;
    }

    goto Done;
  }

  // At least one parameter (the target) is required by all operation types
  ParamNumber = 1;
  if (ChkPck.ValueCount < ParamNumber) {
    PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
    Result = EFI_INVALID_PARAMETER;
    goto Done;
  }

  //
  // Read in the operation target
  //
  Item = GetFirstArg (&ChkPck);
  if        (StriCmp (Item->VarStr, L"driver") == 0) {
    CurrentOperation.Target = BcfgTargetDriverOrder;
  } else if (StriCmp (Item->VarStr, L"boot")   == 0) {
    CurrentOperation.Target = BcfgTargetBootOrder;
  } else {
    PrintToken (STRING_TOKEN (STR_BCFG_NO_DRIVER_BOOT), HiiHandle, COMMAND_NAME);
    Result = EFI_INVALID_PARAMETER;
    goto Done;
  }

  //
  // Read in the boot or driver order environment variable
  //
  OrderCount = 0;
  while (GrowBuffer (&Status, (VOID **) &CurrentOperation.Order, OrderCount)) {
    Status = RT->GetVariable (
                   CurrentOperation.Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder",
                   &gEfiGlobalVariableGuid,
                   NULL,
                   &OrderCount,
                   CurrentOperation.Order
                   );
  }
  if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
    PrintToken (STRING_TOKEN (STR_BCFG_VAR_READ_FAIL), HiiHandle, COMMAND_NAME, CurrentOperation.Target == BcfgTargetBootOrder?L"BootOrder":L"DriverOrder", Status);
    Result = Status;
    goto Done;
  }
  // Compute the number of entries in the order list
  OrderCount /= sizeof (*CurrentOperation.Order);

  //
  // Read in the type of operation and verify parameter types for the info.
  //
  if (LibCheckVarGetFlag (&ChkPck, L"-opt") != NULL) {
    // At least two more parameter are required by the operation -opt operation type
    ParamNumber += 2;
    if (ChkPck.ValueCount < ParamNumber) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    // Read in the entry index
    Item         = GetNextArg (Item);
    CurrentParam = Item->VarStr;
    Intermediate = StrToUInteger (CurrentParam, &Status);
    if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }
    CurrentOperation.Number1     = (UINT16) Intermediate;
    if (CurrentOperation.Number1 >= OrderCount) {
      PrintToken (STRING_TOKEN (STR_BCFG_NUMB_RANGE), HiiHandle, COMMAND_NAME, OrderCount);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    // Advance Item to point to the next parameter
    Item         = GetNextArg (Item);

    CurrentOperation.Type = BcfgTypeOpt;
  } else {
    // At least one more parameter is required by all of the following operation types
    ParamNumber += 1;
    if (ChkPck.ValueCount < ParamNumber) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    Item         = GetNextArg (Item);
    CurrentParam = Item->VarStr;

    if        (StriCmp (CurrentParam, L"dump") == 0) {
      CurrentOperation.Type = BcfgTypeDump;
    } else if (StriCmp (CurrentParam, L"add" ) == 0) {
      // This operation type requires three more parameters
      ParamNumber += 3;
      if (ChkPck.ValueCount < ParamNumber) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number1     = (UINT16) Intermediate;

      // Read in the entry path
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      CurrentOperation.FileName    = StrDuplicate (CurrentParam);
      if (CurrentOperation.FileName == NULL) {
        Result = EFI_OUT_OF_RESOURCES;
        goto Done;
      }

      // Read in the entry description
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      CurrentOperation.Description = StrDuplicate (CurrentParam);
      if (CurrentOperation.Description == NULL) {
        Result = EFI_OUT_OF_RESOURCES;
        goto Done;
      }

      CurrentOperation.Type = BcfgTypeAdd;
    } else if (StriCmp (CurrentParam, L"addh") == 0) {
      // This operation type requires three more parameters
      ParamNumber += 3;
      if (ChkPck.ValueCount < ParamNumber) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number1     = (UINT16) Intermediate;

      // Read in the handle index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.HandleIndex = (UINT16) Intermediate;

      // Read in the entry description
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      CurrentOperation.Description = StrDuplicate (CurrentParam);
      if (CurrentOperation.Description == NULL) {
        Result = EFI_OUT_OF_RESOURCES;
        goto Done;
      }

      CurrentOperation.Type = BcfgTypeAddh;
    } else if (StriCmp (CurrentParam, L"addg") == 0) {
      // This operation type requires three more parameters
      ParamNumber += 3;
      if (ChkPck.ValueCount < ParamNumber) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number1     = (UINT16) Intermediate;

      // Read in the guid
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Status = StrToGuid (CurrentParam, &CurrentOperation.Guid);
      if (EFI_ERROR (Status)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry description
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      CurrentOperation.Description = StrDuplicate (CurrentParam);
      if (CurrentOperation.Description == NULL) {
        Result = EFI_OUT_OF_RESOURCES;
        goto Done;
      }

      CurrentOperation.Type = BcfgTypeAddg;
    } else if (StriCmp (CurrentParam, L"mv"  ) == 0) {
      // This operation type requires two more parameters
      ParamNumber += 2;
      if (ChkPck.ValueCount < ParamNumber) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number1     = (UINT16) Intermediate;
      if (CurrentOperation.Number1 >= OrderCount) {
        PrintToken (STRING_TOKEN (STR_BCFG_NUMB_RANGE), HiiHandle, COMMAND_NAME, OrderCount);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the target entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number2     = (UINT16) Intermediate;
      if (CurrentOperation.Number2 >= OrderCount || CurrentOperation.Number2 == CurrentOperation.Number1) {
        PrintToken (STRING_TOKEN (STR_BCFG_NUMB_RANGE), HiiHandle, COMMAND_NAME, OrderCount);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      CurrentOperation.Type = BcfgTypeMv;
    } else if (StriCmp (CurrentParam, L"set"  ) == 0) {
      // This operation type requires two more parameters
      ParamNumber += 2;
      if (ChkPck.ValueCount < ParamNumber) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number1     = (UINT16) Intermediate;
      if (CurrentOperation.Number1 > OrderCount) {
        PrintToken (STRING_TOKEN (STR_BCFG_NUMB_RANGE), HiiHandle, COMMAND_NAME, OrderCount);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the target index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number2     = (UINT16) Intermediate;

      CurrentOperation.Type = BcfgTypeSet;
    } else if (StriCmp (CurrentParam, L"rm"  ) == 0) {
      // This operation type requires one more parameter
      ParamNumber += 1;
      if (ChkPck.ValueCount < ParamNumber) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_FEW), HiiHandle, COMMAND_NAME);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      // Read in the entry index
      Item         = GetNextArg (Item);
      CurrentParam = Item->VarStr;
      Intermediate = StrToUInteger (CurrentParam, &Status);
      if (EFI_ERROR (Status) || (((UINT16) Intermediate) != Intermediate)) {
        PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, COMMAND_NAME, CurrentParam);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }
      CurrentOperation.Number1     = (UINT16) Intermediate;
      if (CurrentOperation.Number1 >= OrderCount) {
        PrintToken (STRING_TOKEN (STR_BCFG_NUMB_RANGE), HiiHandle, COMMAND_NAME, OrderCount);
        Result = EFI_INVALID_PARAMETER;
        goto Done;
      }

      CurrentOperation.Type = BcfgTypeRm;
    } else {
      PrintToken (STRING_TOKEN (STR_BCFG_ARG_PROBLEM), HiiHandle, COMMAND_NAME, CurrentParam);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    if (ChkPck.ValueCount > ParamNumber) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiHandle, COMMAND_NAME);
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }
  }

  //
  // we have all the info.  Do the work
  //
  if (CurrentOperation.Type == BcfgTypeDump) {
    Result = BcfgDisplayDump (
               CurrentOperation.Target,
               LibCheckVarGetFlag (&ChkPck, L"-v") != NULL,
               CurrentOperation.Order,
               OrderCount
               );
  } else {
    if (LibCheckVarGetFlag (&ChkPck, L"-v") != NULL) {
      PrintToken (STRING_TOKEN (STR_BCFG_ARG_PROBLEM), HiiHandle, COMMAND_NAME, L"-v (without dump)");
      Result = EFI_INVALID_PARAMETER;
      goto Done;
    }

    switch (CurrentOperation.Type) {
      case BcfgTypeAdd:
      case BcfgTypeAddh:
      case BcfgTypeAddg:
        Result = BcfgAdd (
                   CurrentOperation.Target,
                   CurrentOperation.Number1,
                   CurrentOperation.Type,
                   CurrentOperation.FileName,
                   CurrentOperation.HandleIndex,
                   &CurrentOperation.Guid,
                   CurrentOperation.Description,
                   CurrentOperation.Order,
                   OrderCount
                   );
        break;
      case BcfgTypeMv:
        Result = BcfgMove (
                   CurrentOperation.Target,
                   CurrentOperation.Number1,
                   CurrentOperation.Number2,
                   CurrentOperation.Order,
                   OrderCount
                   );
        break;
      case BcfgTypeSet:
        Result = BcfgSet (
                   CurrentOperation.Target,
                   CurrentOperation.Number1,
                   CurrentOperation.Number2,
                   CurrentOperation.Order,
                   OrderCount
                   );
        break;
      case BcfgTypeRm:
        Result = BcfgRemove (
                   CurrentOperation.Target,
                   CurrentOperation.Number1,
                   CurrentOperation.Order,
                   OrderCount
                   );
        break;
      case BcfgTypeOpt:
        Result = BcfgAddOpt (
                   CurrentOperation.Target,
                   CurrentOperation.Number1,
                   Item,
                   CurrentOperation.Order,
                   OrderCount
                   );
        break;
      default:
        ASSERT (FALSE);
    }
  }

Done:
  if (CurrentOperation.FileName != NULL) {
    FreePool (CurrentOperation.FileName);
  }
  if (CurrentOperation.Description != NULL) {
    FreePool (CurrentOperation.Description);
  }
  if (CurrentOperation.Order != NULL) {
    FreePool (CurrentOperation.Order);
  }

  LibCheckVarFreeVarList (&ChkPck);
  LibUnInitializeStrings ();

  return Result;
}

EFI_STATUS
EFIAPI
InitializeBcfgGetLineHelp (
  OUT CHAR16  **Str
  )
/*++

Routine Description:

  Retrieves the line help of the 'bcfg' command.

Arguments:

  Str     Pointer to a variable which will receive the pointer to the line help string.

Returns:

  EFI_SUCCESS     The line help was retrieved successfully.
  other value     An error occured.

--*/
{
  return LibCmdGetStringByToken (STRING_ARRAY_NAME, &EfiBcfgGuid, STRING_TOKEN (STR_BCFG_LINE_HELP), Str);
}
