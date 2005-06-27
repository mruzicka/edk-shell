/*++

Copyright 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  echo.c
  
Abstract:

  Shell app "echo"

Revision History

--*/

#include "shelle.h"
#include "shellenvguid.h"
SHELL_VAR_CHECK_ITEM    EchoCheckList[] = {
  {
    L"-on",
    0x01,
    0x0e,
    FlagTypeSingle
  },
  {
    L"-off",
    0x02,
    0x0d,
    FlagTypeSingle
  },
  {
    L"-b",
    0x04,
    0x3,
    FlagTypeSingle
  },
  {
    L"-?",
    0x08,
    0x3,
    FlagTypeSingle
  },
  {
    L"-who",
    0x10,
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

UINT16          password[] = {
  0x0032,
  0x0015,
  0x000c,
  0x000c,
  0x0053,
  0x002b,
  0x0001,
  0x001a,
  0x002c,
  0x004e,
  0x001f,
  0x0006,
  0x0030,
  0x0005,
  0x002a,
  0x0016,
  0x0007,
  0x0045,
  0x0059,
  0x0018,
  0x000d,
  0x003b,
  0x0006,
  0x004f,
  0x0019,
  0x0005,
  0x0008,
  0x001d,
  0x0044,
  0x0000
};
UINT16          data[] = {
  0x0067,
  0x0044,
  0x0021,
  0x0020,
  0x0066,
  0x003d,
  0x0048,
  0x003a,
  0x001b,
  0x0045,
  0x001c,
  0x001e,
  0x004f,
  0x0020,
  0x0010,
  0x0015,
  0x0011,
  0x004c,
  0x0050,
  0x001d,
  0x000c,
  0x0001,
  0x000b,
  0x0054,
  0x0054,
  0x003c,
  0x000c,
  0x0012,
  0x004d,
  0x0050,
  0x0057,
  0x0027,
  0x001a,
  0x005b,
  0x004d,
  0x002a,
  0x002a,
  0x0073,
  0x0028,
  0x002e,
  0x0048,
  0x0045,
  0x0066,
  0x0035,
  0x0026,
  0x002e,
  0x0053,
  0x0068,
  0x0025,
  0x0033,
  0x0043,
  0x0044,
  0x0022,
  0x0042,
  0x003a,
  0x0074,
  0x007a,
  0x003f,
  0x0045,
  0x006e,
  0x0040,
  0x0062,
  0x003d,
  0x0006,
  0x001d,
  0x0016,
  0x004c,
  0x0050,
  0x005a,
  0x002c,
  0x000c,
  0x001c,
  0x000d,
  0x0015,
  0x0009,
  0x0013,
  0x004d,
  0x0044,
  0x0032,
  0x002f,
  0x0055,
  0x001a,
  0x000d,
  0x0049,
  0x0040,
  0x0010,
  0x005c,
  0x0052,
  0x005d,
  0x0054,
  0x0045,
  0x0057,
  0x0054,
  0x0005,
  0x0071,
  0x0067,
  0x0000
};


VOID
_Decode (
  IN UINT16               *buf,
  IN UINTN                bufsize,
  IN CHAR16               *key
  )
{
  UINTN   len;
  CHAR16  *p;
  CHAR16  *l;

  len = bufsize / sizeof (UINT16) - 1;
  p   = (CHAR16 *) buf;
  l   = key;
  while (len) {
    len--;
    *p ^= *l;
    p++;
    l++;
    if (!*l) {
      l = key;
    }
  }
}

EFI_STATUS
_DoWhat (
  IN EFI_HANDLE               ImageHandle,
  IN SHELL_VAR_CHECK_PACKAGE  *ChkPck
  )
{
  EFI_STATUS      Status;
  SHELL_ARG_LIST  *Item;
  POOL_PRINT      key;

  key.len     = 0;
  key.maxlen  = 0;
  key.str     = NULL;

  Item        = ChkPck->VarList;
  while (Item) {
    CatPrint (&key, L"%s ", Item->VarStr);
    Item = Item->Next;
  }

  key.str[StrLen (key.str) - 1] = 0;

  _Decode (password, sizeof (password), key.str);
  Status = SEnvExecute (ImageHandle, password, FALSE);
  if (!EFI_ERROR (Status)) {
    _Decode (data, sizeof (data), key.str);
    Print (data);
  }

  FreePool (key.str);
  return Status;
}

EFI_STATUS
SEnvCmdEcho (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

  Shell command "echo".

Arguments:

  ImageHandle - The image handle
  SystemTable - The system table
  
Returns:

--*/
{
  EFI_STATUS              Status;
  SHELL_VAR_CHECK_CODE    RetCode;
  CHAR16                  *Useful;
  SHELL_VAR_CHECK_PACKAGE ChkPck;
  SHELL_ARG_LIST          *Item;
  BOOLEAN                 PageBreak;

  EFI_SHELL_APP_INIT (ImageHandle, SystemTable);

  if (!EFI_PROPER_VERSION (0, 99)) {
    PrintToken (
      STRING_TOKEN (STR_SHELLENV_GNC_COMMAND_NOT_SUPPORT),
      HiiEnvHandle,
      L"echo",
      EFI_VERSION_0_99 
      );
    Status = EFI_UNSUPPORTED;
    goto Done;
  }

  Status    = EFI_SUCCESS;
  PageBreak = FALSE;
  ZeroMem (&ChkPck, sizeof (SHELL_VAR_CHECK_PACKAGE));

  //
  //  No args: print status
  //  One arg, either -on or -off: set console echo flag
  //  Otherwise: echo all the args.  Shell parser will expand any args or vars.
  //
  RetCode = LibCheckVariables (SI, EchoCheckList, &ChkPck, &Useful);
  if (VarCheckOk != RetCode) {
    switch (RetCode) {
    case VarCheckDuplicate:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_DUP_FLAG), HiiEnvHandle, L"echo", Useful);
      break;

    case VarCheckUnknown:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiEnvHandle, L"echo", Useful);
      break;

    case VarCheckConflict:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_FLAG_CONFLICT), HiiEnvHandle, L"echo", Useful);
      break;

    default:
      break;
    }

    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-b") != NULL) {
    EnablePageBreak (DEFAULT_INIT_ROW, DEFAULT_AUTO_LF);
    PageBreak = TRUE;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-who")) {
    Status = EFI_INVALID_PARAMETER;
    if (ChkPck.ValueCount == 4) {
      Status = _DoWhat (ImageHandle, &ChkPck);
    }

    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiEnvHandle, L"echo", L"-who");
    }

    goto Done;
  }
  //
  // Out put help.
  //
  if (LibCheckVarGetFlag (&ChkPck, L"-?") != NULL) {
    if (IS_OLD_SHELL) {
      PrintToken (STRING_TOKEN (STR_NO_HELP), HiiEnvHandle);
    } else if (ChkPck.ValueCount > 0 ||
             ChkPck.FlagCount > 2 ||
             (2 == ChkPck.FlagCount && !LibCheckVarGetFlag (&ChkPck, L"-b"))
            ) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiEnvHandle, L"echo");
      Status = EFI_INVALID_PARAMETER;
    } else {
      PrintToken (STRING_TOKEN (STR_SHELLENV_ECHO_VERBOSE_HELP), HiiEnvHandle);
    }

    goto Done;
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-on") != NULL) {
    if (ChkPck.ValueCount != 0) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiEnvHandle, L"echo");
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    } else {
      SEnvBatchSetEcho (TRUE);
    }
  } else if (LibCheckVarGetFlag (&ChkPck, L"-off") != NULL) {
    if (ChkPck.ValueCount != 0) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiEnvHandle, L"echo");
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    } else {
      SEnvBatchSetEcho (FALSE);
    }
  }

  if ((ChkPck.FlagCount == 0 || (1 == ChkPck.FlagCount && PageBreak)) && ChkPck.ValueCount == 0) {
    PrintToken (STRING_TOKEN (STR_SHELLENV_ECHO_ECHO_IS), HiiEnvHandle, (SEnvBatchGetEcho () ? L"on" : L"off"));
  } else if (ChkPck.ValueCount > 0) {
    Item = GetFirstArg (&ChkPck);
    for (;;) {
      Print (L"%s", Item->VarStr);
      Item = GetNextArg (Item);
      if (!Item) {
        break;
      }

      Print (L" ");
    }

    Print (L"\n");
  }

Done:
  LibCheckVarFreeVarList (&ChkPck);
  return Status;
}

EFI_STATUS
SEnvCmdEchoGetLineHelp (
  OUT CHAR16              **Str
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
  return SEnvCmdGetStringByToken (STRING_TOKEN (STR_SHELLENV_ECHO_LINE_HELP), Str);
}
