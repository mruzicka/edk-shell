/*++
 
Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  rm.h

Abstract: 

  declares interface functions

Revision History

--*/

#ifndef _EFI_SHELL_RM_H_
#define _EFI_SHELL_RM_H_

#include "EfiShellLib.h"

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include STRING_DEFINES_FILE

#define EFI_RM_GUID \
  { \
    0x38eec554, 0xd163, 0x49f0, 0xac, 0xfa, 0x24, 0x2f, 0xa9, 0xf8, 0x45, 0xfb \
  }

extern UINT8          STRING_ARRAY_NAME[];

//
// Function declarations
//
extern EFI_HII_HANDLE HiiRmHandle;
extern EFI_GUID       EfiRmGuid;

EFI_STATUS
RmMainProc (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN UINT16               VHlpToken
  );

EFI_STATUS
RemoveRM (
  IN EFI_FILE_HANDLE          FileHandle,
  IN CHAR16                   *FileName,
  IN BOOLEAN                  Quiet
  );

#endif
