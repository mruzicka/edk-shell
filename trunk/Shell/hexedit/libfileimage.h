/*++

Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:
    libFileImage.h

  Abstract:
    Defines FileImage - the view of the file that is visible at any point, 
    as well as the event handlers for editing the file

--*/

#ifndef _LIB_FILE_IMAGE_H_
#define _LIB_FILE_IMAGE_H_

#include "heditortype.h"

EFI_STATUS
HFileImageInit (
  VOID
  );
EFI_STATUS
HFileImageCleanup (
  VOID
  );
EFI_STATUS
HFileImageBackup (
  VOID
  );

EFI_STATUS
HFileImageSetFileName (
  IN CHAR16 *
  );

EFI_STATUS
HFileImageGetFileInfo (
  EFI_FILE_HANDLE,
  CHAR16          *,
  EFI_FILE_INFO   **
  );

EFI_STATUS
HFileImageRead (
  IN CHAR16   *,
  IN          BOOLEAN
  );
EFI_STATUS
HFileImageSave (
  IN CHAR16 *
  );

#endif
