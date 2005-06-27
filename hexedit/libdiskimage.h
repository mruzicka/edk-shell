/*++

Copyright 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:
    libDiskImage.h

  Abstract:
    Defines DiskImage - the view of the file that is visible at any point, 
    as well as the event handlers for editing the file

--*/

#ifndef _LIB_DISK_IMAGE_H_
#define _LIB_DISK_IMAGE_H_

#include "heditortype.h"

EFI_STATUS
HDiskImageInit (
  VOID
  );
EFI_STATUS
HDiskImageCleanup (
  VOID
  );
EFI_STATUS
HDiskImageBackup (
  VOID
  );

EFI_STATUS
HDiskImageSetDiskNameOffsetSize (
  IN CHAR16   *,
  IN          UINTN,
  IN          UINTN
  );

EFI_STATUS
HDiskImageRead (
  IN CHAR16   *,
  IN          UINTN,
  IN          UINTN,
  IN          BOOLEAN
  );
EFI_STATUS
HDiskImageSave (
  IN CHAR16   *,
  IN          UINTN,
  IN          UINTN
  );

#endif
