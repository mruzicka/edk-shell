/*++

Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:
  libDiskImage.c

  Abstract:
  Functions to deal with Disk buffer
--*/

#include "heditor.h"

extern EFI_HANDLE                 HImageHandleBackup;
extern HEFI_EDITOR_BUFFER_IMAGE   HBufferImage;

extern BOOLEAN                    HBufferImageNeedRefresh;
extern BOOLEAN                    HBufferImageOnlyLineNeedRefresh;
extern BOOLEAN                    HBufferImageMouseNeedRefresh;

extern HEFI_EDITOR_GLOBAL_EDITOR  HMainEditor;

HEFI_EDITOR_DISK_IMAGE            HDiskImage;
HEFI_EDITOR_DISK_IMAGE            HDiskImageBackupVar;

//
// for basic initialization of HDiskImage
//
HEFI_EDITOR_DISK_IMAGE            HDiskImageConst = {
  NULL,
  0,
  0,
  0
};

EFI_STATUS
HDiskImageInit (
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
  //
  // basically initialize the HDiskImage
  //
  CopyMem (&HDiskImage, &HDiskImageConst, sizeof (HDiskImage));

  CopyMem (&HDiskImageBackupVar, &HDiskImageConst, sizeof (HDiskImageBackupVar));

  return EFI_SUCCESS;
}

EFI_STATUS
HDiskImageBackup (
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

  EFI_SUCCESS          - Success
  EFI_OUT_OF_RESOURCES - Out of resources

--*/
{
  //
  // backup the disk name, offset and size
  //
  //
  HEditorFreePool (HDiskImageBackupVar.Name);

  HDiskImageBackupVar.Name = PoolPrint (L"%s", HDiskImage.Name);
  if (HDiskImageBackupVar.Name == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HDiskImageBackupVar.Offset  = HDiskImage.Offset;
  HDiskImageBackupVar.Size    = HDiskImage.Size;

  return EFI_SUCCESS;
}

EFI_STATUS
HDiskImageCleanup (
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
  HEditorFreePool (HDiskImage.Name);
  HEditorFreePool (HDiskImageBackupVar.Name);

  return EFI_SUCCESS;
}

EFI_STATUS
HDiskImageSetDiskNameOffsetSize (
  IN CHAR16   *Str,
  IN UINTN    Offset,
  IN UINTN    Size
  )
/*++

Routine Description: 

  Set FileName field in HFileImage

Arguments:  

  Str    - File name to set
  Offset - The offset
  Size   - The size

Returns:  

  EFI_SUCCESS
  EFI_OUT_OF_RESOURCES

--*/
{
  UINTN Len;
  UINTN Index;

  //
  // free the old file name
  //
  HEditorFreePool (HDiskImage.Name);

  Len             = StrLen (Str);

  HDiskImage.Name = AllocatePool (2 * (Len + 1));
  if (HDiskImage.Name == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < Len; Index++) {
    HDiskImage.Name[Index] = Str[Index];
  }

  HDiskImage.Name[Len]  = L'\0';

  HDiskImage.Offset     = Offset;
  HDiskImage.Size       = Size;

  return EFI_SUCCESS;
}

EFI_STATUS
HDiskImageRead (
  IN CHAR16   *DeviceName,
  IN UINTN    Offset,
  IN UINTN    Size,
  IN BOOLEAN  Recover
  )
/*++

Routine Description: 

  Read a disk from disk into HBufferImage

Arguments:  

  DeviceName - filename to read
  Offset     - The offset
  Size       - The size
  Recover    - if is for recover, no information print

Returns:  

  EFI_SUCCESS
  EFI_LOAD_ERROR
  EFI_OUT_OF_RESOURCES
  EFI_INVALID_PARAMETER 
  
--*/
{
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_BLOCK_IO_PROTOCOL           *BlkIo;
  EFI_STATUS                      Status;

  VOID                            *Buffer;
  CHAR16                          *Str;
  UINTN                           Bytes;

  HEFI_EDITOR_LINE                *Line;
  UINT64                          ByteOffset;

  HEFI_EDITOR_ACTIVE_BUFFER_TYPE  BufferTypeBackup;

  BufferTypeBackup        = HBufferImage.BufferType;
  HBufferImage.BufferType = DISK_BUFFER;

  DevicePath              = (EFI_DEVICE_PATH_PROTOCOL *) ShellGetMap (DeviceName);
  if (DevicePath == NULL) {
    HMainStatusBarSetStatusString (L"Cannot Find Device");
    return EFI_INVALID_PARAMETER;
  }
  //
  // get blkio interface
  //
  Status = LibDevicePathToInterface (
            &gEfiBlockIoProtocolGuid,
            DevicePath,
            &BlkIo
            );
  if (EFI_ERROR (Status)) {
    HMainStatusBarSetStatusString (L"Read Disk Failed");
    return Status;
  }
  //
  // if Offset exceeds LastBlock,
  //   return error
  //
  if (Offset > BlkIo->Media->LastBlock || Offset + Size > BlkIo->Media->LastBlock) {
    HMainStatusBarSetStatusString (L"Invalid Offset + Size");
    return EFI_LOAD_ERROR;
  }

  Bytes   = BlkIo->Media->BlockSize * Size;
  Buffer  = AllocatePool (Bytes);

  if (Buffer == NULL) {
    HMainStatusBarSetStatusString (L"Read Disk Failed");
    return EFI_OUT_OF_RESOURCES;
  }

  ByteOffset = MultU64x32 (Offset, BlkIo->Media->BlockSize);

  //
  // read from disk
  //
  Status = BlkIo->ReadBlocks (
                    BlkIo,
                    BlkIo->Media->MediaId,
                    Offset,
                    Bytes,
                    Buffer
                    );

  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    HMainStatusBarSetStatusString (L"Read Disk Failed");
    return EFI_LOAD_ERROR;
  }

  HBufferImageFree ();

  //
  // convert buffer to line list
  //
  Status = HBufferImageBufferToList (Buffer, Bytes);
  FreePool (Buffer);

  if (EFI_ERROR (Status)) {
    HMainStatusBarSetStatusString (L"Read Disk Failed");
    return Status;
  }

  Status = HDiskImageSetDiskNameOffsetSize (DeviceName, Offset, Size);
  if (EFI_ERROR (Status)) {
    HMainStatusBarSetStatusString (L"Read Disk Failed");
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // initialize some variables
  //
  HDiskImage.BlockSize                = BlkIo->Media->BlockSize;

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
      HMainStatusBarSetStatusString (L"Read Disk Failed");
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
    HBufferImage.CurrentLine = CR (
                                HBufferImage.ListHead->Flink,
                                HEFI_EDITOR_LINE,
                                Link,
                                EFI_EDITOR_LINE_LIST
                                );
  } else {
    //
    // create a dummy line
    //
    Line = HBufferImageCreateLine ();
    if (Line == NULL) {
      HMainStatusBarSetStatusString (L"Read Disk Failed");
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
HDiskImageSave (
  IN CHAR16 *DeviceName,
  IN UINTN  Offset,
  IN UINTN  Size
  )
/*++

Routine Description: 

  Save lines in HBufferImage to disk
  NOT ALLOW TO WRITE TO ANOTHER DISK!!!!!!!!!

Arguments:  

  DeviceName - The device name
  Offset     - The offset
  Size       - The size

Returns:  

  EFI_SUCCESS
  EFI_LOAD_ERROR
  EFI_OUT_OF_RESOURCES
  EFI_INVALID_PARAMETER

--*/
{

  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  EFI_BLOCK_IO_PROTOCOL           *BlkIo;
  EFI_STATUS                      Status;

  VOID                            *Buffer;
  UINTN                           Bytes;

  UINT64                          ByteOffset;

  HEFI_EDITOR_ACTIVE_BUFFER_TYPE  BufferTypeBackup;

  //
  // if not modified, directly return
  //
  if (HBufferImage.Modified == FALSE) {
    return EFI_SUCCESS;
  }

  BufferTypeBackup        = HBufferImage.BufferType;
  HBufferImage.BufferType = DISK_BUFFER;

  DevicePath              = (EFI_DEVICE_PATH_PROTOCOL *) ShellGetMap (DeviceName);
  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = LibDevicePathToInterface (
            &gEfiBlockIoProtocolGuid,
            DevicePath,
            &BlkIo
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Bytes   = BlkIo->Media->BlockSize * Size;
  Buffer  = AllocatePool (Bytes);

  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // concatenate the line list to a buffer
  //
  Status = HBufferImageListToBuffer (Buffer, Bytes);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return Status;
  }

  ByteOffset = MultU64x32 (Offset, BlkIo->Media->BlockSize);

  //
  // write the buffer to disk
  //
  Status = BlkIo->WriteBlocks (
                    BlkIo,
                    BlkIo->Media->MediaId,
                    Offset,
                    Bytes,
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
