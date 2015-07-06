/*++

Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  bcfg.h

Abstract:

  declares interface functions

Revision History

--*/

#ifndef _EFI_SHELL_BCFG_H_
#define _EFI_SHELL_BCFG_H_

#include "EfiShellLib.h"

#define EFI_BCFG_GUID \
  { \
    0xd250e364, 0x51c6, 0x49ed, 0xae, 0xbf, 0x6d, 0x83, 0xf5, 0x13, 0x0f, 0x74 \
  }

//
// EFI Load Options Attributes
//
#define LOAD_OPTION_ACTIVE            0x00000001
#define LOAD_OPTION_FORCE_RECONNECT   0x00000002
#define LOAD_OPTION_HIDDEN            0x00000008
#define LOAD_OPTION_CATEGORY          0x00001F00

#define LOAD_OPTION_CATEGORY_BOOT     0x00000000
#define LOAD_OPTION_CATEGORY_APP      0x00000100

///
/// EFI Boot Key Data
///
typedef union {
  struct {
    ///
    /// Indicates the revision of the EFI_KEY_OPTION structure. This revision level should be 0.
    ///
    UINT32  Revision        : 8;
    ///
    /// Either the left or right Shift keys must be pressed (1) or must not be pressed (0).
    ///
    UINT32  ShiftPressed    : 1;
    ///
    /// Either the left or right Control keys must be pressed (1) or must not be pressed (0).
    ///
    UINT32  ControlPressed  : 1;
    ///
    /// Either the left or right Alt keys must be pressed (1) or must not be pressed (0).
    ///
    UINT32  AltPressed      : 1;
    ///
    /// Either the left or right Logo keys must be pressed (1) or must not be pressed (0).
    ///
    UINT32  LogoPressed     : 1;
    ///
    /// The Menu key must be pressed (1) or must not be pressed (0).
    ///
    UINT32  MenuPressed     : 1;
    ///
    /// The SysReq key must be pressed (1) or must not be pressed (0).
    ///
    UINT32  SysReqPressed    : 1;
    UINT32  Reserved        : 16;
    ///
    /// Specifies the actual number of entries in EFI_KEY_OPTION.Keys, from 0-3. If
    /// zero, then only the shift state is considered. If more than one, then the boot option will
    /// only be launched if all of the specified keys are pressed with the same shift state.
    ///
    UINT32  InputKeyCount   : 2;
  } Options;
  UINT32  PackedValue;
} EFI_BOOT_KEY_DATA;

///
/// EFI Key Option.
///
#pragma pack(1)
typedef struct {
  ///
  /// Specifies options about how the key will be processed.
  ///
  EFI_BOOT_KEY_DATA  KeyData;
  ///
  /// The CRC-32 which should match the CRC-32 of the entire EFI_LOAD_OPTION to
  /// which BootOption refers. If the CRC-32s do not match this value, then this key
  /// option is ignored.
  ///
  UINT32             BootOptionCrc;
  ///
  /// The Boot#### option which will be invoked if this key is pressed and the boot option
  /// is active (LOAD_OPTION_ACTIVE is set).
  ///
  UINT16             BootOption;
  ///
  /// The key codes to compare against those returned by the
  /// EFI_SIMPLE_TEXT_INPUT and EFI_SIMPLE_TEXT_INPUT_EX protocols.
  /// The number of key codes (0-3) is specified by the EFI_KEY_CODE_COUNT field in KeyOptions.
  ///
  //EFI_INPUT_KEY      Keys[];
} EFI_KEY_OPTION;

typedef struct {
  UINT32  Attributes;
  UINT16  FilePathListLength;
  CHAR16  Description[1];
} LOAD_OPTION_HEADER;
#pragma pack()

typedef enum {
  BcfgTargetBootOrder = 0,
  BcfgTargetDriverOrder,
  BcfgTargetMax
} BCFG_OPERATION_TARGET;

typedef enum {
  BcfgTypeDump = 0,
  BcfgTypeAdd,
  BcfgTypeAddh,
  BcfgTypeAddg,
  BcfgTypeMv,
  BcfgTypeSet,
  BcfgTypeRm,
  BcfgTypeOpt,
  BcfgTypeMax
} BCFG_OPERATION_TYPE;

typedef struct {
  BCFG_OPERATION_TARGET  Target;
  BCFG_OPERATION_TYPE    Type;
  UINT16                 Number1;
  UINT16                 Number2;
  CHAR16                 *FileName;
  UINTN                  HandleIndex;
  EFI_GUID               Guid;
  CHAR16                 *Description;
  UINT16                 *Order;
} BGFG_OPERATION;
#endif
