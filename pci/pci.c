/*++

Copyright (c) 2005 - 2010, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  pci.c
  
Abstract: 
  EFI shell command "pci"

Revision History

--*/

#include "EfiShellLib.h"
#include "pci22.h"
#include "pci_class.h"
#include "pci.h"
#include "Acpi.h"

extern UINT8      STRING_ARRAY_NAME[];

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include STRING_DEFINES_FILE

#include EFI_PROTOCOL_DEFINITION (PciRootBridgeIo)

EFI_STATUS
PciDump (
  IN EFI_HANDLE                             ImageHandle,
  IN EFI_SYSTEM_TABLE                       *SystemTable
  );

EFI_STATUS
PciFindProtocolInterface (
  IN  EFI_HANDLE                            *HandleBuf,
  IN  UINTN                                 HandleCount,
  IN  UINT16                                Segment,
  IN  UINT16                                Bus,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev
  );

EFI_STATUS
PciGetProtocolAndResource (
  IN  EFI_HANDLE                            Handle,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev,
  OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     **Descriptors
  );

EFI_STATUS
PciGetNextBusRange (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  **Descriptors,
  OUT    UINT16                             *MinBus,
  OUT    UINT16                             *MaxBus,
  OUT    BOOLEAN                            *IsEnd
  );

EFI_STATUS
PciExplainData (
  IN PCI_CONFIG_SPACE                       *ConfigSpace,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  );

EFI_STATUS
PciExplainDeviceData (
  IN PCI_DEVICE_HEADER                      *Device,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  );

EFI_STATUS
PciExplainBridgeData (
  IN PCI_BRIDGE_HEADER                      *Bridge,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  );

EFI_STATUS
PciExplainBar (
  IN UINT32                                 *Bar,
  IN UINT16                                 *Command,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev,
  IN OUT UINTN                              *Index
  );

EFI_STATUS
PciExplainCardBusData (
  IN PCI_CARDBUS_HEADER                     *CardBus,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  );

EFI_STATUS
PciExplainStatus (
  IN UINT16                                 *Status,
  IN BOOLEAN                                MainStatus,
  IN PCI_HEADER_TYPE                        HeaderType
  );

EFI_STATUS
PciExplainCommand (
  IN UINT16                                 *Command
  );

EFI_STATUS
PciExplainBridgeControl (
  IN UINT16                                 *BridgeControl,
  IN PCI_HEADER_TYPE                        HeaderType
  );

EFI_STATUS
PciExplainCapabilityStruct (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *IoDev,
  IN UINT64                                   Address,
  IN  UINT8                                   CapPtr
  );

EFI_STATUS
PciExplainPciExpress (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *IoDev,
  IN  UINT64                                  Address,
  IN  UINT8                                   CapabilityPtr
  );

EFI_STATUS
ExplainPcieCapReg (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieDeviceCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieDeviceControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieDeviceStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieLinkCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieLinkControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieLinkStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieSlotCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieSlotControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieSlotStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieRootControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieRootCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

EFI_STATUS
ExplainPcieRootStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
);

typedef EFI_STATUS (*PCIE_EXPLAIN_FUNCTION) (IN PCIE_CAP_STURCTURE *PciExpressCap);

typedef enum {
  FieldWidthUINT8,
  FieldWidthUINT16,
  FieldWidthUINT32
} PCIE_CAPREG_FIELD_WIDTH;

typedef enum {
  PcieExplainTypeCommon,
  PcieExplainTypeDevice,
  PcieExplainTypeLink,
  PcieExplainTypeSlot,
  PcieExplainTypeRoot,
  PcieExplainTypeMax
} PCIE_EXPLAIN_TYPE;

typedef struct
{
  UINT16                  Token;
  UINTN                   Offset;
  PCIE_CAPREG_FIELD_WIDTH Width;
  PCIE_EXPLAIN_FUNCTION   Func;
  PCIE_EXPLAIN_TYPE       Type;
} PCIE_EXPLAIN_STRUCT;

PCIE_EXPLAIN_STRUCT PcieExplainList[] = {
  {
    STRING_TOKEN (STR_PCIEX_CAPABILITY_CAPID),
    0x00,
    FieldWidthUINT8,
    NULL,
    PcieExplainTypeCommon
  },
  {
    STRING_TOKEN (STR_PCIEX_NEXTCAP_PTR),
    0x01,
    FieldWidthUINT8,
    NULL,
    PcieExplainTypeCommon
  },
  {
    STRING_TOKEN (STR_PCIEX_CAP_REGISTER),
    0x02,
    FieldWidthUINT16,
    ExplainPcieCapReg,
    PcieExplainTypeCommon
  },
  {
    STRING_TOKEN (STR_PCIEX_DEVICE_CAP),
    0x04,
    FieldWidthUINT32,
    ExplainPcieDeviceCap,
    PcieExplainTypeDevice
  },
  {
    STRING_TOKEN (STR_PCIEX_DEVICE_CONTROL),
    0x08,
    FieldWidthUINT16,
    ExplainPcieDeviceControl,
    PcieExplainTypeDevice
  },
  {
    STRING_TOKEN (STR_PCIEX_DEVICE_STATUS),
    0x0a,
    FieldWidthUINT16,
    ExplainPcieDeviceStatus,
    PcieExplainTypeDevice
  },
  {
    STRING_TOKEN (STR_PCIEX_LINK_CAPABILITIES),
    0x0c,
    FieldWidthUINT32,
    ExplainPcieLinkCap,
    PcieExplainTypeLink
  },
  {
    STRING_TOKEN (STR_PCIEX_LINK_CONTROL),
    0x10,
    FieldWidthUINT16,
    ExplainPcieLinkControl,
    PcieExplainTypeLink
  },
  {
    STRING_TOKEN (STR_PCIEX_LINK_STATUS),
    0x12,
    FieldWidthUINT16,
    ExplainPcieLinkStatus,
    PcieExplainTypeLink
  },
  {
    STRING_TOKEN (STR_PCIEX_SLOT_CAPABILITIES),
    0x14,
    FieldWidthUINT32,
    ExplainPcieSlotCap,
    PcieExplainTypeSlot
  },
  {
    STRING_TOKEN (STR_PCIEX_SLOT_CONTROL),
    0x18,
    FieldWidthUINT16,
    ExplainPcieSlotControl,
    PcieExplainTypeSlot
  },
  {
    STRING_TOKEN (STR_PCIEX_SLOT_STATUS),
    0x1a,
    FieldWidthUINT16,
    ExplainPcieSlotStatus,
    PcieExplainTypeSlot
  },
  {
    STRING_TOKEN (STR_PCIEX_ROOT_CONTROL),
    0x1c,
    FieldWidthUINT16,
    ExplainPcieRootControl,
    PcieExplainTypeRoot
  },
  {
    STRING_TOKEN (STR_PCIEX_RSVDP),
    0x1e,
    FieldWidthUINT16,
    ExplainPcieRootCap,
    PcieExplainTypeRoot
  },
  {
    STRING_TOKEN (STR_PCIEX_ROOT_STATUS),
    0x20,
    FieldWidthUINT32,
    ExplainPcieRootStatus,
    PcieExplainTypeRoot
  },
  {
    0,
    0,
    0,
    NULL,
    PcieExplainTypeMax
  }
};

//
// Global Variables
//
PCI_CONFIG_SPACE  *mConfigSpace;
EFI_HII_HANDLE    HiiHandle;
EFI_GUID          EfiPciGuid = EFI_PCI_GUID;
SHELL_VAR_CHECK_ITEM    PciCheckList[] = {
  {
    L"-s",
    0x01,
    0,
    FlagTypeNeedVar
  },
  {
    L"-i",
    0x02,
    0,
    FlagTypeSingle
  },
  {
    L"-b",
    0x04,
    0,
    FlagTypeSingle
  },
  {
    L"-?",
    0x08,
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

CHAR16 *DevicePortTypeTable[] = {
  L"PCI Express Endpoint",
  L"Legacy PCI Express Endpoint",
  L"Unknown Type",
  L"Unknonw Type",
  L"Root Port of PCI Express Root Complex",
  L"Upstream Port of PCI Express Switch",
  L"Downstream Port of PCI Express Switch",
  L"PCI Express to PCI/PCI-X Bridge",
  L"PCI/PCI-X to PCI Express Bridge",
  L"Root Complex Integrated Endpoint",
  L"Root Complex Event Collector"
};

CHAR16 *L0sLatencyStrTable[] = {
  L"Less than 64ns",
  L"64ns to less than 128ns",
  L"128ns to less than 256ns",
  L"256ns to less than 512ns",
  L"512ns to less than 1us",
  L"1us to less than 2us",
  L"2us-4us",
  L"More than 4us"
};

CHAR16 *L1LatencyStrTable[] = {
  L"Less than 1us",
  L"1us to less than 2us",
  L"2us to less than 4us",
  L"4us to less than 8us",
  L"8us to less than 16us",
  L"16us to less than 32us",
  L"32us-64us",
  L"More than 64us"
};

CHAR16 *ASPMCtrlStrTable[] = {
  L"Disabled",
  L"L0s Entry Enabled",
  L"L1 Entry Enabled",
  L"L0s and L1 Entry Enabled"
};

CHAR16 *SlotPwrLmtScaleTable[] = {
  L"1.0x",
  L"0.1x",
  L"0.01x",
  L"0.001x"
};

CHAR16 *IndicatorTable[] = {
  L"Reserved",
  L"On",
  L"Blink",
  L"Off"
};

EFI_BOOTSHELL_CODE(
  EFI_APPLICATION_ENTRY_POINT(PciDump)
)

EFI_STATUS
PciDump (
  IN EFI_HANDLE                             ImageHandle,
  IN EFI_SYSTEM_TABLE                       *SystemTable
  )
/*++

Routine Description:

  Command entry point. Parses command line arguments and execute it. If 
  needed, calls internal function to perform certain operation.

Arguments:

  ImageHandle     The image handle.
  SystemTable     The system table.

Returns:

  EFI_SUCCESS           - The command completed successfully
  EFI_INVALID_PARAMETER - Invalid parameter
  EFI_OUT_OF_RESOURCES  - Out of resources

--*/
{
  UINT16                            Segment;
  UINT16                            Bus;
  UINT16                            Device;
  UINT16                            Func;
  UINT64                            Address;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *IoDev;
  EFI_STATUS                        Status;
  PCI_COMMON_HEADER                 PciHeader;
  PCI_CONFIG_SPACE                  ConfigSpace;
  UINTN                             ScreenCount;
  UINTN                             TempColumn;
  UINTN                             ScreenSize;
  BOOLEAN                           ExplainData;
  UINTN                             Index;
  UINTN                             SizeOfHeader;
  BOOLEAN                           PrintTitle;
  UINTN                             HandleBufSize;
  EFI_HANDLE                        *HandleBuf;
  UINTN                             HandleCount;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            MinBus;
  UINT16                            MaxBus;
  BOOLEAN                           IsEnd;

  SHELL_VAR_CHECK_CODE              RetCode;
  CHAR16                            *Useful;
  SHELL_ARG_LIST                    *Item;
  SHELL_VAR_CHECK_PACKAGE           ChkPck;

  IoDev = NULL;
  HandleBuf = NULL;
  ZeroMem (&ChkPck, sizeof (SHELL_VAR_CHECK_PACKAGE));
  EFI_SHELL_APP_INIT (ImageHandle, SystemTable);
  
  //
  // Enable tab key which can pause the output
  //
  EnableOutputTabPause();

  Status = LibInitializeStrings (&HiiHandle, STRING_ARRAY_NAME, &EfiPciGuid);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (!EFI_PROPER_VERSION (0, 99)) {
    PrintToken (
      STRING_TOKEN (STR_SHELLENV_GNC_COMMAND_NOT_SUPPORT),
      HiiHandle,
      L"pci",
      EFI_VERSION_0_99 
      );
    Status = EFI_UNSUPPORTED;
    goto Done;
  }

  RetCode = LibCheckVariables (SI, PciCheckList, &ChkPck, &Useful);
  if (VarCheckOk != RetCode) {
    switch (RetCode) {
    case VarCheckUnknown:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_UNKNOWN_FLAG), HiiHandle, L"pci", Useful);
      break;

    case VarCheckDuplicate:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_DUP_FLAG), HiiHandle, L"pci", Useful);
      break;

    case VarCheckLackValue:
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_LACK_ARG), HiiHandle, L"pci", Useful);
      break;

    default:
      break;
    }

    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  //
  // Out put help.
  //
  if (LibCheckVarGetFlag (&ChkPck, L"-b") != NULL) {
    EnablePageBreak (DEFAULT_INIT_ROW, DEFAULT_AUTO_LF);
  }

  if (LibCheckVarGetFlag (&ChkPck, L"-?") != NULL) {
    if (ChkPck.ValueCount > 0 ||
        ChkPck.FlagCount > 2 ||
        (2 == ChkPck.FlagCount && !LibCheckVarGetFlag (&ChkPck, L"-b"))
        ) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_TOO_MANY), HiiHandle, L"pci");
      Status = EFI_INVALID_PARAMETER;
    } else {
      PrintToken (STRING_TOKEN (STR_HELPINFO_PCI_VERBOSEHELP), HiiHandle);
      Status = EFI_SUCCESS;
    }

    goto Done;
  }
  //
  // Get all instances of PciRootBridgeIo. Allocate space for 1 EFI_HANDLE and
  // call LibLocateHandle(), if EFI_BUFFER_TOO_SMALL is returned, allocate enough
  // space for handles and call it again.
  //
  HandleBufSize = sizeof (EFI_HANDLE);
  HandleBuf     = (EFI_HANDLE *) AllocatePool (HandleBufSize);
  if (HandleBuf == NULL) {
    PrintToken (STRING_TOKEN (STR_PCI2_OUT_RESOURCES), HiiHandle);
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = BS->LocateHandle (
                ByProtocol,
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &HandleBufSize,
                HandleBuf
                );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    HandleBuf = ReallocatePool (HandleBuf, sizeof (EFI_HANDLE), HandleBufSize);
    if (HandleBuf == NULL) {
      PrintToken (STRING_TOKEN (STR_PCI2_OUT_RESOURCES), HiiHandle);
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Status = BS->LocateHandle (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &HandleBufSize,
                  HandleBuf
                  );
  }

  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_PCI2_LOCATE_HANDLE), HiiHandle, Status);
    goto Done;
  }

  HandleCount = HandleBufSize / sizeof (EFI_HANDLE);
  //
  // Argument Count == 1(no other argument): enumerate all pci functions
  //
  if (ChkPck.ValueCount == 0) {
    ST->ConOut->QueryMode (
                  ST->ConOut,
                  ST->ConOut->Mode->Mode,
                  &TempColumn,
                  &ScreenSize
                  );

    ScreenCount = 0;
    ScreenSize -= 4;
    if ((ScreenSize & 1) == 1) {
      ScreenSize -= 1;
    }

    PrintTitle = TRUE;

    //
    // For each handle, which decides a segment and a bus number range,
    // enumerate all devices on it.
    //
    for (Index = 0; Index < HandleCount; Index++) {
      Status = PciGetProtocolAndResource (
                HandleBuf[Index],
                &IoDev,
                &Descriptors
                );
      if (EFI_ERROR (Status)) {
        PrintToken (STRING_TOKEN (STR_PCI2_HANDLE_PROTOCOL), HiiHandle, Status);
        goto Done;
      }
      //
      // No document say it's impossible for a RootBridgeIo protocol handle
      // to have more than one address space descriptors, so find out every
      // bus range and for each of them do device enumeration.
      //
      while (TRUE) {
        Status = PciGetNextBusRange (&Descriptors, &MinBus, &MaxBus, &IsEnd);

        if (EFI_ERROR (Status)) {
          PrintToken (STRING_TOKEN (STR_PCI2_BUS), HiiHandle, Status);
          goto Done;
        }

        if (IsEnd) {
          break;
        }

        for (Bus = MinBus; Bus <= MaxBus; Bus++) {
          //
          // For each devices, enumerate all functions it contains
          //
          for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
            //
            // For each function, read its configuration space and print summary
            //
            for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
              if (GetExecutionBreak ()) {
                Status = EFI_SUCCESS;
                goto Done;
              }
              Address = CALC_EFI_PCI_ADDRESS (Bus, Device, Func, 0);
              IoDev->Pci.Read (
                          IoDev,
                          EfiPciWidthUint16,
                          Address,
                          1,
                          &PciHeader.VendorId
                          );

              //
              // If VendorId = 0xffff, there does not exist a device at this
              // location. For each device, if there is any function on it,
              // there must be 1 function at Function 0. So if Func = 0, there
              // will be no more functions in the same device, so we can break
              // loop to deal with the next device.
              //
              if (PciHeader.VendorId == 0xffff && Func == 0) {
                break;
              }

              if (PciHeader.VendorId != 0xffff) {

                if (PrintTitle) {
                  PrintToken (STRING_TOKEN (STR_PCI2_SEG_BUS), HiiHandle);
                  Print (L"   ---  ---  ---  ----\n");
                  PrintTitle = FALSE;
                }

                IoDev->Pci.Read (
                            IoDev,
                            EfiPciWidthUint32,
                            Address,
                            sizeof (PciHeader) / sizeof (UINT32),
                            &PciHeader
                            );

                PrintToken (
                  STRING_TOKEN (STR_PCI2_FOUR_VARS),
                  HiiHandle,
                  IoDev->SegmentNumber,
                  Bus,
                  Device,
                  Func
                  );

                PciPrintClassCode (PciHeader.ClassCode, FALSE);
                PrintToken (
                  STRING_TOKEN (STR_PCI2_VENDOR),
                  HiiHandle,
                  PciHeader.VendorId,
                  PciHeader.DeviceId,
                  PciHeader.ClassCode[0]
                  );

                ScreenCount += 2;
                if (ScreenCount >= ScreenSize && ScreenSize != 0) {
                  //
                  // If ScreenSize == 0 we have the console redirected so don't
                  //  block updates
                  //
                  ScreenCount = 0;
                }
                //
                // If this is not a multi-function device, we can leave the loop
                // to deal with the next device.
                //
                if (Func == 0 && ((PciHeader.HeaderType & HEADER_TYPE_MULTI_FUNCTION) == 0x00)) {
                  break;
                }
              }
            }
          }
        }
        //
        // If Descriptor is NULL, Configuration() returns EFI_UNSUPPRORED,
        // we assume the bus range is 0~PCI_MAX_BUS. After enumerated all
        // devices on all bus, we can leave loop.
        //
        if (Descriptors == NULL) {
          break;
        }
      }
    }

    Status = EFI_SUCCESS;
    goto Done;
  }

  if (ChkPck.ValueCount == 1) {
    PrintToken (STRING_TOKEN (STR_PCI2_TOO_FEW_ARGS), HiiHandle);
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }
  //
  // Arg count >= 3, dump binary of specified function, interpret if necessary
  //
  if (ChkPck.ValueCount > 3) {
    PrintToken (STRING_TOKEN (STR_PCI2_TOO_MANY_ARGS), HiiHandle);
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  ExplainData                   = FALSE;
  Segment                       = 0;
  Bus                           = 0;
  Device                        = 0;
  Func                          = 0;
  if (LibCheckVarGetFlag (&ChkPck, L"-i") != NULL) {
    ExplainData = TRUE;
  }

  Item = LibCheckVarGetFlag (&ChkPck, L"-s");
  if (NULL != Item) {
    Segment = (UINT16) StrToUIntegerBase (Item->VarStr, 16, &Status);
    if (EFI_ERROR (Status)) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, L"pci", Item->VarStr);
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
  }

  //
  // The first Argument(except "-i") is assumed to be Bus number, second
  // to be Device number, and third to be Func number.
  //
  Item = ChkPck.VarList;
  if (NULL != Item) {
    Bus = (UINT16) StrToUIntegerBase (Item->VarStr, 16, &Status);
    if (EFI_ERROR (Status) || Bus > MAX_BUS_NUMBER) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, L"pci", Item->VarStr);
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }

    Item = Item->Next;
  }

  if (NULL != Item) {
    Device = (UINT16) StrToUIntegerBase (Item->VarStr, 16, &Status);
    if (EFI_ERROR (Status) || Device > MAX_DEVICE_NUMBER) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, L"pci", Item->VarStr);
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }

    Item = Item->Next;
  }

  if (NULL != Item) {
    Func = (UINT16) StrToUIntegerBase (Item->VarStr, 16, &Status);
    if (EFI_ERROR (Status) || Func > MAX_FUNCTION_NUMBER) {
      PrintToken (STRING_TOKEN (STR_SHELLENV_GNC_INVALID_ARG), HiiHandle, L"pci", Item->VarStr);
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
  }


  //
  // Find the protocol interface who's in charge of current segment, and its
  // bus range covers the current bus
  //
  Status = PciFindProtocolInterface (
            HandleBuf,
            HandleCount,
            Segment,
            Bus,
            &IoDev
            );

  if (EFI_ERROR (Status)) {
    PrintToken (
      STRING_TOKEN (STR_PCI2_CANNOT_FIND_PROTOCOL),
      HiiHandle,
      Segment,
      Bus
      );

    goto Done;
  }

  Address = CALC_EFI_PCI_ADDRESS (Bus, Device, Func, 0);
  Status = IoDev->Pci.Read (
                        IoDev,
                        EfiPciWidthUint8,
                        Address,
                        sizeof (ConfigSpace),
                        &ConfigSpace
                        );

  if (EFI_ERROR (Status)) {
    PrintToken (STRING_TOKEN (STR_PCI2_CANNOT_READ_CONFIG), HiiHandle, Status);
    goto Done;
  }

  mConfigSpace = &ConfigSpace;
  PrintToken (
    STRING_TOKEN (STR_PCI2_SEGMENT_BUS),
    HiiHandle,
    Segment,
    Bus,
    Device,
    Func,
    Segment,
    Bus,
    Device,
    Func
    );

  //
  // Dump standard header of configuration space
  //
  SizeOfHeader = sizeof (ConfigSpace.Common) + sizeof (ConfigSpace.NonCommon);

  PrivateDumpHex (2, 0, SizeOfHeader, &ConfigSpace);
  Print (L"\n");

  //
  // Dump device dependent Part of configuration space
  //
  PrivateDumpHex (
    2,
    SizeOfHeader,
    sizeof (ConfigSpace) - SizeOfHeader,
    ConfigSpace.Data
    );

  //
  // If "-i" appears in command line, interpret data in configuration space
  //
  if (ExplainData) {
    Status = PciExplainData (&ConfigSpace, Address, IoDev);
  }
Done:
  if (HandleBuf != NULL) {
    FreePool (HandleBuf);
  }
  LibCheckVarFreeVarList (&ChkPck);
  LibUnInitializeStrings ();
  return Status;
}

EFI_STATUS
PciFindProtocolInterface (
  IN  EFI_HANDLE                            *HandleBuf,
  IN  UINTN                                 HandleCount,
  IN  UINT16                                Segment,
  IN  UINT16                                Bus,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev
  )
/*++

Routine Description:

  This function finds out the protocol which is in charge of the given 
  segment, and its bus range covers the current bus number. It lookes 
  each instances of RootBridgeIoProtocol handle, until the one meets the
  criteria is found. 

Arguments:

  HandleBuf       Buffer which holds all PCI_ROOT_BRIDIGE_IO_PROTOCOL handles
  HandleCount     Count of all PCI_ROOT_BRIDIGE_IO_PROTOCOL handles
  Segment         Segment number of device we are dealing with
  Bus             Bus number of device we are dealing with
  IoDev           Handle used to access configuration space of PCI device

Returns:

  EFI_SUCCESS           - The command completed successfully
  EFI_INVALID_PARAMETER - Invalid parameter

--*/
{
  UINTN                             Index;
  EFI_STATUS                        Status;
  BOOLEAN                           FoundInterface;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            MinBus;
  UINT16                            MaxBus;
  BOOLEAN                           IsEnd;

  FoundInterface = FALSE;
  //
  // Go through all handles, until the one meets the criteria is found
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = PciGetProtocolAndResource (HandleBuf[Index], IoDev, &Descriptors);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // When Descriptors == NULL, the Configuration() is not implemented,
    // so we only check the Segment number
    //
    if (Descriptors == NULL && Segment == (*IoDev)->SegmentNumber) {
      return EFI_SUCCESS;
    }

    if ((*IoDev)->SegmentNumber != Segment) {
      continue;
    }

    while (TRUE) {
      Status = PciGetNextBusRange (&Descriptors, &MinBus, &MaxBus, &IsEnd);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      if (IsEnd) {
        break;
      }

      if (MinBus <= Bus && MaxBus >= Bus) {
        FoundInterface = TRUE;
        break;
      }
    }
    if (FoundInterface) {
      break;
    }
  }

  if (FoundInterface) {
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

EFI_STATUS
PciGetProtocolAndResource (
  IN  EFI_HANDLE                            Handle,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev,
  OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     **Descriptors
  )
/*++

Routine Description:

  This function gets the protocol interface from the given handle, and
  obtains its address space descriptors.

Arguments:

  Handle          The PCI_ROOT_BRIDIGE_IO_PROTOCOL handle
  IoDev           Handle used to access configuration space of PCI device
  Descriptors     Points to the address space descriptors

Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  EFI_STATUS  Status;

  //
  // Get inferface from protocol
  //
  Status = BS->HandleProtocol (
                Handle,
                &gEfiPciRootBridgeIoProtocolGuid,
                IoDev
                );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Call Configuration() to get address space descriptors
  //
  Status = (*IoDev)->Configuration (*IoDev, Descriptors);
  if (Status == EFI_UNSUPPORTED) {
    *Descriptors = NULL;
    return EFI_SUCCESS;

  } else {
    return Status;
  }
}

EFI_STATUS
PciGetNextBusRange (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  **Descriptors,
  OUT    UINT16                             *MinBus,
  OUT    UINT16                             *MaxBus,
  OUT    BOOLEAN                            *IsEnd
  )
/*++

Routine Description:

  This function get the next bus range of given address space descriptors.
  It also moves the pointer backward a node, to get prepared to be called
  again.

Arguments:

  Descriptors     points to current position of a serial of address space 
                  descriptors
  MinBus          The lower range of bus number
  ManBus          The upper range of bus number
  IsEnd           Meet end of the serial of descriptors 
  
Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  *IsEnd = FALSE;

  //
  // When *Descriptors is NULL, Configuration() is not implemented, so assume
  // range is 0~PCI_MAX_BUS
  //
  if ((*Descriptors) == NULL) {
    *MinBus = 0;
    *MaxBus = PCI_MAX_BUS;
    return EFI_SUCCESS;
  }
  //
  // *Descriptors points to one or more address space descriptors, which
  // ends with a end tagged descriptor. Examine each of the descriptors,
  // if a bus typed one is found and its bus range covers bus, this handle
  // is the handle we are looking for.
  //
  if ((*Descriptors)->Desc == ACPI_END_TAG_DESCRIPTOR) {
    *IsEnd = TRUE;
  }

  while ((*Descriptors)->Desc != ACPI_END_TAG_DESCRIPTOR) {
    if ((*Descriptors)->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS) {
      *MinBus = (UINT16) (*Descriptors)->AddrRangeMin;
      *MaxBus = (UINT16) (*Descriptors)->AddrRangeMax;
      (*Descriptors)++;
      break;
    }

    (*Descriptors)++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainData (
  IN PCI_CONFIG_SPACE                       *ConfigSpace,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  )
/*++

Routine Description:

  Explain the data in PCI configuration space. The part which is common for
  PCI device and bridge is interpreted in this function. It calls other 
  functions to interpret data unique for device or bridge.

Arguments:

  ConfigSpace     Data in PCI configuration space
  Address         Address used to access configuration space of this PCI device
  IoDev           Handle used to access configuration space of PCI device

Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  PCI_COMMON_HEADER *Common;
  PCI_HEADER_TYPE   HeaderType;
  EFI_STATUS        Status;
  UINT8             CapPtr;

  Common = &(ConfigSpace->Common);

  Print (L"\n");

  //
  // Print Vendor Id and Device Id
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_VENDOR_ID),
    HiiHandle,
    INDEX_OF (&(Common->VendorId)),
    Common->VendorId
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_DEVICE_ID),
    HiiHandle,
    INDEX_OF (&(Common->DeviceId)),
    Common->DeviceId
    );

  //
  // Print register Command
  //
  PciExplainCommand (&(Common->Command));

  //
  // Print register Status
  //
  PciExplainStatus (&(Common->Status), TRUE, PciUndefined);

  //
  // Print register Revision ID
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_REVISION_ID),
    HiiHandle,
    INDEX_OF (&(Common->RevisionId)),
    Common->RevisionId
    );

  //
  // Print register BIST
  //
  PrintToken (STRING_TOKEN (STR_PCI2_BIST), HiiHandle, INDEX_OF (&(Common->BIST)));
  if ((Common->BIST & PCI_BIT_7) != 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_CAPABLE_RETURN), HiiHandle, 0x0f & Common->BIST);

  } else {
    PrintToken (STRING_TOKEN (STR_PCI2_INCAPABLE), HiiHandle);
  }
  //
  // Print register Cache Line Size
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_CACHE_LINE_SIZE),
    HiiHandle,
    INDEX_OF (&(Common->CacheLineSize)),
    Common->CacheLineSize
    );

  //
  // Print register Latency Timer
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_LATENCY_TIMER),
    HiiHandle,
    INDEX_OF (&(Common->PrimaryLatencyTimer)),
    Common->PrimaryLatencyTimer
    );

  //
  // Print register Header Type
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_HEADER_TYPE),
    HiiHandle,
    INDEX_OF (&(Common->HeaderType)),
    Common->HeaderType
    );

  if ((Common->HeaderType & PCI_BIT_7) != 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_MULTI_FUNCTION), HiiHandle);

  } else {
    PrintToken (STRING_TOKEN (STR_PCI2_SINGLE_FUNCTION), HiiHandle);
  }

  HeaderType = (UINT8) (Common->HeaderType & 0x7f);
  switch (HeaderType) {
  case PciDevice:
    PrintToken (STRING_TOKEN (STR_PCI2_PCI_DEVICE), HiiHandle);
    break;

  case PciP2pBridge:
    PrintToken (STRING_TOKEN (STR_PCI2_P2P_BRIDGE), HiiHandle);
    break;

  case PciCardBusBridge:
    PrintToken (STRING_TOKEN (STR_PCI2_CARDBUS_BRIDGE), HiiHandle);
    break;

  default:
    PrintToken (STRING_TOKEN (STR_PCI2_RESERVED), HiiHandle);
    HeaderType = PciUndefined;
  }

  //
  // Print register Class Code
  //
  PrintToken (STRING_TOKEN (STR_PCI2_CLASS), HiiHandle);
  PciPrintClassCode ((UINT8 *) Common->ClassCode, TRUE);
  Print (L"\n");

  if (GetExecutionBreak ()) {
    return EFI_SUCCESS;
  }

  //
  // Interpret remaining part of PCI configuration header depending on
  // HeaderType
  //
  CapPtr  = 0;
  Status  = EFI_SUCCESS;
  switch (HeaderType) {
  case PciDevice:
    Status = PciExplainDeviceData (
              &(ConfigSpace->NonCommon.Device),
              Address,
              IoDev
              );
    CapPtr = ConfigSpace->NonCommon.Device.CapabilitiesPtr;
    break;

  case PciP2pBridge:
    Status = PciExplainBridgeData (
              &(ConfigSpace->NonCommon.Bridge),
              Address,
              IoDev
              );
    CapPtr = ConfigSpace->NonCommon.Bridge.CapabilitiesPtr;
    break;

  case PciCardBusBridge:
    Status = PciExplainCardBusData (
              &(ConfigSpace->NonCommon.CardBus),
              Address,
              IoDev
              );
    CapPtr = ConfigSpace->NonCommon.CardBus.CapabilitiesPtr;
    break;
  }
  //
  // If Status bit4 is 1, dump or explain capability structure
  //
  if ((Common->Status) & EFI_PCI_STATUS_CAPABILITY) {
    PciExplainCapabilityStruct (IoDev, Address, CapPtr);
  }

  return Status;
}

EFI_STATUS
PciExplainDeviceData (
  IN PCI_DEVICE_HEADER                      *Device,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  )
/*++

Routine Description:

  Explain the device specific part of data in PCI configuration space.

Arguments:

  Device          Data in PCI configuration space
  Address         Address used to access configuration space of this PCI device
  IoDev           Handle used to access configuration space of PCI device

Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  UINTN       Index;
  BOOLEAN     BarExist;
  EFI_STATUS  Status;
  UINTN       BarCount;

  //
  // Print Base Address Registers(Bar). When Bar = 0, this Bar does not
  // exist. If these no Bar for this function, print "none", otherwise
  // list detail information about this Bar.
  //
  PrintToken (STRING_TOKEN (STR_PCI2_BASE_ADDR), HiiHandle, INDEX_OF (Device->Bar));

  BarExist  = FALSE;
  BarCount  = sizeof (Device->Bar) / sizeof (Device->Bar[0]);
  for (Index = 0; Index < BarCount; Index++) {
    if (Device->Bar[Index] == 0) {
      continue;
    }

    if (!BarExist) {
      BarExist = TRUE;
      PrintToken (STRING_TOKEN (STR_PCI2_START_TYPE), HiiHandle);
      Print (L"  --------------------------------------------------------------------------");
    }

    Status = PciExplainBar (
              &(Device->Bar[Index]),
              &(mConfigSpace->Common.Command),
              Address,
              IoDev,
              &Index
              );

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  if (!BarExist) {
    PrintToken (STRING_TOKEN (STR_PCI2_NONE), HiiHandle);

  } else {
    Print (L"\n  --------------------------------------------------------------------------");
  }

  //
  // Print register Expansion ROM Base Address
  //
  if ((Device->ROMBar & PCI_BIT_0) == 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_EXPANSION_ROM_DISABLED), HiiHandle, INDEX_OF (&(Device->ROMBar)));

  } else {
    PrintToken (
      STRING_TOKEN (STR_PCI2_EXPANSION_ROM_BASE),
      HiiHandle,
      INDEX_OF (&(Device->ROMBar)),
      Device->ROMBar
      );
  }
  //
  // Print register Cardbus CIS ptr
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_CARDBUS_CIS),
    HiiHandle,
    INDEX_OF (&(Device->CardBusCISPtr)),
    Device->CardBusCISPtr
    );

  //
  // Print register Sub-vendor ID and subsystem ID
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_SUB_VENDOR_ID),
    HiiHandle,
    INDEX_OF (&(Device->SubVendorId)),
    Device->SubVendorId
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_SUBSYSTEM_ID),
    HiiHandle,
    INDEX_OF (&(Device->SubSystemId)),
    Device->SubSystemId
    );

  //
  // Print register Capabilities Ptr
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_CAPABILITIES_PTR),
    HiiHandle,
    INDEX_OF (&(Device->CapabilitiesPtr)),
    Device->CapabilitiesPtr
    );

  //
  // Print register Interrupt Line and interrupt pin
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_INTERRUPT_LINE),
    HiiHandle,
    INDEX_OF (&(Device->InterruptLine)),
    Device->InterruptLine
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_INTERRUPT_PIN),
    HiiHandle,
    INDEX_OF (&(Device->InterruptPin)),
    Device->InterruptPin
    );

  //
  // Print register Min_Gnt and Max_Lat
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_MIN_GNT),
    HiiHandle,
    INDEX_OF (&(Device->MinGnt)),
    Device->MinGnt
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_MAX_LAT),
    HiiHandle,
    INDEX_OF (&(Device->MaxLat)),
    Device->MaxLat
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainBridgeData (
  IN  PCI_BRIDGE_HEADER                     *Bridge,
  IN  UINT64                                Address,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *IoDev
  )
/*++

Routine Description:

  Explain the bridge specific part of data in PCI configuration space.

Arguments:

  Bridge          Bridge specific data region in PCI configuration space
  Address         Address used to access configuration space of this PCI device
  IoDev           Handle used to access configuration space of PCI device

Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  UINTN       Index;
  BOOLEAN     BarExist;
  UINTN       BarCount;
  UINT32      IoAddress32;
  EFI_STATUS  Status;

  //
  // Print Base Address Registers. When Bar = 0, this Bar does not
  // exist. If these no Bar for this function, print "none", otherwise
  // list detail information about this Bar.
  //
  PrintToken (STRING_TOKEN (STR_PCI2_BASE_ADDRESS), HiiHandle, INDEX_OF (&(Bridge->Bar)));

  BarExist  = FALSE;
  BarCount  = sizeof (Bridge->Bar) / sizeof (Bridge->Bar[0]);

  for (Index = 0; Index < BarCount; Index++) {
    if (Bridge->Bar[Index] == 0) {
      continue;
    }

    if (!BarExist) {
      BarExist = TRUE;
      PrintToken (STRING_TOKEN (STR_PCI2_START_TYPE_2), HiiHandle);
      Print (L"  --------------------------------------------------------------------------");
    }

    Status = PciExplainBar (
              &(Bridge->Bar[Index]),
              &(mConfigSpace->Common.Command),
              Address,
              IoDev,
              &Index
              );

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  if (!BarExist) {
    PrintToken (STRING_TOKEN (STR_PCI2_NONE), HiiHandle);
  } else {
    Print (L"\n  --------------------------------------------------------------------------");
  }

  //
  // Expansion register ROM Base Address
  //
  if ((Bridge->ROMBar & PCI_BIT_0) == 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_NO_EXPANSION_ROM), HiiHandle, INDEX_OF (&(Bridge->ROMBar)));

  } else {
    PrintToken (
      STRING_TOKEN (STR_PCI2_EXPANSION_ROM_BASE_2),
      HiiHandle,
      INDEX_OF (&(Bridge->ROMBar)),
      Bridge->ROMBar
      );
  }
  //
  // Print Bus Numbers(Primary, Secondary, and Subordinate
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_BUS_NUMBERS),
    HiiHandle,
    INDEX_OF (&(Bridge->PrimaryBus)),
    INDEX_OF (&(Bridge->SecondaryBus)),
    INDEX_OF (&(Bridge->SubordinateBus))
    );

  Print (L"               ------------------------------------------------------\n");

  PrintToken (STRING_TOKEN (STR_PCI2_BRIDGE), HiiHandle, Bridge->PrimaryBus);
  PrintToken (STRING_TOKEN (STR_PCI2_BRIDGE), HiiHandle, Bridge->SecondaryBus);
  PrintToken (STRING_TOKEN (STR_PCI2_BRIDGE), HiiHandle, Bridge->SubordinateBus);

  //
  // Print register Secondary Latency Timer
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_SECONDARY_TIMER),
    HiiHandle,
    INDEX_OF (&(Bridge->SecondaryLatencyTimer)),
    Bridge->SecondaryLatencyTimer
    );

  //
  // Print register Secondary Status
  //
  PciExplainStatus (&(Bridge->SecondaryStatus), FALSE, PciP2pBridge);

  //
  // Print I/O and memory ranges this bridge forwards. There are 3 resource
  // types: I/O, memory, and pre-fetchable memory. For each resource type,
  // base and limit address are listed.
  //
  PrintToken (STRING_TOKEN (STR_PCI2_RESOURCE_TYPE), HiiHandle);
  Print (L"----------------------------------------------------------------------\n");

  //
  // IO Base & Limit
  //
  IoAddress32 = (Bridge->IoBaseUpper << 16 | Bridge->IoBase << 8);
  IoAddress32 &= 0xfffff000;
  PrintToken (
    STRING_TOKEN (STR_PCI2_TWO_VARS),
    HiiHandle,
    INDEX_OF (&(Bridge->IoBase)),
    IoAddress32
    );

  IoAddress32 = (Bridge->IoLimitUpper << 16 | Bridge->IoLimit << 8);
  IoAddress32 |= 0x00000fff;
  PrintToken (STRING_TOKEN (STR_PCI2_ONE_VAR), HiiHandle, IoAddress32);

  //
  // Memory Base & Limit
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_MEMORY),
    HiiHandle,
    INDEX_OF (&(Bridge->MemoryBase)),
    (Bridge->MemoryBase << 16) & 0xfff00000
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_ONE_VAR),
    HiiHandle,
    (Bridge->MemoryLimit << 16) | 0x000fffff
    );

  //
  // Pre-fetch-able Memory Base & Limit
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_PREFETCHABLE),
    HiiHandle,
    INDEX_OF (&(Bridge->PrefetchableMemBase)),
    Bridge->PrefetchableBaseUpper,
    (Bridge->PrefetchableMemBase << 16) & 0xfff00000
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_TWO_VARS_2),
    HiiHandle,
    Bridge->PrefetchableLimitUpper,
    (Bridge->PrefetchableMemLimit << 16) | 0x000fffff
    );

  //
  // Print register Capabilities Pointer
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_CAPABILITIES_PTR_2),
    HiiHandle,
    INDEX_OF (&(Bridge->CapabilitiesPtr)),
    Bridge->CapabilitiesPtr
    );

  //
  // Print register Bridge Control
  //
  PciExplainBridgeControl (&(Bridge->BridgeControl), PciP2pBridge);

  //
  // Print register Interrupt Line & PIN
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_INTERRUPT_LINE_2),
    HiiHandle,
    INDEX_OF (&(Bridge->InterruptLine)),
    Bridge->InterruptLine
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_INTERRUPT_PIN),
    HiiHandle,
    INDEX_OF (&(Bridge->InterruptPin)),
    Bridge->InterruptPin
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainBar (
  IN UINT32                                 *Bar,
  IN UINT16                                 *Command,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev,
  IN OUT UINTN                              *Index
  )
/*++

Routine Description:

  Explain the Base Address Register(Bar) in PCI configuration space.

Arguments:

  Bar             Points to the Base Address Register intended to interpret
  Command         Points to the register Command
  Address         Address used to access configuration space of this PCI device
  IoDev           Handle used to access configuration space of PCI device
  Index           The Index

Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  UINT16  OldCommand;
  UINT16  NewCommand;
  UINT64  Bar64;
  UINT32  OldBar32;
  UINT32  NewBar32;
  UINT64  OldBar64;
  UINT64  NewBar64;
  BOOLEAN IsMem;
  BOOLEAN IsBar32;
  UINT64  RegAddress;

  IsBar32   = TRUE;
  Bar64     = 0;
  NewBar32  = 0;
  NewBar64  = 0;

  //
  // According the bar type, list detail about this bar, for example: 32 or
  // 64 bits; pre-fetchable or not.
  //
  if ((*Bar & PCI_BIT_0) == 0) {
    //
    // This bar is of memory type
    //
    IsMem = TRUE;

    if ((*Bar & PCI_BIT_1) == 0 && (*Bar & PCI_BIT_2) == 0) {
      PrintToken (STRING_TOKEN (STR_PCI2_BAR), HiiHandle, *Bar & 0xfffffff0);
      PrintToken (STRING_TOKEN (STR_PCI2_MEM), HiiHandle);
      PrintToken (STRING_TOKEN (STR_PCI2_32_BITS), HiiHandle);

    } else if ((*Bar & PCI_BIT_1) == 0 && (*Bar & PCI_BIT_2) != 0) {
      Bar64 = 0x0;
      CopyMem (&Bar64, Bar, sizeof (UINT64));
      PrintToken (STRING_TOKEN (STR_PCI2_ONE_VAR_2), HiiHandle, RShiftU64 ((Bar64 & 0xfffffffffffffff0), 32));
      PrintToken (STRING_TOKEN (STR_PCI2_ONE_VAR_3), HiiHandle, (UINT32) (Bar64 & 0xfffffffffffffff0));
      PrintToken (STRING_TOKEN (STR_PCI2_MEM), HiiHandle);
      PrintToken (STRING_TOKEN (STR_PCI2_64_BITS), HiiHandle);
      IsBar32 = FALSE;
      *Index += 1;

    } else {
      //
      // Reserved
      //
      PrintToken (STRING_TOKEN (STR_PCI2_BAR), HiiHandle, *Bar & 0xfffffff0);
      PrintToken (STRING_TOKEN (STR_PCI2_MEM_2), HiiHandle);
    }

    if ((*Bar & PCI_BIT_3) == 0) {
      PrintToken (STRING_TOKEN (STR_PCI2_NO), HiiHandle);

    } else {
      PrintToken (STRING_TOKEN (STR_PCI2_YES), HiiHandle);
    }

  } else {
    //
    // This bar is of io type
    //
    IsMem = FALSE;
    PrintToken (STRING_TOKEN (STR_PCI2_ONE_VAR_4), HiiHandle, *Bar & 0xfffffffc);
    Print (L"I/O                               ");
  }

  //
  // Get BAR length(or the amount of resource this bar demands for). To get
  // Bar length, first we should temporarily disable I/O and memory access
  // of this function(by set bits in the register Command), then write all
  // "1"s to this bar. The bar value read back is the amount of resource
  // this bar demands for.
  //
  //
  // Disable io & mem access
  //
  OldCommand  = *Command;
  NewCommand  = (UINT16) (OldCommand & 0xfffc);
  RegAddress  = Address | INDEX_OF (Command);
  IoDev->Pci.Write (IoDev, EfiPciWidthUint16, RegAddress, 1, &NewCommand);

  RegAddress = Address | INDEX_OF (Bar);

  //
  // Read after write the BAR to get the size
  //
  if (IsBar32) {
    OldBar32  = *Bar;
    NewBar32  = 0xffffffff;

    IoDev->Pci.Write (IoDev, EfiPciWidthUint32, RegAddress, 1, &NewBar32);
    IoDev->Pci.Read (IoDev, EfiPciWidthUint32, RegAddress, 1, &NewBar32);
    IoDev->Pci.Write (IoDev, EfiPciWidthUint32, RegAddress, 1, &OldBar32);

    if (IsMem) {
      NewBar32  = NewBar32 & 0xfffffff0;
      NewBar32  = (~NewBar32) + 1;

    } else {
      NewBar32  = NewBar32 & 0xfffffffc;
      NewBar32  = (~NewBar32) + 1;
      NewBar32  = NewBar32 & 0x0000ffff;
    }
  } else {

    OldBar64 = 0x0;
    CopyMem (&OldBar64, Bar, sizeof (UINT64));
    NewBar64 = 0xffffffffffffffff;

    IoDev->Pci.Write (IoDev, EfiPciWidthUint32, RegAddress, 2, &NewBar64);
    IoDev->Pci.Read (IoDev, EfiPciWidthUint32, RegAddress, 2, &NewBar64);
    IoDev->Pci.Write (IoDev, EfiPciWidthUint32, RegAddress, 2, &OldBar64);

    if (IsMem) {
      NewBar64  = NewBar64 & 0xfffffffffffffff0;
      NewBar64  = (~NewBar64) + 1;

    } else {
      NewBar64  = NewBar64 & 0xfffffffffffffffc;
      NewBar64  = (~NewBar64) + 1;
      NewBar64  = NewBar64 & 0x000000000000ffff;
    }
  }
  //
  // Enable io & mem access
  //
  RegAddress = Address | INDEX_OF (Command);
  IoDev->Pci.Write (IoDev, EfiPciWidthUint16, RegAddress, 1, &OldCommand);

  if (IsMem) {
    if (IsBar32) {
      PrintToken (STRING_TOKEN (STR_PCI2_NEWBAR_32), HiiHandle, NewBar32);
      PrintToken (STRING_TOKEN (STR_PCI2_NEWBAR_32_2), HiiHandle, NewBar32 + (*Bar & 0xfffffff0) - 1);

    } else {
      PrintToken (STRING_TOKEN (STR_PCI2_RSHIFT), HiiHandle, RShiftU64 (NewBar64, 32));
      PrintToken (STRING_TOKEN (STR_PCI2_RSHIFT), HiiHandle, (UINT32) NewBar64);
      Print (L"  ");
      PrintToken (
        STRING_TOKEN (STR_PCI2_RSHIFT),
        HiiHandle,
        RShiftU64 ((NewBar64 + (Bar64 & 0xfffffffffffffff0) - 1), 32)
        );
      PrintToken (STRING_TOKEN (STR_PCI2_RSHIFT), HiiHandle, (UINT32) (NewBar64 + (Bar64 & 0xfffffffffffffff0) - 1));

    }
  } else {
    PrintToken (STRING_TOKEN (STR_PCI2_NEWBAR_32_3), HiiHandle, NewBar32);
    PrintToken (STRING_TOKEN (STR_PCI2_NEWBAR_32_4), HiiHandle, NewBar32 + (*Bar & 0xfffffffc) - 1);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainCardBusData (
  IN PCI_CARDBUS_HEADER                     *CardBus,
  IN UINT64                                 Address,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL        *IoDev
  )
/*++

Routine Description:

  Explain the cardbus specific part of data in PCI configuration space.

Arguments:

  CardBus         CardBus specific region of PCI configuration space
  Address         Address used to access configuration space of this PCI device
  IoDev           Handle used to access configuration space of PCI device

Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  BOOLEAN           Io32Bit;
  PCI_CARDBUS_DATA  *CardBusData;

  PrintToken (
    STRING_TOKEN (STR_PCI2_CARDBUS_SOCKET),
    HiiHandle,
    INDEX_OF (&(CardBus->CardBusSocketReg)),
    CardBus->CardBusSocketReg
    );

  //
  // Print Secondary Status
  //
  PciExplainStatus (&(CardBus->SecondaryStatus), FALSE, PciCardBusBridge);

  //
  // Print Bus Numbers(Primary bus number, CardBus bus number, and
  // Subordinate bus number
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_BUS_NUMBERS_2),
    HiiHandle,
    INDEX_OF (&(CardBus->PciBusNumber)),
    INDEX_OF (&(CardBus->CardBusBusNumber)),
    INDEX_OF (&(CardBus->SubordinateBusNumber))
    );

  Print (L"               ------------------------------------------------------\n");

  PrintToken (STRING_TOKEN (STR_PCI2_CARDBUS), HiiHandle, CardBus->PciBusNumber);
  PrintToken (STRING_TOKEN (STR_PCI2_CARDBUS_2), HiiHandle, CardBus->CardBusBusNumber);
  PrintToken (STRING_TOKEN (STR_PCI2_CARDBUS_3), HiiHandle, CardBus->SubordinateBusNumber);

  //
  // Print CardBus Latency Timer
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_CARDBUS_LATENCY),
    HiiHandle,
    INDEX_OF (&(CardBus->CardBusLatencyTimer)),
    CardBus->CardBusLatencyTimer
    );

  //
  // Print Memory/Io ranges this cardbus bridge forwards
  //
  PrintToken (STRING_TOKEN (STR_PCI2_RESOURCE_TYPE_2), HiiHandle);
  Print (L"----------------------------------------------------------------------\n");

  PrintToken (
    STRING_TOKEN (STR_PCI2_MEM_3),
    HiiHandle,
    INDEX_OF (&(CardBus->MemoryBase0)),
    CardBus->BridgeControl & PCI_BIT_8 ? L"    Prefetchable" : L"Non-Prefetchable",
    CardBus->MemoryBase0 & 0xfffff000,
    CardBus->MemoryLimit0 | 0x00000fff
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_MEM_3),
    HiiHandle,
    INDEX_OF (&(CardBus->MemoryBase1)),
    CardBus->BridgeControl & PCI_BIT_9 ? L"    Prefetchable" : L"Non-Prefetchable",
    CardBus->MemoryBase1 & 0xfffff000,
    CardBus->MemoryLimit1 | 0x00000fff
    );

  Io32Bit = (BOOLEAN) (CardBus->IoBase0 & PCI_BIT_0);
  PrintToken (
    STRING_TOKEN (STR_PCI2_IO_2),
    HiiHandle,
    INDEX_OF (&(CardBus->IoBase0)),
    Io32Bit ? L"          32 bit" : L"          16 bit",
    CardBus->IoBase0 & (Io32Bit ? 0xfffffffc : 0x0000fffc),
    CardBus->IoLimit0 & (Io32Bit ? 0xffffffff : 0x0000ffff) | 0x00000003
    );

  Io32Bit = (BOOLEAN) (CardBus->IoBase1 & PCI_BIT_0);
  PrintToken (
    STRING_TOKEN (STR_PCI2_IO_2),
    HiiHandle,
    INDEX_OF (&(CardBus->IoBase1)),
    Io32Bit ? L"          32 bit" : L"          16 bit",
    CardBus->IoBase1 & (Io32Bit ? 0xfffffffc : 0x0000fffc),
    CardBus->IoLimit1 & (Io32Bit ? 0xffffffff : 0x0000ffff) | 0x00000003
    );

  //
  // Print register Interrupt Line & PIN
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_INTERRUPT_LINE_3),
    HiiHandle,
    INDEX_OF (&(CardBus->InterruptLine)),
    CardBus->InterruptLine,
    INDEX_OF (&(CardBus->InterruptPin)),
    CardBus->InterruptPin
    );

  //
  // Print register Bridge Control
  //
  PciExplainBridgeControl (&(CardBus->BridgeControl), PciCardBusBridge);

  //
  // Print some registers in data region of PCI configuration space for cardbus
  // bridge. Fields include: Sub VendorId, Subsystem ID, and Legacy Mode Base
  // Address.
  //
  CardBusData = (PCI_CARDBUS_DATA *) ((UINT8 *) CardBus + sizeof (PCI_CARDBUS_HEADER));

  PrintToken (
    STRING_TOKEN (STR_PCI2_SUB_VENDOR_ID_2),
    HiiHandle,
    INDEX_OF (&(CardBusData->SubVendorId)),
    CardBusData->SubVendorId,
    INDEX_OF (&(CardBusData->SubSystemId)),
    CardBusData->SubSystemId
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_OPTIONAL),
    HiiHandle,
    INDEX_OF (&(CardBusData->LegacyBase)),
    CardBusData->LegacyBase
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainStatus (
  IN UINT16                                 *Status,
  IN BOOLEAN                                MainStatus,
  IN PCI_HEADER_TYPE                        HeaderType
  )
/*++

Routine Description:

  Explain each meaningful bit of register Status. The definition of Status is
  slightly different depending on the PCI header type.

Arguments:

  Status          Points to the content of register Status
  MainStatus      Indicates if this register is main status(not secondary 
                  status)
  HeaderType      Header type of this PCI device
  
Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  if (MainStatus) {
    PrintToken (STRING_TOKEN (STR_PCI2_STATUS), HiiHandle, INDEX_OF (Status), *Status);

  } else {
    PrintToken (STRING_TOKEN (STR_PCI2_SECONDARY_STATUS), HiiHandle, INDEX_OF (Status), *Status);
  }

  PrintToken (STRING_TOKEN (STR_PCI2_NEW_CAPABILITIES), HiiHandle, (*Status & PCI_BIT_4) != 0);

  //
  // Bit 5 is meaningless for CardBus Bridge
  //
  if (HeaderType == PciCardBusBridge) {
    PrintToken (STRING_TOKEN (STR_PCI2_66_CAPABLE), HiiHandle, (*Status & PCI_BIT_5) != 0);

  } else {
    PrintToken (STRING_TOKEN (STR_PCI2_66_CAPABLE_2), HiiHandle, (*Status & PCI_BIT_5) != 0);
  }

  PrintToken (STRING_TOKEN (STR_PCI2_FAST_BACK), HiiHandle, (*Status & PCI_BIT_7) != 0);

  PrintToken (STRING_TOKEN (STR_PCI2_MASTER_DATA), HiiHandle, (*Status & PCI_BIT_8) != 0);
  //
  // Bit 9 and bit 10 together decides the DEVSEL timing
  //
  PrintToken (STRING_TOKEN (STR_PCI2_DEVSEL_TIMING), HiiHandle);
  if ((*Status & PCI_BIT_9) == 0 && (*Status & PCI_BIT_10) == 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_FAST), HiiHandle);

  } else if ((*Status & PCI_BIT_9) != 0 && (*Status & PCI_BIT_10) == 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_MEDIUM), HiiHandle);

  } else if ((*Status & PCI_BIT_9) == 0 && (*Status & PCI_BIT_10) != 0) {
    PrintToken (STRING_TOKEN (STR_PCI2_SLOW), HiiHandle);

  } else {
    PrintToken (STRING_TOKEN (STR_PCI2_RESERVED_2), HiiHandle);
  }

  PrintToken (
    STRING_TOKEN (STR_PCI2_SIGNALED_TARGET),
    HiiHandle,
    (*Status & PCI_BIT_11) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_RECEIVED_TARGET),
    HiiHandle,
    (*Status & PCI_BIT_12) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_RECEIVED_MASTER),
    HiiHandle,
    (*Status & PCI_BIT_13) != 0
    );

  if (MainStatus) {
    PrintToken (
      STRING_TOKEN (STR_PCI2_SIGNALED_ERROR),
      HiiHandle,
      (*Status & PCI_BIT_14) != 0
      );

  } else {
    PrintToken (
      STRING_TOKEN (STR_PCI2_RECEIVED_ERROR),
      HiiHandle,
      (*Status & PCI_BIT_14) != 0
      );
  }

  PrintToken (
    STRING_TOKEN (STR_PCI2_DETECTED_ERROR),
    HiiHandle,
    (*Status & PCI_BIT_15) != 0
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainCommand (
  IN UINT16                                 *Command
  )
/*++

Routine Description:

  Explain each meaningful bit of register Command. 

Arguments:

  Command         Points to the content of register Command
  
Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  //
  // Print the binary value of register Command
  //
  PrintToken (STRING_TOKEN (STR_PCI2_COMMAND), HiiHandle, INDEX_OF (Command), *Command);

  //
  // Explain register Command bit by bit
  //
  PrintToken (
    STRING_TOKEN (STR_PCI2_SPACE_ACCESS_DENIED),
    HiiHandle,
    (*Command & PCI_BIT_0) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_MEMORY_SPACE),
    HiiHandle,
    (*Command & PCI_BIT_1) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_BEHAVE_BUS_MASTER),
    HiiHandle,
    (*Command & PCI_BIT_2) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_MONITOR_SPECIAL_CYCLE),
    HiiHandle,
    (*Command & PCI_BIT_3) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_MEM_WRITE_INVALIDATE),
    HiiHandle,
    (*Command & PCI_BIT_4) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_PALETTE_SNOOPING),
    HiiHandle,
    (*Command & PCI_BIT_5) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_ASSERT_PERR),
    HiiHandle,
    (*Command & PCI_BIT_6) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_DO_ADDR_STEPPING),
    HiiHandle,
    (*Command & PCI_BIT_7) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_SERR_DRIVER),
    HiiHandle,
    (*Command & PCI_BIT_8) != 0
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_FAST_BACK_2),
    HiiHandle,
    (*Command & PCI_BIT_9) != 0
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainBridgeControl (
  IN UINT16                                 *BridgeControl,
  IN PCI_HEADER_TYPE                        HeaderType
  )
/*++

Routine Description:

  Explain each meaningful bit of register Bridge Control. 

Arguments:

  BridgeControl   Points to the content of register Bridge Control
  HeaderType      The headertype
  
Returns:

  EFI_SUCCESS     The command completed successfully

--*/
{
  PrintToken (
    STRING_TOKEN (STR_PCI2_BRIDGE_CONTROL),
    HiiHandle,
    INDEX_OF (BridgeControl),
    *BridgeControl
    );

  PrintToken (
    STRING_TOKEN (STR_PCI2_PARITY_ERROR),
    HiiHandle,
    (*BridgeControl & PCI_BIT_0) != 0
    );
  PrintToken (
    STRING_TOKEN (STR_PCI2_SERR_ENABLE),
    HiiHandle,
    (*BridgeControl & PCI_BIT_1) != 0
    );
  PrintToken (
    STRING_TOKEN (STR_PCI2_ISA_ENABLE),
    HiiHandle,
    (*BridgeControl & PCI_BIT_2) != 0
    );
  PrintToken (
    STRING_TOKEN (STR_PCI2_VGA_ENABLE),
    HiiHandle,
    (*BridgeControl & PCI_BIT_3) != 0
    );
  PrintToken (
    STRING_TOKEN (STR_PCI2_MASTER_ABORT),
    HiiHandle,
    (*BridgeControl & PCI_BIT_5) != 0
    );

  //
  // Register Bridge Control has some slight differences between P2P bridge
  // and Cardbus bridge from bit 6 to bit 11.
  //
  if (HeaderType == PciP2pBridge) {
    PrintToken (
      STRING_TOKEN (STR_PCI2_SECONDARY_BUS_RESET),
      HiiHandle,
      (*BridgeControl & PCI_BIT_6) != 0
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_FAST_ENABLE),
      HiiHandle,
      (*BridgeControl & PCI_BIT_7) != 0
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_PRIMARY_DISCARD_TIMER),
      HiiHandle,
      (*BridgeControl & PCI_BIT_8) ? L"2^10" : L"2^15"
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_SECONDARY_DISCARD_TIMER),
      HiiHandle,
      (*BridgeControl & PCI_BIT_9) ? L"2^10" : L"2^15"
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_DISCARD_TIMER_STATUS),
      HiiHandle,
      (*BridgeControl & PCI_BIT_10) != 0
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_DISCARD_TIMER_SERR),
      HiiHandle,
      (*BridgeControl & PCI_BIT_11) != 0
      );

  } else {
    PrintToken (
      STRING_TOKEN (STR_PCI2_CARDBUS_RESET),
      HiiHandle,
      (*BridgeControl & PCI_BIT_6) != 0
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_IREQ_ENABLE),
      HiiHandle,
      (*BridgeControl & PCI_BIT_7) != 0
      );
    PrintToken (
      STRING_TOKEN (STR_PCI2_WRITE_POSTING_ENABLE),
      HiiHandle,
      (*BridgeControl & PCI_BIT_10) != 0
      );
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainCapabilityStruct (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *IoDev,
  IN UINT64                                   Address,
  IN  UINT8                                   CapPtr
  )
{
  UINT8   CapabilityPtr;
  UINT16  CapabilityEntry;
  UINT8   CapabilityID;
  UINT64  RegAddress;

  CapabilityPtr = CapPtr;

  //
  // Go through the Capability list
  //
  while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) { 
    RegAddress = Address + CapabilityPtr;
    IoDev->Pci.Read (IoDev, EfiPciWidthUint16, RegAddress, 1, &CapabilityEntry);

    CapabilityID = (UINT8) CapabilityEntry;

    //
    // Explain PciExpress data
    //
    if (EFI_PCI_CAPABILITY_ID_PCIEXP == CapabilityID) {
      PciExplainPciExpress (IoDev, Address, CapabilityPtr);
      return EFI_SUCCESS;
    }
    //
    // Explain other capabilities here
    //
    CapabilityPtr = (UINT8) (CapabilityEntry >> 8);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieCapReg (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieCapReg;
  CHAR16 *DevicePortType;

  PcieCapReg = PciExpressCap->PcieCapReg;
  Print (
    L"  Capability Version(3:0):          %E0x%04x%N\n",
    PCIE_CAP_VERSION (PcieCapReg)
    );
  if ((UINT8) PCIE_CAP_DEVICEPORT_TYPE (PcieCapReg) < PCIE_DEVICE_PORT_TYPE_MAX) {
    DevicePortType = DevicePortTypeTable[PCIE_CAP_DEVICEPORT_TYPE (PcieCapReg)];
  } else {
    DevicePortType = L"Unknown Type";
  }
  Print (
    L"  Device/PortType(7:4):             %E%s%N\n",
    DevicePortType
    );
  //
  // 'Slot Implemented' is only valid for:
  // a) Root Port of PCI Express Root Complex, or
  // b) Downstream Port of PCI Express Switch
  //
  if (PCIE_CAP_DEVICEPORT_TYPE (PcieCapReg) == PCIE_ROOT_COMPLEX_ROOT_PORT ||
      PCIE_CAP_DEVICEPORT_TYPE (PcieCapReg) == PCIE_SWITCH_DOWNSTREAM_PORT) {
    Print (
      L"  Slot Implemented(8):              %E%d%N\n",
      PCIE_CAP_SLOT_IMPLEMENTED (PcieCapReg)
      );
  }
  Print (
    L"  Interrupt Message Number(13:9):   %E0x%05x%N\n",
    PCIE_CAP_INT_MSG_NUM (PcieCapReg)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieDeviceCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieCapReg;
  UINT32 PcieDeviceCap;
  UINT8  DevicePortType;
  UINT8  L0sLatency;
  UINT8  L1Latency;

  PcieCapReg     = PciExpressCap->PcieCapReg;
  PcieDeviceCap  = PciExpressCap->PcieDeviceCap;
  DevicePortType = (UINT8) PCIE_CAP_DEVICEPORT_TYPE (PcieCapReg);
  Print (L"  Max_Payload_Size Supported(2:0):          ");
  if (PCIE_CAP_MAX_PAYLOAD (PcieDeviceCap) < 6) {
    Print (L"%E%d bytes%N\n", 1 << (PCIE_CAP_MAX_PAYLOAD (PcieDeviceCap) + 7));
  } else {
    Print (L"%EUnknown%N\n");
  }
  Print (
    L"  Phantom Functions Supported(4:3):         %E%d%N\n",
    PCIE_CAP_PHANTOM_FUNC (PcieDeviceCap)
    );
  Print (
    L"  Extended Tag Field Supported(5):          %E%d-bit Tag field supported%N\n",
    PCIE_CAP_EXTENDED_TAG (PcieDeviceCap) ? 8 : 5
    );
  //
  // Endpoint L0s and L1 Acceptable Latency is only valid for Endpoint
  //
  if (IS_PCIE_ENDPOINT (DevicePortType)) {
    L0sLatency = (UINT8) PCIE_CAP_L0sLatency (PcieDeviceCap);
    L1Latency  = (UINT8) PCIE_CAP_L1Latency (PcieDeviceCap);
    Print (L"  Endpoint L0s Acceptable Latency(8:6):     ");
    if (L0sLatency < 4) {
      Print (L"%EMaximum of %d ns%N\n", 1 << (L0sLatency + 6));
    } else {
      if (L0sLatency < 7) {
        Print (L"%EMaximum of %d us%N\n", 1 << (L0sLatency - 3));
      } else {
        Print (L"%ENo limit%N\n");
      }
    }
    Print (L"  Endpoint L1 Acceptable Latency(11:9):     ");
    if (L1Latency < 7) {
      Print (L"%EMaximum of %d us%N\n", 1 << (L1Latency + 1));
    } else {
      Print (L"%ENo limit%N\n");
    }
  }
  Print (
    L"  Role-based Error Reporting(15):           %E%d%N\n",
    PCIE_CAP_ERR_REPORTING (PcieDeviceCap)
    );
  //
  // Only valid for Upstream Port:
  // a) Captured Slot Power Limit Value
  // b) Captured Slot Power Scale
  //
  if (DevicePortType == PCIE_SWITCH_UPSTREAM_PORT) {
    Print (
      L"  Captured Slot Power Limit Value(25:18):   %E0x%02x%N\n",
      PCIE_CAP_SLOT_POWER_VALUE (PcieDeviceCap)
      );
    Print (
      L"  Captured Slot Power Limit Scale(27:26):   %E%s%N\n",
      SlotPwrLmtScaleTable[PCIE_CAP_SLOT_POWER_SCALE (PcieDeviceCap)]
      );
  }
  //
  // Function Level Reset Capability is only valid for Endpoint
  //
  if (IS_PCIE_ENDPOINT (DevicePortType)) {
    Print (
      L"  Function Level Reset Capability(28):      %E%d%N\n",
      PCIE_CAP_FUNC_LEVEL_RESET (PcieDeviceCap)
      );
  }
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieDeviceControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieCapReg;
  UINT16 PcieDeviceControl;

  PcieCapReg        = PciExpressCap->PcieCapReg;
  PcieDeviceControl = PciExpressCap->DeviceControl;
  Print (
    L"  Correctable Error Reporting Enable(0):    %E%d%N\n",
    PCIE_CAP_COR_ERR_REPORTING_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Non-Fatal Error Reporting Enable(1):      %E%d%N\n",
    PCIE_CAP_NONFAT_ERR_REPORTING_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Fatal Error Reporting Enable(2):          %E%d%N\n",
    PCIE_CAP_FATAL_ERR_REPORTING_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Unsupported Request Reporting Enable(3):  %E%d%N\n",
    PCIE_CAP_UNSUP_REQ_REPORTING_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Enable Relaxed Ordering(4):               %E%d%N\n",
    PCIE_CAP_RELAXED_ORDERING_ENABLE (PcieDeviceControl)
    );
  Print (L"  Max_Payload_Size(7:5):                    ");
  if (PCIE_CAP_MAX_PAYLOAD_SIZE (PcieDeviceControl) < 6) {
    Print (L"%E%d bytes%N\n", 1 << (PCIE_CAP_MAX_PAYLOAD_SIZE (PcieDeviceControl) + 7));
  } else {
    Print (L"%EUnknown%N\n");
  }
  Print (
    L"  Extended Tag Field Enable(8):             %E%d%N\n",
    PCIE_CAP_EXTENDED_TAG_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Phantom Functions Enable(9):              %E%d%N\n",
    PCIE_CAP_PHANTOM_FUNC_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Auxiliary (AUX) Power PM Enable(10):      %E%d%N\n",
    PCIE_CAP_AUX_PM_ENABLE (PcieDeviceControl)
    );
  Print (
    L"  Enable No Snoop(11):                      %E%d%N\n",
    PCIE_CAP_NO_SNOOP_ENABLE (PcieDeviceControl)
    );
  Print (L"  Max_Read_Request_Size(14:12):             ");
  if (PCIE_CAP_MAX_READ_REQ_SIZE (PcieDeviceControl) < 6) {
    Print (L"%E%d bytes%N\n", 1 << (PCIE_CAP_MAX_READ_REQ_SIZE (PcieDeviceControl) + 7));
  } else {
    Print (L"%EUnknown%N\n");
  }
  //
  // Read operation is only valid for PCI Express to PCI/PCI-X Bridges
  //
  if (PCIE_CAP_DEVICEPORT_TYPE (PcieCapReg) == PCIE_PCIE_TO_PCIX_BRIDGE) {
    Print (
      L"  Bridge Configuration Retry Enable(15):  %E%d%N\n",
      PCIE_CAP_BRG_CONF_RETRY (PcieDeviceControl)
      );
  }
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieDeviceStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieDeviceStatus;

  PcieDeviceStatus = PciExpressCap->DeviceStatus;
  Print (
    L"  Correctable Error Detected(0):            %E%d%N\n",
    PCIE_CAP_COR_ERR_DETECTED (PcieDeviceStatus)
    );
  Print (
    L"  Non-Fatal Error Detected(1):              %E%d%N\n",
    PCIE_CAP_NONFAT_ERR_DETECTED (PcieDeviceStatus)
    );
  Print (
    L"  Fatal Error Detected(2):                  %E%d%N\n",
    PCIE_CAP_FATAL_ERR_DETECTED (PcieDeviceStatus)
    );
  Print (
    L"  Unsupported Request Detected(3):          %E%d%N\n",
    PCIE_CAP_UNSUP_REQ_DETECTED (PcieDeviceStatus)
    );
  Print (
    L"  AUX Power Detected(4):                    %E%d%N\n",
    PCIE_CAP_AUX_POWER_DETECTED (PcieDeviceStatus)
    );
  Print (
    L"  Transactions Pending(5):                  %E%d%N\n",
    PCIE_CAP_TRANSACTION_PENDING (PcieDeviceStatus)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieLinkCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT32 PcieLinkCap;
  CHAR16 *SupLinkSpeeds;
  CHAR16 *ASPM;

  PcieLinkCap = PciExpressCap->LinkCap;
  switch (PCIE_CAP_SUP_LINK_SPEEDS (PcieLinkCap)) {
    case 1:
      SupLinkSpeeds = L"2.5 GT/s";
      break;
    case 2:
      SupLinkSpeeds = L"5.0 GT/s and 2.5 GT/s";
      break;
    default:
      SupLinkSpeeds = L"Unknown";
      break;
  }
  Print (
    L"  Supported Link Speeds(3:0):                         %E%s supported%N\n",
    SupLinkSpeeds
    );
  Print (
    L"  Maximum Link Width(9:4):                            %Ex%d%N\n",
    PCIE_CAP_MAX_LINK_WIDTH (PcieLinkCap)
    );
  switch (PCIE_CAP_ASPM_SUPPORT (PcieLinkCap)) {
    case 1:
      ASPM = L"L0s Entry";
      break;
    case 3:
      ASPM = L"L0s and L1";
      break;
    default:
      ASPM = L"Reserved";
      break;
  }
  Print (
    L"  Active State Power Management Support(11:10):       %E%s Supported%N\n",
    ASPM
    );
  Print (
    L"  L0s Exit Latency(14:12):                            %E%s%N\n",
    L0sLatencyStrTable[PCIE_CAP_L0s_LATENCY (PcieLinkCap)]
    );
  Print (
    L"  L1 Exit Latency(17:15):                             %E%s%N\n",
    L1LatencyStrTable[PCIE_CAP_L0s_LATENCY (PcieLinkCap)]
    );
  Print (
    L"  Clock Power Management(18):                         %E%d%N\n",
    PCIE_CAP_CLOCK_PM (PcieLinkCap)
    );
  Print (
    L"  Surprise Down Error Reporting Capable(19):          %E%d%N\n",
    PCIE_CAP_SUP_DOWN_ERR_REPORTING (PcieLinkCap)
    );
  Print (
    L"  Data Link Layer Link Active Reporting Capable(20):  %E%d%N\n",
    PCIE_CAP_LINK_ACTIVE_REPORTING (PcieLinkCap)
    );
  Print (
    L"  Link Bandwidth Notification Capability(21):         %E%d%N\n",
    PCIE_CAP_LINK_BWD_NOTIF_CAP (PcieLinkCap)
    );
  Print (
    L"  Port Number(31:24):                                 %E0x%02x%N\n",
    PCIE_CAP_PORT_NUMBER (PcieLinkCap)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieLinkControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieLinkControl;
  UINT8  DevicePortType;

  PcieLinkControl = PciExpressCap->LinkControl;
  DevicePortType  = (UINT8) PCIE_CAP_DEVICEPORT_TYPE (PciExpressCap->PcieCapReg);
  Print (
    L"  Active State Power Management Control(1:0):         %E%s%N\n",
    ASPMCtrlStrTable[PCIE_CAP_ASPM_CONTROL (PcieLinkControl)]
    );
  //
  // RCB is not applicable to switches
  //
  if (!IS_PCIE_SWITCH(DevicePortType)) {
    Print (
      L"  Read Completion Boundary (RCB)(3):                  %E%d byte%N\n",
      1 << (PCIE_CAP_RCB (PcieLinkControl) + 6)
      );
  }
  //
  // Link Disable is reserved on
  // a) Endpoints
  // b) PCI Express to PCI/PCI-X bridges
  // c) Upstream Ports of Switches
  //
  if (!IS_PCIE_ENDPOINT (DevicePortType) &&
      DevicePortType != PCIE_SWITCH_UPSTREAM_PORT &&
      DevicePortType != PCIE_PCIE_TO_PCIX_BRIDGE) {
    Print (
      L"  Link Disable(4):                                    %E%d%N\n",
      PCIE_CAP_LINK_DISABLE (PcieLinkControl)
      );
  }
  Print (
    L"  Common Clock Configuration(6):                      %E%d%N\n",
    PCIE_CAP_COMMON_CLK_CONF (PcieLinkControl)
    );
  Print (
    L"  Extended Synch(7):                                  %E%d%N\n",
    PCIE_CAP_EXT_SYNC (PcieLinkControl)
    );
  Print (
    L"  Enable Clock Power Management(8):                   %E%d%N\n",
    PCIE_CAP_CLK_PWR_MNG (PcieLinkControl)
    );
  Print (
    L"  Hardware Autonomous Width Disable(9):               %E%d%N\n",
    PCIE_CAP_HW_AUTO_WIDTH_DISABLE (PcieLinkControl)
    );
  Print (
    L"  Link Bandwidth Management Interrupt Enable(10):     %E%d%N\n",
    PCIE_CAP_LINK_BDW_MNG_INT_EN (PcieLinkControl)
    );
  Print (
    L"  Link Autonomous Bandwidth Interrupt Enable(11):     %E%d%N\n",
    PCIE_CAP_LINK_AUTO_BDW_INT_EN (PcieLinkControl)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieLinkStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieLinkStatus;
  CHAR16 *SupLinkSpeeds;

  PcieLinkStatus = PciExpressCap->LinkStatus;
  switch (PCIE_CAP_CUR_LINK_SPEED (PcieLinkStatus)) {
    case 1:
      SupLinkSpeeds = L"2.5 GT/s";
      break;
    case 2:
      SupLinkSpeeds = L"5.0 GT/s";
      break;
    default:
      SupLinkSpeeds = L"Reserved";
      break;
  }
  Print (
    L"  Current Link Speed(3:0):                            %E%s%N\n",
    SupLinkSpeeds
    );
  Print (
    L"  Negotiated Link Width(9:4):                         %Ex%d%N\n",
    PCIE_CAP_NEGO_LINK_WIDTH (PcieLinkStatus)
    );
  Print (
    L"  Link Training(11):                                  %E%d%N\n",
    PCIE_CAP_LINK_TRAINING (PcieLinkStatus)
    );
  Print (
    L"  Slot Clock Configuration(12):                       %E%d%N\n",
    PCIE_CAP_SLOT_CLK_CONF (PcieLinkStatus)
    );
  Print (
    L"  Data Link Layer Link Active(13):                    %E%d%N\n",
    PCIE_CAP_DATA_LINK_ACTIVE (PcieLinkStatus)
    );
  Print (
    L"  Link Bandwidth Management Status(14):               %E%d%N\n",
    PCIE_CAP_LINK_BDW_MNG_STAT (PcieLinkStatus)
    );
  Print (
    L"  Link Autonomous Bandwidth Status(15):               %E%d%N\n",
    PCIE_CAP_LINK_AUTO_BDW_STAT (PcieLinkStatus)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieSlotCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT32 PcieSlotCap;

  PcieSlotCap = PciExpressCap->SlotCap;

  Print (
    L"  Attention Button Present(0):                        %E%d%N\n",
    PCIE_CAP_ATT_BUT_PRESENT (PcieSlotCap)
    );
  Print (
    L"  Power Controller Present(1):                        %E%d%N\n",
    PCIE_CAP_PWR_CTRLLER_PRESENT (PcieSlotCap)
    );
  Print (
    L"  MRL Sensor Present(2):                              %E%d%N\n",
    PCIE_CAP_MRL_SENSOR_PRESENT (PcieSlotCap)
    );
  Print (
    L"  Attention Indicator Present(3):                     %E%d%N\n",
    PCIE_CAP_ATT_IND_PRESENT (PcieSlotCap)
    );
  Print (
    L"  Power Indicator Present(4):                         %E%d%N\n",
    PCIE_CAP_PWD_IND_PRESENT (PcieSlotCap)
    );
  Print (
    L"  Hot-Plug Surprise(5):                               %E%d%N\n",
    PCIE_CAP_HOTPLUG_SUPPRISE (PcieSlotCap)
    );
  Print (
    L"  Hot-Plug Capable(6):                                %E%d%N\n",
    PCIE_CAP_HOTPLUG_CAPABLE (PcieSlotCap)
    );
  Print (
    L"  Slot Power Limit Value(14:7):                       %E0x%02x%N\n",
    PCIE_CAP_SLOT_PWR_LIMIT_VALUE (PcieSlotCap)
    );
  Print (
    L"  Slot Power Limit Scale(16:15):                      %E%s%N\n",
    SlotPwrLmtScaleTable[PCIE_CAP_SLOT_PWR_LIMIT_SCALE (PcieSlotCap)]
    );
  Print (
    L"  Electromechanical Interlock Present(17):            %E%d%N\n",
    PCIE_CAP_ELEC_INTERLOCK_PRESENT (PcieSlotCap)
    );
  Print (
    L"  No Command Completed Support(18):                   %E%d%N\n",
    PCIE_CAP_NO_COMM_COMPLETED_SUP (PcieSlotCap)
    );
  Print (
    L"  Physical Slot Number(31:19):                        %E%d%N\n",
    PCIE_CAP_PHY_SLOT_NUM (PcieSlotCap)
    );
    
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieSlotControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieSlotControl;

  PcieSlotControl = PciExpressCap->SlotControl;
  Print (
    L"  Attention Button Pressed Enable(0):                 %E%d%N\n",
    PCIE_CAP_ATT_BUT_ENABLE (PcieSlotControl)
    );
  Print (
    L"  Power Fault Detected Enable(1):                     %E%d%N\n",
    PCIE_CAP_PWR_FLT_DETECT_ENABLE (PcieSlotControl)
    );
  Print (
    L"  MRL Sensor Changed Enable(2):                       %E%d%N\n",
    PCIE_CAP_MRL_SENSOR_CHANGE_ENABLE (PcieSlotControl)
    );
  Print (
    L"  Presence Detect Changed Enable(3):                  %E%d%N\n",
    PCIE_CAP_PRES_DETECT_CHANGE_ENABLE (PcieSlotControl)
    );
  Print (
    L"  Command Completed Interrupt Enable(4):              %E%d%N\n",
    PCIE_CAP_COMM_CMPL_INT_ENABLE (PcieSlotControl)
    );
  Print (
    L"  Hot-Plug Interrupt Enable(5):                       %E%d%N\n",
    PCIE_CAP_HOTPLUG_INT_ENABLE (PcieSlotControl)
    );
  Print (
    L"  Attention Indicator Control(7:6):                   %E%s%N\n",
    IndicatorTable[PCIE_CAP_ATT_IND_CTRL (PcieSlotControl)]
    );
  Print (
    L"  Power Indicator Control(9:8):                       %E%s%N\n",
    IndicatorTable[PCIE_CAP_PWR_IND_CTRL (PcieSlotControl)]
    );
  Print (L"  Power Controller Control(10):                       %EPower ");
  if (PCIE_CAP_PWR_CTRLLER_CTRL (PcieSlotControl)) {
    Print (L"Off%N\n");
  } else {
    Print (L"On%N\n");
  }
  Print (
    L"  Electromechanical Interlock Control(11):            %E%d%N\n",
    PCIE_CAP_ELEC_INTERLOCK_CTRL (PcieSlotControl)
    );
  Print (
    L"  Data Link Layer State Changed Enable(12):           %E%d%N\n",
    PCIE_CAP_DLINK_STAT_CHANGE_ENABLE (PcieSlotControl)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieSlotStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieSlotStatus;

  PcieSlotStatus = PciExpressCap->SlotStatus;

  Print (
    L"  Attention Button Pressed(0):           %E%d%N\n",
    PCIE_CAP_ATT_BUT_PRESSED (PcieSlotStatus)
    );
  Print (
    L"  Power Fault Detected(1):               %E%d%N\n",
    PCIE_CAP_PWR_FLT_DETECTED (PcieSlotStatus)
    );
  Print (
    L"  MRL Sensor Changed(2):                 %E%d%N\n",
    PCIE_CAP_MRL_SENSOR_CHANGED (PcieSlotStatus)
    );
  Print (
    L"  Presence Detect Changed(3):            %E%d%N\n",
    PCIE_CAP_PRES_DETECT_CHANGED (PcieSlotStatus)
    );
  Print (
    L"  Command Completed(4):                  %E%d%N\n",
    PCIE_CAP_COMM_COMPLETED (PcieSlotStatus)
    );
  Print (L"  MRL Sensor State(5):                   %EMRL ");
  if (PCIE_CAP_MRL_SENSOR_STATE (PcieSlotStatus)) {
    Print (L" Opened%N\n");
  } else {
    Print (L" Closed%N\n");
  }
  Print (L"  Presence Detect State(6):              ");
  if (PCIE_CAP_PRES_DETECT_STATE (PcieSlotStatus)) {
    Print (L"%ECard Present in slot%N\n");
  } else {
    Print (L"%ESlot Empty%N\n");
  }
  Print (L"  Electromechanical Interlock Status(7): %EElectromechanical Interlock ");
  if (PCIE_CAP_ELEC_INTERLOCK_STATE (PcieSlotStatus)) {
    Print (L"Engaged%N\n");
  } else {
    Print (L"Disengaged%N\n");
  }
  Print (
    L"  Data Link Layer State Changed(8):      %E%d%N\n",
    PCIE_CAP_DLINK_STAT_CHANGED (PcieSlotStatus)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieRootControl (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieRootControl;

  PcieRootControl = PciExpressCap->RootControl;

  Print (
    L"  System Error on Correctable Error Enable(0):  %E%d%N\n",
    PCIE_CAP_SYSERR_ON_CORERR_EN (PcieRootControl)
    );
  Print (
    L"  System Error on Non-Fatal Error Enable(1):    %E%d%N\n",
    PCIE_CAP_SYSERR_ON_NONFATERR_EN (PcieRootControl)
    );
  Print (
    L"  System Error on Fatal Error Enable(2):        %E%d%N\n",
    PCIE_CAP_SYSERR_ON_FATERR_EN (PcieRootControl)
    );
  Print (
    L"  PME Interrupt Enable(3):                      %E%d%N\n",
    PCIE_CAP_PME_INT_ENABLE (PcieRootControl)
    );
  Print (
    L"  CRS Software Visibility Enable(4):            %E%d%N\n",
    PCIE_CAP_CRS_SW_VIS_ENABLE (PcieRootControl)
    );
    
  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieRootCap (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT16 PcieRootCap;

  PcieRootCap = PciExpressCap->RsvdP;

  Print (
    L"  CRS Software Visibility(0):                   %E%d%N\n",
    PCIE_CAP_CRS_SW_VIS (PcieRootCap)
    );

  return EFI_SUCCESS;
}

EFI_STATUS
ExplainPcieRootStatus (
  IN PCIE_CAP_STURCTURE *PciExpressCap
)
{
  UINT32 PcieRootStatus;

  PcieRootStatus = PciExpressCap->RootStatus;

  Print (
    L"  PME Requester ID(15:0):                       %E0x%04x%N\n",
    PCIE_CAP_PME_REQ_ID (PcieRootStatus)
    );
  Print (
    L"  PME Status(16):                               %E%d%N\n",
    PCIE_CAP_PME_STATUS (PcieRootStatus)
    );
  Print (
    L"  PME Pending(17):                              %E%d%N\n",
    PCIE_CAP_PME_PENDING (PcieRootStatus)
    );
  return EFI_SUCCESS;
}

EFI_STATUS
PciExplainPciExpress (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *IoDev,
  IN  UINT64                                  Address,
  IN  UINT8                                   CapabilityPtr
  )
{

  PCIE_CAP_STURCTURE  PciExpressCap;
  EFI_STATUS          Status;
  UINT64              CapRegAddress;
  UINT8               Bus;
  UINT8               Dev;
  UINT8               Func;
  UINT8               *ExRegBuffer;
  UINTN               ExtendRegSize;
  UINT64              Pciex_Address;
  UINT8               DevicePortType;
  UINTN               Index;
  UINT8               *RegAddr;
  UINTN               RegValue;

  CapRegAddress = Address + CapabilityPtr;
  IoDev->Pci.Read (
              IoDev,
              EfiPciWidthUint32,
              CapRegAddress,
              sizeof (PciExpressCap) / sizeof (UINT32),
              &PciExpressCap
              );

  DevicePortType = (UINT8) PCIE_CAP_DEVICEPORT_TYPE (PciExpressCap.PcieCapReg);
  
  Print (L"\nPci Express device capability structure:\n");

  for (Index = 0; PcieExplainList[Index].Type < PcieExplainTypeMax; Index++) {
    if (GetExecutionBreak ()) {
      goto Done;
    }
    RegAddr = ((UINT8 *) &PciExpressCap) + PcieExplainList[Index].Offset;
    switch (PcieExplainList[Index].Width) {
      case FieldWidthUINT8:
        RegValue = *(UINT8 *) RegAddr;
        break;
      case FieldWidthUINT16:
        RegValue = *(UINT16 *) RegAddr;
        break;
      case FieldWidthUINT32:
        RegValue = *(UINT32 *) RegAddr;
        break;
      default:
        RegValue = 0;
        break;
    }
    PrintToken (
      PcieExplainList[Index].Token,
      HiiHandle,
      PcieExplainList[Index].Offset,
      RegValue
      );
    if (PcieExplainList[Index].Func == NULL) {
      continue;
    }
    switch (PcieExplainList[Index].Type) {
      case PcieExplainTypeLink:
        //
        // Link registers should not be used by
        // a) Root Complex Integrated Endpoint
        // b) Root Complex Event Collector
        //
        if (DevicePortType == PCIE_ROOT_COMPLEX_INTEGRATED_PORT ||
            DevicePortType == PCIE_ROOT_COMPLEX_EVENT_COLLECTOR) {
          continue;
        }
        break;
      case PcieExplainTypeSlot:
        //
        // Slot registers are only valid for
        // a) Root Port of PCI Express Root Complex
        // b) Downstream Port of PCI Express Switch
        // and when SlotImplemented bit is set in PCIE cap register.
        //
        if ((DevicePortType != PCIE_ROOT_COMPLEX_ROOT_PORT &&
             DevicePortType != PCIE_SWITCH_DOWNSTREAM_PORT) ||
            !PCIE_CAP_SLOT_IMPLEMENTED (PciExpressCap.PcieCapReg)) {
          continue;
        }
        break;
      case PcieExplainTypeRoot:
        //
        // Root registers are only valid for
        // Root Port of PCI Express Root Complex
        //
        if (DevicePortType != PCIE_ROOT_COMPLEX_ROOT_PORT) {
          continue;
        }
        break;
      default:
        break;
    }
    PcieExplainList[Index].Func (&PciExpressCap);
  }

  Bus           = (UINT8) (RShiftU64 (Address, 24));
  Dev           = (UINT8) (RShiftU64 (Address, 16));
  Func          = (UINT8) (RShiftU64 (Address, 8));

  Pciex_Address = CALC_EFI_PCIEX_ADDRESS (Bus, Dev, Func, 0x100);

  ExtendRegSize = 0x1000 - 0x100;

  ExRegBuffer   = (UINT8 *) AllocatePool (ExtendRegSize);

  //
  // PciRootBridgeIo protocol should support pci express extend space IO
  // (Begins at offset 0x100)
  //
  Status = IoDev->Pci.Read (
                        IoDev,
                        EfiPciWidthUint32,
                        Pciex_Address,
                        (ExtendRegSize) / sizeof (UINT32),
                        (VOID *) (ExRegBuffer)
                        );
  if (EFI_ERROR (Status)) {
    FreePool ((VOID *) ExRegBuffer);
    return EFI_UNSUPPORTED;
  }
  //
  // Start outputing PciEx extend space( 0xFF-0xFFF)
  //
  Print (L"\n%HStart dumping PCIex extended configuration space (0x100 - 0xFFF).%N\n\n");

  PrivateDumpHex (
    2,
    0x100,
    ExtendRegSize,
    (VOID *) (ExRegBuffer)
    );

  FreePool ((VOID *) ExRegBuffer);

Done:
  return EFI_SUCCESS;
}

EFI_STATUS
InitializePciGetLineHelp (
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
  return LibCmdGetStringByToken (
          STRING_ARRAY_NAME,
          &EfiPciGuid,
          STRING_TOKEN (STR_HELPINFO_PCI_LINEHELP),
          Str
          );
}
