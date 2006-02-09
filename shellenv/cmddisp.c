/*++ 

Copyright (c) 2005, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  cmddisp.c
  
Abstract:

  Shell Environment internal command management

Revision History

--*/

#include "shelle.h"
#include "shellenvguid.h"

//
// ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////                                                                  ////
////                        Internal prototype                        ////
////                                                                  ////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
EFI_MONOSHELL_CODE (
  
EFI_STATUS
DriversGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
DrvcfgGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
DrvdiagGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeResetGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeReset (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeCPGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeCP (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeRMGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeRM (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeDateGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeDate (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
DriversMain (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeLSGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeLS (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeMkDirGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeMkDir (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeTimeGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeTime (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeVolGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeVol (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeVerGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeVer (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeTypeGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeType (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeMvGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeMv (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeTouchGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeTouch (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeAttribGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeAttrib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeClsGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeCls (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeLoadGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeLoad (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
InitializeUnloadGetLineHelp (
  OUT CHAR16              **Str
  );

EFI_STATUS
InitializeUnload (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
GuidMain (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  );

EFI_STATUS
OpeninfoMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
DrvcfgMain (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  );

EFI_STATUS
DrvdiagMain (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  );

)

struct {
  SHELLENV_INTERNAL_COMMAND Dispatch;
  CHAR16                    *Cmd;
  SHELLCMD_GET_LINE_HELP    GetLineHelp;
}
SEnvInternalCommands[] = {
  SEnvHelp,
  L"?",
  SEnvHelpGetLineHelp,
  SEnvHelp,
  L"help",
  SEnvHelpGetLineHelp,
  SEnvCmdSet,
  L"set",
  SEnvCmdSetGetLineHelp,
  SEnvCmdAlias,
  L"alias",
  SEnvCmdAliasGetLineHelp,
  SEnvCmdDH,
  L"dh",
  SEnvCmdDHGetLineHelp,
  SEnvCmdConnect,
  L"connect",
  SEnvCmdConnectGetLineHelp,
  SEnvCmdDisconnect,
  L"disconnect",
  SEnvCmdDisconnectGetLineHelp,
  SEnvCmdReconnect,
  L"reconnect",
  SEnvCmdReconnectGetLineHelp,
  SEnvCmdMap,
  L"map",
  SEnvCmdMapGetLineHelp,
  SEnvCmdCd,
  L"cd",
  SEnvCmdCdGetLineHelp,
  SEnvCmdEcho,
  L"echo",
  SEnvCmdEchoGetLineHelp,
  SEnvCmdIf,
  L"if",
  SEnvCmdIfGetLineHelp,
  SEnvCmdElse,
  L"else",
  SEnvCmdElseGetLineHelp,
  SEnvCmdEndif,
  L"endif",
  SEnvCmdEndifGetLineHelp,
  SEnvCmdShift,
  L"shift",
  SEnvCmdShiftGetLineHelp,
  SEnvCmdGoto,
  L"goto",
  SEnvCmdGotoGetLineHelp,
  SEnvCmdFor,
  L"for",
  SEnvCmdForGetLineHelp,
  SEnvCmdEndfor,
  L"endfor",
  SEnvCmdEndforGetLineHelp,
  SEnvCmdPause,
  L"pause",
  SEnvCmdPauseGetLineHelp,
  SEnvExit,
  L"exit",
  SEnvExitGetLineHelp,

  SEnvLoadDefaults,
  L"_load_defaults",
  0,
  SEnvNoUse,
  L"_this_is_not_a_useful_command",
  0,

#ifdef EFI_MONOSHELL
  InitializeUnload,
  L"unload",
  InitializeUnloadGetLineHelp,
  DriversMain,
  L"drivers",
  DriversGetLineHelp,
  DrvcfgMain,
  L"drvcfg",
  DrvcfgGetLineHelp,
  DrvdiagMain,
  L"drvdiag",
  DrvdiagGetLineHelp,
  InitializeReset,
  L"reset",
  InitializeResetGetLineHelp,
  InitializeAttrib,
  L"attrib",
  InitializeAttribGetLineHelp,
  InitializeCls,
  L"cls",
  InitializeClsGetLineHelp,
  InitializeCP,
  L"cp",
  InitializeCPGetLineHelp,
  InitializeDate,
  L"date",
  InitializeDateGetLineHelp,
  InitializeLS,
  L"ls",
  InitializeLSGetLineHelp,
  InitializeMkDir,
  L"mkdir",
  InitializeMkDirGetLineHelp,
  InitializeMv,
  L"mv",
  InitializeMvGetLineHelp,
  InitializeRM,
  L"rm",
  InitializeRMGetLineHelp,
  InitializeVer,
  L"ver",
  InitializeVerGetLineHelp,
  InitializeTouch,
  L"touch",
  InitializeTouchGetLineHelp,
  InitializeTime,
  L"time",
  InitializeTimeGetLineHelp,
  InitializeLoad,
  L"load",
  InitializeLoadGetLineHelp,
  InitializeType,
  L"type",
  InitializeTypeGetLineHelp,
  InitializeVol,
  L"vol",
  InitializeVolGetLineHelp,
#endif

  {
    NULL
  }
};

//
//
//
VOID
SEnvInitCommandTable (
  VOID
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  UINTN Index;

  //
  // Add all of our internal commands to the command dispatch table
  //
  InitializeListHead (&SEnvCmds);
  for (Index = 0; SEnvInternalCommands[Index].Dispatch; Index += 1) {
    SEnvAddCommand (
      SEnvInternalCommands[Index].Dispatch,
      SEnvInternalCommands[Index].Cmd,
      SEnvInternalCommands[Index].GetLineHelp
      );
  }
}

EFI_STATUS
SEnvAddCommand (
  IN SHELLENV_INTERNAL_COMMAND    Handler,
  IN CHAR16                       *CmdStr,
  IN SHELLCMD_GET_LINE_HELP       GetLineHelp
  )
/*++

Routine Description:

Arguments:

  Handler     - Handler
  CmdStr      - The command name
  GetLineHelp - The interface of get line help

Returns:

--*/
{
  COMMAND         *Cmd;
  COMMAND         *Command;
  EFI_LIST_ENTRY  *Link;

  Command = NULL;
  Cmd     = AllocateZeroPool (sizeof (COMMAND));

  if (Cmd) {
    AcquireLock (&SEnvLock);

    Cmd->Signature    = COMMAND_SIGNATURE;
    Cmd->Dispatch     = Handler;
    Cmd->Cmd          = CmdStr;
    Cmd->GetLineHelp  = GetLineHelp;
    InsertTailList (&SEnvCmds, &Cmd->Link);

    //
    // Find the proper place of Cmd
    //
    for (Link = SEnvCmds.Flink; Link != &SEnvCmds; Link = Link->Flink) {
      Command = CR (Link, COMMAND, Link, COMMAND_SIGNATURE);
      if (StriCmp (Command->Cmd, Cmd->Cmd) > 0) {
        //
        //  Insert it to proper place
        //
        SwapListEntries (&Command->Link, &Cmd->Link);
        break;
      }
    }

    ReleaseLock (&SEnvLock);
  }

  return Cmd ? EFI_SUCCESS : EFI_OUT_OF_RESOURCES;
}

EFI_STATUS
SEnvFreeCommandTable (
  VOID
  )
{
  COMMAND *Cmd;

  AcquireLock (&SEnvLock);

  while (!IsListEmpty (&SEnvCmds)) {
    Cmd = CR (SEnvCmds.Flink, COMMAND, Link, COMMAND_SIGNATURE);
    RemoveEntryList (&Cmd->Link);
    FreePool (Cmd);
  }

  ReleaseLock (&SEnvLock);

  return EFI_SUCCESS;
}

SHELLENV_INTERNAL_COMMAND
SEnvGetCmdDispath (
  IN CHAR16                   *CmdName
  )
/*++

Routine Description:

Arguments:

  CmdName - Command name
  
Returns:

--*/
{
  EFI_LIST_ENTRY            *Link;
  COMMAND                   *Command;
  SHELLENV_INTERNAL_COMMAND Dispatch;

  Dispatch = NULL;
  AcquireLock (&SEnvLock);

  //
  // Walk through SEnvCmds linked list
  // to get corresponding command dispatch
  //
  for (Link = SEnvCmds.Flink; Link != &SEnvCmds; Link = Link->Flink) {
    Command = CR (Link, COMMAND, Link, COMMAND_SIGNATURE);
    if (StriCmp (Command->Cmd, CmdName) == 0) {
      Dispatch = Command->Dispatch;
      break;
    }
  }

  ReleaseLock (&SEnvLock);
  return Dispatch;
}
