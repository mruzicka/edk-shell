/*++

Copyright 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:
    libclipboard.c

  Abstract:
    Functions to deal with Clip Board
--*/

#include "heditor.h"

HEFI_EDITOR_CLIPBOARD HClipBoard;

//
// for basic initialization of HClipBoard
//
HEFI_EDITOR_CLIPBOARD HClipBoardConst = {
  NULL,
  0
};

EFI_STATUS
HClipBoardInit (
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
  // basiclly initialize the HDiskImage
  //
  CopyMem (&HClipBoard, &HClipBoardConst, sizeof (HClipBoard));

  return EFI_SUCCESS;
}

EFI_STATUS
HClipBoardCleanup (
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

  HEditorFreePool (HClipBoard.Buffer);

  return EFI_SUCCESS;
}

EFI_STATUS
HClipBoardSet (
  IN UINT8 *Buffer,
  IN UINTN Size
  )
{
  //
  // free the old clipboard buffer
  // and set new clipboard buffer
  //
  HEditorFreePool (HClipBoard.Buffer);
  HClipBoard.Buffer = Buffer;

  HClipBoard.Size   = Size;

  return EFI_SUCCESS;
}

UINTN
HClipBoardGet (
  OUT UINT8  **Buffer
  )
{
  //
  // return the clipboard buffer
  //
  *Buffer = HClipBoard.Buffer;

  return HClipBoard.Size;
}
