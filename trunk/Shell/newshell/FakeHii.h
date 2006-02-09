/*++

Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  FakeHii.h

Abstract:

  HII support for running under non-Tiano environment
  According to the requirement that Tiano shell.efi should be able to run under EFI1.1, we should embed HII support inside
  shell, because there is no native HII support in EFI1.1.
  We are just to impelment a subset of the complete HII, because not all the features of HII are required in shell

Revision History

--*/

#ifndef _EFI_SHELL_FAKE_HII_H
#define _EFI_SHELL_FAKE_HII_H

#include "EfiShellLib.h"

#include EFI_PROTOCOL_DEFINITION (Hii)
#include EFI_GUID_DEFINITION (GlobalVariable)

#define EFI_FAKE_HII_DATA_SIGNATURE     EFI_SIGNATURE_32 ('F', 'H', 'I', 'I')

#define EFI_HII_PROTOCOL_GUID_OLD \
  { \
    0xcd361957, 0xafbe, 0x425e, 0xa3, 0x58, 0x5f, 0x58, 0x89, 0xcf, 0xfe, 0x7b \
  }

#define DEFAULT_FORM_BUFFER_SIZE    0xFFFF
#define DEFAULT_STRING_BUFFER_SIZE  0xFFFF

#define MAX_GLYPH_COUNT             65535
#define NARROW_GLYPH_ARRAY_SIZE     19
#define WIDE_GLYPH_ARRAY_SIZE       38

#define NARROW_WIDTH                8
#define WIDE_WIDTH                  16

typedef struct {
  EFI_HII_IFR_PACK       *IfrPack;
  EFI_HII_FONT_PACK      *FontPack;
  EFI_HII_STRING_PACK    *StringPack;
  EFI_HII_KEYBOARD_PACK  *KeyboardPack;
  EFI_GUID               *GuidId;
} EFI_HII_PACK_LIST;

typedef
EFI_STATUS
(EFIAPI *EFI_HII_NEW_PACK_OLD) (
  IN  EFI_HII_PROTOCOL    *This,
  IN  EFI_HII_PACK_LIST   *Package,
  OUT EFI_HII_HANDLE      *Handle
  );

typedef struct _EFI_HII_PROTOCOL_OLD {
  EFI_HII_NEW_PACK_OLD          NewPack;
  EFI_HII_REMOVE_PACK           RemovePack;
  EFI_HII_FIND_HANDLES          FindHandles;
  EFI_HII_EXPORT                ExportDatabase;

  EFI_HII_TEST_STRING           TestString;
  EFI_HII_GET_GLYPH             GetGlyph;
  EFI_HII_GLYPH_TO_BLT          GlyphToBlt;

  EFI_HII_NEW_STRING            NewString;
  EFI_HII_GET_PRI_LANGUAGES     GetPrimaryLanguages;
  EFI_HII_GET_SEC_LANGUAGES     GetSecondaryLanguages;
  EFI_HII_GET_STRING            GetString;
  EFI_HII_RESET_STRINGS         ResetStrings;
  EFI_HII_GET_LINE              GetLine;
  EFI_HII_GET_FORMS             GetForms;
  EFI_HII_GET_DEFAULT_IMAGE     GetDefaultImage;
  EFI_HII_UPDATE_FORM           UpdateForm;

  EFI_HII_GET_KEYBOARD_LAYOUT   GetKeyboardLayout;
} EFI_HII_PROTOCOL_OLD;

typedef struct {
  EFI_HII_PACK_HEADER   Header;
  EFI_IFR_FORM_SET      FormSet;
  EFI_IFR_END_FORM_SET  EndFormSet;
} EFI_FORM_SET_STUB;

typedef struct {
  EFI_NARROW_GLYPH    NarrowGlyphs[MAX_GLYPH_COUNT];
  EFI_WIDE_GLYPH      WideGlyphs[MAX_GLYPH_COUNT];
  EFI_KEY_DESCRIPTOR  SystemKeyboardLayout[106];
  EFI_KEY_DESCRIPTOR  OverrideKeyboardLayout[106];
  BOOLEAN             SystemKeyboardUpdate;             // Has the SystemKeyboard been updated?
} EFI_FAKE_HII_GLOBAL_DATA;

typedef struct _EFI_FAKE_HII_HANDLE_DATABASE {
  VOID                                  *Buffer;        // Actual buffer pointer
  EFI_HII_HANDLE                        Handle;         // Monotonically increasing value to signify the value returned to caller
  UINT32                                NumberOfTokens; // The initial number of tokens when first registered
  struct _EFI_FAKE_HII_HANDLE_DATABASE  *NextHandleDatabase;
} EFI_FAKE_HII_HANDLE_DATABASE;

typedef struct _EFI_FAKE_HII_DATA_OLD {
  UINTN                         Signature;
  EFI_FAKE_HII_GLOBAL_DATA      *GlobalData;
  EFI_FAKE_HII_HANDLE_DATABASE  *DatabaseHead;          // Head of the Null-terminated singly-linked list of handles.
  EFI_HII_PROTOCOL_OLD          Hii;
} EFI_FAKE_HII_DATA_OLD;

typedef struct _EFI_FAKE_HII_DATA {
  UINTN                         Signature;
  EFI_FAKE_HII_GLOBAL_DATA      *GlobalData;
  EFI_FAKE_HII_HANDLE_DATABASE  *DatabaseHead;          // Head of the Null-terminated singly-linked list of handles.
  EFI_HII_PROTOCOL              Hii;
} EFI_FAKE_HII_DATA;

typedef struct {
  EFI_HII_HANDLE      Handle;
  EFI_GUID            Guid;
  EFI_HII_HANDLE_PACK HandlePack;
  UINTN               IfrSize;
  UINTN               StringSize;
  EFI_HII_IFR_PACK    *IfrData;                         // All the IFR data stored here
  EFI_HII_STRING_PACK *StringData;                      // All the String data stored at &IfrData + IfrSize (StringData is just a label - never referenced)
} EFI_FAKE_HII_PACKAGE_INSTANCE;

#define EFI_FAKE_HII_DATA_FROM_THIS(a)      CR (a, EFI_FAKE_HII_DATA, Hii, EFI_FAKE_HII_DATA_SIGNATURE)
#define EFI_FAKE_HII_DATA_OLD_FROM_THIS(a)  CR (a, EFI_FAKE_HII_DATA_OLD, Hii, EFI_FAKE_HII_DATA_SIGNATURE)

EFI_STATUS
FakeInitializeHiiDatabase (
  IN     EFI_HANDLE                     ImageHandle,
  IN     EFI_SYSTEM_TABLE               *SystemTable
  );

EFI_STATUS
FakeUninstallHiiDatabase (
  VOID
  );

EFI_STATUS
FakeHiiNewPackOld (
  IN EFI_HII_PROTOCOL               *This,
  IN EFI_HII_PACK_LIST              *PackageList,
  OUT EFI_HII_HANDLE                *Handle
  );

EFI_STATUS
FakeHiiNewPack (
  IN EFI_HII_PROTOCOL               *This,
  IN EFI_HII_PACKAGES               *PackageList,
  OUT EFI_HII_HANDLE                *Handle
  );

EFI_STATUS
FakeHiiRemovePack (
  IN EFI_HII_PROTOCOL                   *This,
  IN EFI_HII_HANDLE                     Handle
  );

EFI_STATUS
FakeHiiFindHandles (
  IN EFI_HII_PROTOCOL                     *This,
  IN OUT UINT16                           *HandleBufferLength,
  OUT EFI_HII_HANDLE                      *Handle
  );

EFI_STATUS
FakeHiiGetString (
  IN EFI_HII_PROTOCOL                       *This,
  IN EFI_HII_HANDLE                         Handle,
  IN STRING_REF                             Token,
  IN BOOLEAN                                Raw,
  IN CHAR16                                 *LanguageString,
  IN OUT UINTN                              *BufferLength,
  OUT EFI_STRING                            StringBuffer
  );

EFI_STATUS
FakeHiiGetPrimaryLanguages (
  IN EFI_HII_PROTOCOL                     *This,
  IN EFI_HII_HANDLE                       Handle,
  OUT EFI_STRING                          *LanguageString
  );

EFI_STATUS
FakeHiiGetForms (
  IN     EFI_HII_PROTOCOL   *This,
  IN     EFI_HII_HANDLE     Handle,
  IN     EFI_FORM_ID        FormId,
  IN OUT UINTN              *BufferLength,
  OUT    UINT8              *Buffer
  );

extern BOOLEAN  gHiiInitialized;

#endif
