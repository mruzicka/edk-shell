/*++

Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:
    libMemImage.c

  Abstract:
    Functions to deal with Mem buffer
--*/

#include "heditor.h"

extern EFI_HANDLE                 HImageHandleBackup;

extern HEFI_EDITOR_BUFFER_IMAGE   HBufferImage;

extern BOOLEAN                    HBufferImageNeedRefresh;
extern BOOLEAN                    HBufferImageOnlyLineNeedRefresh;
extern BOOLEAN                    HBufferImageMouseNeedRefresh;

extern HEFI_EDITOR_GLOBAL_EDITOR  HMainEditor;

HEFI_EDITOR_MEM_IMAGE             HMemImage;
HEFI_EDITOR_MEM_IMAGE             HMemImageBackupVar;

//
// for basic initialization of HDiskImage
//
HEFI_EDITOR_MEM_IMAGE             HMemImageConst = {
  NULL,
  0,
  0
};

EFI_STATUS
HMemImageInit (
  VOID
  )
/*++

Routine Description: 

  Initialization function for HDiskImage

Arguments:  

  None

Returns:  

  EFI_SUCCESS
  EFI_LOAD_ERROR

--*/
{
  EFI_STATUS  Status;

  //
  // basically initialize the HMemImage
  //
  CopyMem (&HMemImage, &HMemImageConst, sizeof (HMemImage));

  Status = BS->LocateProtocol (
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &HMemImage.IoFncs
                );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  } else {
    return EFI_LOAD_ERROR;
  }
}

EFI_STATUS
HMemImageBackup (
  VOID
  )
/*++

Routine Description: 

  Backup function for HDiskImage
  Only a few fields need to be backup. 
  This is for making the Disk buffer refresh 
  as few as possible.

Arguments:  

  None

Returns:  

  EFI_SUCCESS

--*/
{
  HMemImageBackupVar.Offset = HMemImage.Offset;
  HMemImageBackupVar.Size   = HMemImage.Size;

  return EFI_SUCCESS;
}

EFI_STATUS
HMemImageCleanup (
  VOID
  )
/*++

Routine Description: 

  Cleanup function for HDiskImage

Arguments:  

  None

Returns:  

  EFI_SUCCESS

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
HMemImageSetMemOffsetSize (
  IN UINTN Offset,
  IN UINTN Size
  )
/*++

Routine Description: 

  Set FileName field in HFileImage

Arguments:  

  Offset - The offset
  Size   - The size

Returns:  

  EFI_SUCCESS
  EFI_OUT_OF_RESOURCES

--*/
{

  HMemImage.Offset  = Offset;
  HMemImage.Size    = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
HMemImageRead (
  IN UINTN  Offset,
  IN UINTN  Size,
  BOOLEAN   Recover
  )
/*++

Routine Description: 

  Read a disk from disk into HBufferImage

Arguments:  

  Offset  - The offset
  Size    - The size
  Recover - if is for recover, no information print

Returns:  

  EFI_SUCCESS
  EFI_LOAD_ERROR
  EFI_OUT_OF_RESOURCES
  
--*/
{

  EFI_STATUS                      Status;
  void                            *Buffer;
  CHAR16                          *Str;
  HEFI_EDITOR_LINE                *Line;

  HEFI_EDITOR_ACTIVE_BUFFER_TYPE  BufferTypeBackup;

  BufferTypeBackup        = HBufferImage.BufferType;
  HBufferImage.BufferType = MEM_BUFFER;

  Buffer                  = AllocatePool (Size);
  if (Buffer == NULL) {
    HMainStatusBarSetStatusString (L"Read Memory Failed");
    return EFI_OUT_OF_RESOURCES;
  }

  Status = HMemImage.IoFncs->Mem.Read (
                                  HMemImage.IoFncs,
                                  EfiPciWidthUint8,
                                  Offset,
                                  Size,
                                  Buffer
                                  );

  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    HMainStatusBarSetStatusString (L"Memory Specified Not Accessible");
    return EFI_LOAD_ERROR;
  }

  HBufferImageFree ();

  Status = HBufferImageBufferToList (Buffer, Size);
  FreePool (Buffer);

  if (EFI_ERROR (Status)) {
    HMainStatusBarSetStatusString (L"Read Memory Failed");
    return Status;
  }

  Status  = HMemImageSetMemOffsetSize (Offset, Size);

  HBufferImage.DisplayPosition.Row    = TEXT_START_ROW;
  HBufferImage.DisplayPosition.Column = HEX_POSITION;

  HBufferImage.MousePosition.Row      = TEXT_START_ROW;
  HBufferImage.MousePosition.Column   = HEX_POSITION;

  HBufferImage.LowVisibleRow          = 1;
  HBufferImage.HighBits               = TRUE;

  HBufferImage.BufferPosition.Row     = 1;
  HBufferImage.BufferPosition.Column  = 1;

  if (!Recover) {
    Str = PoolPrint (L"%d Lines Read", HBufferImage.NumLines);
    if (Str == NULL) {
      HMainStatusBarSetStatusString (L"Read Memory Failed");
      return EFI_OUT_OF_RESOURCES;
    }

    HMainStatusBarSetStatusString (Str);
    HEditorFreePool (Str);

    HMainEditor.SelectStart = 0;
    HMainEditor.SelectEnd   = 0;

  }

  //
  // has line
  //
  if (HBufferImage.Lines != NULL) {
    HBufferImage.CurrentLine = CR (HBufferImage.ListHead->Flink, HEFI_EDITOR_LINE, Link, EFI_EDITOR_LINE_LIST);
  } else {
    //
    // create a dummy line
    //
    Line = HBufferImageCreateLine ();
    if (Line == NULL) {
      HMainStatusBarSetStatusString (L"Read Memory Failed");
      return EFI_OUT_OF_RESOURCES;
    }

    HBufferImage.CurrentLine = Line;
  }

  HBufferImage.Modified           = FALSE;
  HBufferImageNeedRefresh         = TRUE;
  HBufferImageOnlyLineNeedRefresh = FALSE;
  HBufferImageMouseNeedRefresh    = TRUE;

  return EFI_SUCCESS;

}

EFI_STATUS
HMemImageSave (
  IN UINTN Offset,
  IN UINTN Size
  )
/*++

Routine Description: 

  Save lines in HBufferImage to disk

Arguments:  

  Offset - The offset
  Size   - The size

Returns:  

  EFI_SUCCESS
  EFI_LOAD_ERROR
  EFI_OUT_OF_RESOURCES

--*/
{

  EFI_STATUS                      Status;
  VOID                            *Buffer;

  HEFI_EDITOR_ACTIVE_BUFFER_TYPE  BufferTypeBackup;

  //
  // not modified, so directly return
  //
  if (HBufferImage.Modified == FALSE) {
    return EFI_SUCCESS;
  }

  BufferTypeBackup        = HBufferImage.BufferType;
  HBufferImage.BufferType = MEM_BUFFER;

  Buffer                  = AllocatePool (Size);

  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = HBufferImageListToBuffer (Buffer, Size);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return Status;
  }
  //
  // write back to memory
  //
  Status = HMemImage.IoFncs->Mem.Write (
                                  HMemImage.IoFncs,
                                  EfiPciWidthUint8,
                                  Offset,
                                  Size,
                                  Buffer
                                  );

  FreePool (Buffer);

  if (EFI_ERROR (Status)) {
    return EFI_LOAD_ERROR;
  }
  //
  // now not modified
  //
  HBufferImage.Modified = FALSE;

  return EFI_SUCCESS;
}
