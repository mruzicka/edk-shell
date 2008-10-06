/*++

Copyright (c) 2005 - 2008, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  Misc.c

Abstract:

  Misc functions

Revision History

--*/

#include "EfiShelllib.h"

#if (PLATFORM == NT32)
#define LOCAL_EFI_WIN_NT_THUNK_PROTOCOL_GUID \
  { \
    0x58c518b1, 0x76f3, 0x11d4, 0xbc, 0xea, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 \
  }

#define LOCAL_EFI_WIN_NT_BUS_DRIVER_IO_PROTOCOL_GUID \
  { \
    0x96eb4ad6, 0xa32a, 0x11d4, 0xbc, 0xfd, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 \
  }

#define LOCAL_EFI_WIN_NT_SERIAL_PORT_GUID \
  { \
    0xc95a93d, 0xa006, 0x11d4, 0xbc, 0xfa, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 \
  }
EFI_GUID WinNtThunkProtocolGuid = LOCAL_EFI_WIN_NT_THUNK_PROTOCOL_GUID;
EFI_GUID WinNtIoProtocolGuid    = LOCAL_EFI_WIN_NT_BUS_DRIVER_IO_PROTOCOL_GUID;
EFI_GUID WinNtSerialPortGuid    = LOCAL_EFI_WIN_NT_SERIAL_PORT_GUID;
#endif

EFI_GUID        ShellInterfaceProtocol  = SHELL_INTERFACE_PROTOCOL;
EFI_GUID        PcAnsiProtocol          = DEVICE_PATH_MESSAGING_PC_ANSI;
EFI_GUID        Vt100Protocol           = DEVICE_PATH_MESSAGING_VT_100;
EFI_GUID        Vt100PlusProtocol       = DEVICE_PATH_MESSAGING_VT_100_PLUS;
EFI_GUID        VtUtf8Protocol          = DEVICE_PATH_MESSAGING_VT_UTF8;

#define DEFAULT_FORM_BUFFER_SIZE  0xFFFF

struct {
  EFI_GUID  *Guid;
  CHAR16    *GuidName;
}
KnownGuids[] = {
  //
  // Loaded Image
  //
  &gEfiLoadedImageProtocolGuid,
  L"Image",
  //
  // Device Path
  //
  &gEfiDevicePathProtocolGuid,
  L"Dpath",
  &gEfiLoadedImageDevicePathProtocolGuid,
  L"ImageDPath",
  &gEfiDevicePathUtilitiesProtocolGuid,
  L"DpathUtil",
  &gEfiDevicePathToTextProtocolGuid,
  L"DpathToText",
  &gEfiDevicePathFromTextProtocolGuid,
  L"DpathFromText",
  &PcAnsiProtocol,
  L"PcAnsi",
  &Vt100Protocol,
  L"Vt100",
  &Vt100PlusProtocol,
  L"Vt100+",
  &VtUtf8Protocol,
  L"VtUtf8",
  //
  // Driver Model
  //
  &gEfiDriverBindingProtocolGuid,
  L"DriverBinding",
  &gEfiPlatformDriverOverrideProtocolGuid,
  L"PlatformOverride",
  &gEfiBusSpecificDriverOverrideProtocolGuid,
  L"BusSpecificDriverOverride",
  &gEfiDriverDiagnosticsProtocolGuid,
  L"Diagnostics",
  &gEfiDriverDiagnostics2ProtocolGuid,
  L"Diagnostics2",
  &gEfiComponentNameProtocolGuid,
  L"ComponentName",
  &gEfiComponentName2ProtocolGuid,
  L"ComponentName2",
  &gEfiPlatformToDriverConfigurationProtocolGuid,
  L"PlatformDriverConfig",
  &gEfiDriverSupportedEfiVersionProtocolGuid,
  L"DriverEFIVersion",
  //
  // Console Support
  //
  &gEfiSimpleTextInputExProtocolGuid,
  L"TxtinEx",
  &gEfiSimpleTextInProtocolGuid,
  L"Txtin",
  &gEfiSimpleTextOutProtocolGuid,
  L"Txtout",
  &gEfiSimplePointerProtocolGuid,
  L"SimplePointer",
  &gEfiAbsolutePointerProtocolGuid,
  L"AbsolutePointer",
  &gEfiSerialIoProtocolGuid,
  L"SerialIo",
  &gEfiGraphicsOutputProtocolGuid,
  L"GraphicsOutput",
  &gEfiEdidDiscoveredProtocolGuid,
  L"EdidDiscovered",
  &gEfiEdidActiveProtocolGuid,
  L"EdidActive",
  &gEfiEdidOverrideProtocolGuid,
  L"EdidOverride",
  &gEfiConsoleInDeviceGuid,
  L"ConIn",
  &gEfiConsoleOutDeviceGuid,
  L"ConOut",
  &gEfiStandardErrorDeviceGuid,
  L"StdErr",
  //
  // Media Access
  //
  &gEfiLoadFileProtocolGuid,
  L"Load",
  &gEfiSimpleFileSystemProtocolGuid,
  L"Fs",
  &gEfiFileInfoGuid,
  L"GenFileInfo",
  &gEfiFileSystemInfoGuid,
  L"FileSysInfo",
  &gEfiTapeIoProtocolGuid,
  L"TapeIo",
  &gEfiDiskIoProtocolGuid,
  L"DiskIo",
  &gEfiBlockIoProtocolGuid,
  L"BlkIo",
  &gEfiUnicodeCollationProtocolGuid,
  L"UnicodeCollation",
  &gEfiUnicodeCollation2ProtocolGuid,
  L"UnicodeCollation2",
  //
  // PCI Bus Support
  //
  &gEfiPciRootBridgeIoProtocolGuid,
  L"PciRootBridgeIo",
  &gEfiPciIoProtocolGuid,
  L"PciIo",
  //
  // SCSI Bus Support
  //
  &gEfiScsiPassThruProtocolGuid,
  L"ScsiPassThru",
  &gEfiScsiIoProtocolGuid,
  L"ScsiIo",
  &gEfiExtScsiPassThruProtocolGuid,
  L"ExtScsiPassThru",
  //
  // iSCSI
  //
  &gEfiIScsiInitiatorNameProtocolGuid,
  L"IScsiInitName",
  //
  // USB Support
  //
  &gEfiUsbIoProtocolGuid,
  L"UsbIo",
  &gEfiUsb2HcProtocolGuid,
  L"UsbHc",
  &gEfiUsb2HcProtocolGuid,
  L"UsbHc2", 
  //
  // Debugger Support
  //
  &gEfiDebugSupportProtocolGuid,
  L"DebugSupport",
  &gEfiDebugPortDevicePathGuid,
  L"DebugPort",
  //
  // Decompression Algorithm
  //
  &gEfiDecompressProtocolGuid,
  L"Decompress",
  //
  // ACPI
  //
  &gEfiAcpiTableProtocolGuid,
  L"AcpiTable",
  // EBC
  //
  &gEfiEbcProtocolGuid,
  L"EbcInterp",
  //
  // SNP, PXE, BIS
  //
  &gEfiSimpleNetworkProtocolGuid,
  L"Net",
  &gEfiNetworkInterfaceIdentifierProtocolGuid,
  L"Nii",
  &gEfiPxeBaseCodeProtocolGuid,
  L"Pxebc",
  &gEfiPxeBaseCodeCallbackProtocolGuid,
  L"PxebcCallback",
  &gEfiBisProtocolGuid,
  L"Bis",
  //
  // Managed Network
  //
  &gEfiManagedNetworkServiceBindingProtocolGuid,
  L"MNPSb",
  &gEfiManagedNetworkProtocolGuid,
  L"MNP",
  //
  // ARP, DHCPv4
  //
  &gEfiArpServiceBindingProtocolGuid,
  L"ARPSb",
  &gEfiArpProtocolGuid,
  L"ARP",
  &gEfiDhcp4ServiceBindingProtocolGuid,
  L"DHCPv4Sb",
  &gEfiDhcp4ProtocolGuid,
  L"DHCPv4",
  //
  // TCPv4, IPv4 and Configuration
  //
  &gEfiTcp4ServiceBindingProtocolGuid,
  L"TCPv4Sb",
  &gEfiTcp4ProtocolGuid,
  L"TCPv4",
  &gEfiIp4ServiceBindingProtocolGuid,
  L"IPv4Sb",
  &gEfiIp4ProtocolGuid,
  L"IPv4",
  &gEfiIp4ConfigProtocolGuid,
  L"IPv4Config",
  //
  // UDPv4, MTFTPv4
  //
  &gEfiUdp4ServiceBindingProtocolGuid,
  L"UDPv4Sb",
  &gEfiUdp4ProtocolGuid,
  L"UDPv4",
  &gEfiMtftp4ServiceBindingProtocolGuid,
  L"MTFTPv4Sb",
  &gEfiMtftp4ProtocolGuid,
  L"MTFTPv4",
  //
  // Security
  //
  &gEfiAuthenticationInfoProtocolGuid,
  L"AuthInfo",
  &gEfiHashServiceBindingProtocolGuid,
  L"HashSb",
  &gEfiHashProtocolGuid,
  L"Hash",
  //
  // HII 
  //
#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
  &gEfiHiiFontProtocolGuid,
  L"HiiFont",
  &gEfiHiiStringProtocolGuid,
  L"HiiString",
  &gEfiHiiImageProtocolGuid,
  L"HiiImage",
  &gEfiHiiDatabaseProtocolGuid,
  L"HiiDatabase",
  //
  // HII Configuration Processing and Browser
  //
  &gEfiHiiConfigRoutingProtocolGuid,
  L"HiiConfRouting",
  &gEfiHiiConfigAccessProtocolGuid,
  L"HiiConfAccess",
  &gEfiFormBrowser2ProtocolGuid,
  L"FormBrowser2",
#else
  &gEfiHiiProtocolGuid,
  L"Hii",
  &gEfiFormBrowserProtocolGuid,
  L"FormBrowser",
  &gEfiFormCallbackProtocolGuid,
  L"FormCallback",
#endif
  //
  // Shell Specific
  //
  &NullGuid,
  L"G0",
  &ShellInterfaceProtocol,
  L"ShellInt",
  //
  // Deprecated
  //
  &gEfiDeviceIoProtocolGuid,
  L"DevIo",
  &gEfiTcpProtocolGuid,
  L"Tcp",
  &gEfiUgaDrawProtocolGuid,
  L"UgaDraw",
  &gEfiUgaIoProtocolGuid,
  L"UgaIo",
  &gEfiGlobalVariableGuid,
  L"Efi",
  &gEfiFileSystemInfoGuid,
  L"FileSysInfo",
  &gEfiPartTypeSystemPartGuid,
  L"ESP",
  &gEfiPartTypeLegacyMbrGuid,
  L"GPT MBR",
  &gEfiDriverConfigurationProtocolGuid,
  L"Configuration",
  &gEfiDriverConfiguration2ProtocolGuid,
  L"Configuration2",
  &gEfiIsaIoProtocolGuid,
  L"IsaIo",
  &gEfiIsaAcpiProtocolGuid,
  L"IsaAcpi",
  //
  // NT32
  //
#if (PLATFORM == NT32)
  &WinNtThunkProtocolGuid,
  L"WinNtThunk",
  &WinNtIoProtocolGuid,
  L"WinNtDriverIo",
  &WinNtSerialPortGuid,
  L"SerialPrivate",
#endif
  //
  // Misc
  //
  &gEfiVariableStoreProtocolGuid,
  L"varstore",
  &UnknownDeviceGuid,
  L"Unknown Device",

  NULL
};

STATIC CHAR8  Hex[] = {
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  'A',
  'B',
  'C',
  'D',
  'E',
  'F'
};

CHAR16        *ShellLibMemoryTypeDesc[EfiMaxMemoryType] = {
  L"reserved  ",
  L"LoaderCode",
  L"LoaderData",
  L"BS_code   ",
  L"BS_data   ",
  L"RT_code   ",
  L"RT_data   ",
  L"available ",
  L"Unusable  ",
  L"ACPI_recl ",
  L"ACPI_NVS  ",
  L"MemMapIO  ",
  L"MemPortIO ",
  L"PAL_code  "
};

CHAR8  ca[] = { 3, 1, 2 };

VOID *
LibGetVariableLang (
  VOID
  )
/*++

Routine Description:
  Function returns the value of the Language Variable.

Arguments:
  None

Returns:

  None

--*/
{
  VOID *Var;

  Var = LibGetVariable (L"PlatformLang", &gEfiGlobalVariableGuid);
  if (Var == NULL) {
    Var = LibGetVariable (L"Lang", &gEfiGlobalVariableGuid);
  }

  return Var;
}

VOID *
LibGetVariable (
  IN CHAR16               *Name,
  IN EFI_GUID             *VendorGuid
  )
/*++

Routine Description:
  Function returns the value of the specified variable.

Arguments:
  Name                - A Null-terminated Unicode string that is 
                        the name of the vendor's variable.

  VendorGuid          - A unique identifier for the vendor.

Returns:

  None

--*/
{
  UINTN VarSize;

  return LibGetVariableAndSize (Name, VendorGuid, &VarSize);
}

VOID *
LibGetVariableAndSize (
  IN CHAR16               *Name,
  IN EFI_GUID             *VendorGuid,
  OUT UINTN               *VarSize
  )
/*++

Routine Description:
  Function returns the value of the specified variable and its size in bytes.

Arguments:
  Name                - A Null-terminated Unicode string that is 
                        the name of the vendor's variable.

  VendorGuid          - A unique identifier for the vendor.

  VarSize             - The size of the returned environment variable in bytes.

Returns:

  None

--*/
{
  EFI_STATUS  Status;
  VOID        *Buffer;
  UINTN       BufferSize;

  ASSERT (VarSize != NULL);

  //
  // Initialize for GrowBuffer loop
  //
  Buffer      = NULL;
  BufferSize  = 100;

  //
  // Call the real function
  //
  while (GrowBuffer (&Status, &Buffer, BufferSize)) {
    Status = RT->GetVariable (
                  Name,
                  VendorGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );
  }

  if (Buffer) {
    *VarSize = BufferSize;
  } else {
    *VarSize = 0;
  }

  return Buffer;
}

BOOLEAN
GrowBuffer (
  IN OUT EFI_STATUS   *Status,
  IN OUT VOID         **Buffer,
  IN UINTN            BufferSize
  )
/*++

Routine Description:

  Helper function called as part of the code needed
  to allocate the proper sized buffer for various 
  EFI interfaces.

Arguments:

  Status      - Current status

  Buffer      - Current allocated buffer, or NULL

  BufferSize  - Current buffer size needed
    
Returns:
    
  TRUE - if the buffer was reallocated and the caller 
  should try the API again.

--*/
{
  BOOLEAN TryAgain;

  ASSERT (Status != NULL);
  ASSERT (Buffer != NULL);

  //
  // If this is an initial request, buffer will be null with a new buffer size
  //
  if (NULL == *Buffer && BufferSize) {
    *Status = EFI_BUFFER_TOO_SMALL;
  }
  //
  // If the status code is "buffer too small", resize the buffer
  //
  TryAgain = FALSE;
  if (*Status == EFI_BUFFER_TOO_SMALL) {

    if (*Buffer) {
      FreePool (*Buffer);
    }

    *Buffer = AllocateZeroPool (BufferSize);

    if (*Buffer) {
      TryAgain = TRUE;
    } else {
      *Status = EFI_OUT_OF_RESOURCES;
    }
  }
  //
  // If there's an error, free the buffer
  //
  if (!TryAgain && EFI_ERROR (*Status) && *Buffer) {
    FreePool (*Buffer);
    *Buffer = NULL;
  }

  return TryAgain;
}

INTN
CompareGuid (
  IN EFI_GUID     *Guid1,
  IN EFI_GUID     *Guid2
  )
/*++

Routine Description:

  Compares to GUIDs

Arguments:

  Guid1       - guid to compare
  Guid2       - guid to compare

Returns:
  = 0     if Guid1 == Guid2

--*/
{
  INT32 *g1;

  INT32 *g2;

  INT32 r;

  ASSERT (Guid1 != NULL);
  ASSERT (Guid2 != NULL);

  //
  // Compare 32 bits at a time
  //
  g1  = (INT32 *) Guid1;
  g2  = (INT32 *) Guid2;

  r   = g1[0] - g2[0];
  r |= g1[1] - g2[1];
  r |= g1[2] - g2[2];
  r |= g1[3] - g2[3];

  return r;
}

VOID
GuidToString (
  OUT CHAR16      *Buffer,
  IN EFI_GUID     *Guid
  )
/*++

Routine Description:

  Converts Guid to a string

Arguments:

  Buffer      - On return, a pointer to the buffer which contains the string.
  Guid        - guid to compare

Returns:
  none

--*/
{
  UINTN Index;

  ASSERT (Guid != NULL);
  ASSERT (Buffer != NULL);

  //
  // Else, (for now) use additional internal function for mapping guids
  //
  for (Index = 0; KnownGuids[Index].Guid; Index++) {
    if (CompareGuid (Guid, KnownGuids[Index].Guid) == 0) {
      SPrint (Buffer, 0, KnownGuids[Index].GuidName);
      return ;
    }
  }
  //
  // Else dump it
  //
  SPrint (
    Buffer,
    0,
    L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    (UINTN) Guid->Data1,
    (UINTN) Guid->Data2,
    (UINTN) Guid->Data3,
    (UINTN) Guid->Data4[0],
    (UINTN) Guid->Data4[1],
    (UINTN) Guid->Data4[2],
    (UINTN) Guid->Data4[3],
    (UINTN) Guid->Data4[4],
    (UINTN) Guid->Data4[5],
    (UINTN) Guid->Data4[6],
    (UINTN) Guid->Data4[7]
    );
}

VOID
StatusToString (
  OUT CHAR16          *Buffer,
  IN EFI_STATUS       Status
  )
/*++

Routine Description:

  Function that converts an EFI status to a string

Arguments:

  Buffer           - On return, a pointer to the buffer containing the string 
  Status           - A status value

Returns:

  none

Notes:
  
  An IF construct is used rather than a pre-initialized array because the
  EFI status codes are UINTN values which are not constants when compiling
  for EBC. As such, the values cannot be used in pre-initialized structures.
  
--*/
{
  CHAR16  *Message;
  
  ASSERT (Buffer != NULL);

  Message = NULL;

  //
  // Can't use global Status String Array as UINTN is not constant for EBC
  //
  if (Status == EFI_SUCCESS) { Message = L"Success"; } else
  if (Status == EFI_LOAD_ERROR) { Message = L"Load Error"; } else
  if (Status == EFI_INVALID_PARAMETER) { Message = L"Invalid Parameter"; } else
  if (Status == EFI_UNSUPPORTED) { Message = L"Unsupported"; } else
  if (Status == EFI_BAD_BUFFER_SIZE) { Message = L"Bad Buffer Size"; } else
  if (Status == EFI_BUFFER_TOO_SMALL) { Message = L"Buffer Too Small"; } else
  if (Status == EFI_NOT_READY) { Message = L"Not Ready"; } else
  if (Status == EFI_DEVICE_ERROR) { Message = L"Device Error"; } else
  if (Status == EFI_WRITE_PROTECTED) { Message = L"Write Protected"; } else
  if (Status == EFI_OUT_OF_RESOURCES) { Message = L"Out of Resources"; } else
  if (Status == EFI_VOLUME_CORRUPTED) { Message = L"Volume Corrupted"; } else
  if (Status == EFI_VOLUME_FULL) { Message = L"Volume Full"; } else
  if (Status == EFI_NO_MEDIA) { Message = L"No Media"; } else
  if (Status == EFI_MEDIA_CHANGED) { Message = L"Media Changed"; } else
  if (Status == EFI_NOT_FOUND) { Message = L"Not Found"; } else
  if (Status == EFI_ACCESS_DENIED) { Message = L"Access Denied"; } else
  if (Status == EFI_NO_RESPONSE) { Message = L"No Response"; } else
  if (Status == EFI_NO_MAPPING) { Message = L"No Mapping"; } else
  if (Status == EFI_TIMEOUT) { Message = L"Time Out"; } else
  if (Status == EFI_NOT_STARTED) { Message = L"Not Started"; } else
  if (Status == EFI_ALREADY_STARTED) { Message = L"Already Started"; } else
  if (Status == EFI_ABORTED) { Message = L"Aborted"; } else
  if (Status == EFI_ICMP_ERROR) { Message = L"ICMP Error"; } else
  if (Status == EFI_TFTP_ERROR) { Message = L"TFTP Error"; } else
  if (Status == EFI_PROTOCOL_ERROR) { Message = L"Protocol Error"; } else
  if (Status == EFI_WARN_UNKNOWN_GLYPH) { Message = L"Warning Unknown Glyph"; } else
  if (Status == EFI_WARN_DELETE_FAILURE) { Message = L"Warning Delete Failure"; } else
  if (Status == EFI_WARN_WRITE_FAILURE) { Message = L"Warning Write Failure"; } else
  if (Status == EFI_WARN_BUFFER_TOO_SMALL) { Message = L"Warning Buffer Too Small"; } else
  if (Status == EFI_INCOMPATIBLE_VERSION) { Message = L"Incompatible Version"; } else
  if (Status == EFI_SECURITY_VIOLATION) { Message = L"Security Violation"; } else
  if (Status == EFI_CRC_ERROR) { Message = L"CRC Error"; } else
  if (Status == EFI_NOT_AVAILABLE_YET) { Message = L"Not Available Yet"; } else
  if (Status == EFI_UNLOAD_IMAGE) { Message = L"Unload Image"; } else
  if (Status == EFI_WARN_RETURN_FROM_LONG_JUMP) { Message = L"Warning Return From Long Jump"; }  
  
  //
  // If we found a match, then copy it to the user's buffer.
  // Otherwise SPrint the hex value into their buffer.
  //
  if (Message != NULL) {
    StrCpy (Buffer, Message);
  } else {
    SPrint (Buffer, 0, L"%X", Status);
  }
}

#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
EFI_STATUS
LibExtractDataFromHiiHandle (
  IN      EFI_HII_HANDLE      HiiHandle,
  IN OUT  UINT16              *ImageLength,
  OUT     UINT8               *DefaultImage,
  OUT     EFI_GUID            *Guid
  )
/*++

Routine Description:

  Extract information pertaining to the HiiHandle
  
Arguments:
  
  HiiHandle    - Hii handle
  ImageLength  - Image length
  DefaultImage - Default image
  Guid         - Guid

Returns: 

  EFI_OUT_OF_RESOURCES - Out of resource
  EFI_BUFFER_TOO_SMALL - Buffer too small
  EFI_SUCCESS          - Success
  
--*/
{
  EFI_STATUS        Status;
  EFI_HII_PROTOCOL  *Hii;
  UINTN             DataLength;
  UINT8             *RawData;
  UINT8             *OldData;
  UINTN             Index;
  UINTN             Temp;
  UINTN             SizeOfNvStore;
  UINTN             CachedStart;

  ASSERT (ImageLength != NULL);
  ASSERT (DefaultImage != NULL);
  ASSERT (Guid != NULL);

  DataLength    = DEFAULT_FORM_BUFFER_SIZE;
  SizeOfNvStore = 0;
  CachedStart   = 0;

  Status        = LibGetHiiInterface (&Hii);

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Allocate space for retrieval of IFR data
  //
  RawData = AllocateZeroPool ((UINTN) DataLength);
  if (RawData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get all the forms associated with this HiiHandle
  //
  Status = Hii->GetForms (Hii, HiiHandle, 0, &DataLength, RawData);

  if (EFI_ERROR (Status)) {
    FreePool (RawData);

    //
    // Allocate space for retrieval of IFR data
    //
    RawData = AllocateZeroPool ((UINTN) DataLength);
    if (RawData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Get all the forms associated with this HiiHandle
    //
    Status = Hii->GetForms (Hii, HiiHandle, 0, &DataLength, RawData);
  }

  OldData = RawData;

  //
  // Point RawData to the beginning of the form data
  //
  RawData = (UINT8 *) ((UINTN) RawData + sizeof (EFI_HII_PACK_HEADER));

  for (Index = 0; RawData[Index] != EFI_IFR_END_FORM_SET_OP;) {
    switch (RawData[Index]) {
    case EFI_IFR_FORM_SET_OP:
      //
      // Copy the GUID information from this handle
      //
      CopyMem (Guid, &((EFI_IFR_FORM_SET *) &RawData[Index])->Guid, sizeof (EFI_GUID));
      break;

    case EFI_IFR_ONE_OF_OP:
    case EFI_IFR_CHECKBOX_OP:
    case EFI_IFR_NUMERIC_OP:
    case EFI_IFR_DATE_OP:
    case EFI_IFR_TIME_OP:
    case EFI_IFR_PASSWORD_OP:
    case EFI_IFR_STRING_OP:
      //
      // Remember, multiple op-codes may reference the same item, so let's keep a running
      // marker of what the highest QuestionId that wasn't zero length.  This will accurately
      // maintain the Size of the NvStore
      //
      if (((EFI_IFR_ONE_OF *) &RawData[Index])->Width != 0) {
        Temp = ((EFI_IFR_ONE_OF *) &RawData[Index])->QuestionId + ((EFI_IFR_ONE_OF *) &RawData[Index])->Width;
        if (SizeOfNvStore < Temp) {
          SizeOfNvStore = ((EFI_IFR_ONE_OF *) &RawData[Index])->QuestionId + ((EFI_IFR_ONE_OF *) &RawData[Index])->Width;
        }
      }
    }

    Index = RawData[Index + 1] + Index;
  }
  //
  // Return an error if buffer is too small
  //
  if (SizeOfNvStore > *ImageLength) {
    FreePool (OldData);
    *ImageLength = (UINT16) SizeOfNvStore;
    return EFI_BUFFER_TOO_SMALL;
  }

  SetMem (DefaultImage, SizeOfNvStore, 0);

  //
  // Copy the default image information to the user's buffer
  //
  for (Index = 0; RawData[Index] != EFI_IFR_END_FORM_SET_OP;) {
    switch (RawData[Index]) {
    case EFI_IFR_ONE_OF_OP:
      CachedStart = ((EFI_IFR_ONE_OF *) &RawData[Index])->QuestionId;
      break;

    case EFI_IFR_ONE_OF_OPTION_OP:
      if (((EFI_IFR_ONE_OF_OPTION *) &RawData[Index])->Flags & EFI_IFR_FLAG_DEFAULT) {
        CopyMem (&DefaultImage[CachedStart], &((EFI_IFR_ONE_OF_OPTION *) &RawData[Index])->Value, 2);
      }
      break;

    case EFI_IFR_CHECKBOX_OP:
      DefaultImage[((EFI_IFR_ONE_OF *) &RawData[Index])->QuestionId] = ((EFI_IFR_CHECK_BOX *) &RawData[Index])->Flags;
      break;

    case EFI_IFR_NUMERIC_OP:
      CopyMem (
        &DefaultImage[((EFI_IFR_ONE_OF *) &RawData[Index])->QuestionId],
        &((EFI_IFR_NUMERIC *) &RawData[Index])->Default,
        2
        );
      break;

    }

    Index = RawData[Index + 1] + Index;
  }

  *ImageLength = (UINT16) SizeOfNvStore;

  //
  // Free our temporary repository of form data
  //
  FreePool (OldData);

  return EFI_SUCCESS;
}

EFI_STATUS
LibGetHiiInterface (
  OUT     EFI_HII_PROTOCOL    **Hii
  )
/*++

Routine Description:

  Get the HII protocol interface
  
Arguments:
  
  Hii - Hii handle
  
Returns: 

--*/
{
  EFI_STATUS  Status;

  ASSERT (Hii != NULL);

  //
  // There should only be one HII protocol
  //
  Status = LibLocateProtocol (
            &gEfiHiiProtocolGuid,
            (VOID **) Hii
            );

  return Status;
}

#endif

CHAR16 *
MemoryTypeStr (
  IN EFI_MEMORY_TYPE  Type
  )
/*++

Routine Description:
  
Arguments:

    Type - Memory type

Returns:
  
--*/
{
  return Type < EfiMaxMemoryType ? ShellLibMemoryTypeDesc[Type] : L"Unkown-Desc-Type";
}

VOID
ValueToHex (
  IN CHAR16   *Buffer,
  IN UINT64   v
  )
{
  CHAR8   str[30];

  CHAR8   *p1;
  CHAR16  *p2;

  if (!v) {
    Buffer[0] = '0';
    Buffer[1] = 0;
    return ;
  }

  p1  = str;
  p2  = Buffer;

  while (v) {
    *(p1++) = Hex[v & 0xf];
    v       = RShiftU64 (v, 4);
  }

  while (p1 != str) {
    *(p2++) = *(--p1);
  }

  *p2 = 0;
}

EFI_STATUS
LibGetSystemConfigurationTable (
  IN EFI_GUID *TableGuid,
  IN OUT VOID **Table
  )
/*++

Routine Description:
  Function returns a system configuration table that is stored in the 
  EFI System Table based on the provided GUID.

Arguments:
  TableGuid        - A pointer to the table's GUID type.

  Table            - On exit, a pointer to a system configuration table.

Returns:

  EFI_SUCCESS      - A configuration table matching TableGuid was found

  EFI_NOT_FOUND    - A configuration table matching TableGuid was not found

--*/
{
  UINTN Index;
  ASSERT (Table != NULL);

  for (Index = 0; Index < ST->NumberOfTableEntries; Index++) {
    if (CompareGuid (TableGuid, &(ST->ConfigurationTable[Index].VendorGuid)) == 0) {
      *Table = ST->ConfigurationTable[Index].VendorTable;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
ShellGetDeviceName (
  EFI_HANDLE  DeviceHandle,
  BOOLEAN     UseComponentName,
  BOOLEAN     UseDevicePath,
  CHAR8       *Language,
  CHAR16      **BestDeviceName,
  EFI_STATUS  *ConfigurationStatus,
  EFI_STATUS  *DiagnosticsStatus,
  BOOLEAN     Display,
  UINTN       Indent
  )
{
  return SE2->GetDeviceName (
                DeviceHandle,
                UseComponentName,
                UseDevicePath,
                Language,
                BestDeviceName,
                ConfigurationStatus,
                DiagnosticsStatus,
                Display,
                Indent
                );
}

EFI_MEMORY_DESCRIPTOR *
LibMemoryMap (
  OUT UINTN               *NoEntries,
  OUT UINTN               *MapKey,
  OUT UINTN               *DescriptorSize,
  OUT UINT32              *DescriptorVersion
  )
/*++

Routine Description:
  This function retrieves the system's current memory map.

Arguments:
  NoEntries                - A pointer to the number of memory descriptors in the system

  MapKey                   - A pointer to the current memory map key.

  DescriptorSize           - A pointer to the size in bytes of a memory descriptor

  DescriptorVersion        - A pointer to the version of the memory descriptor.

Returns:

  None

--*/
{
  EFI_STATUS            Status;
  EFI_MEMORY_DESCRIPTOR *Buffer;
  UINTN                 BufferSize;

  ASSERT (NoEntries != NULL);
  ASSERT (MapKey != NULL);
  ASSERT (DescriptorSize != NULL);
  ASSERT (DescriptorVersion != NULL);
  //
  // Initialize for GrowBuffer loop
  //
  Buffer      = NULL;
  BufferSize  = sizeof (EFI_MEMORY_DESCRIPTOR);

  //
  // Call the real function
  //
  while (GrowBuffer (&Status, (VOID **) &Buffer, BufferSize)) {
    Status = BS->GetMemoryMap (
                  &BufferSize,
                  Buffer,
                  MapKey,
                  DescriptorSize,
                  DescriptorVersion
                  );
  }
  //
  // Convert buffer size to NoEntries
  //
  if (!EFI_ERROR (Status)) {
    *NoEntries = BufferSize / *DescriptorSize;
  }

  return Buffer;
}

EFI_STATUS
LibDeleteVariable (
  IN CHAR16   *VarName,
  IN EFI_GUID *VarGuid
  )
/*++

Routine Description:
  Function deletes the variable specified by VarName and VarGuid.

Arguments:
  VarName              - A Null-terminated Unicode string that is 
                         the name of the vendor's variable.

  VarGuid              - A unique identifier for the vendor.

Returns:

  EFI_SUCCESS          - The variable was found and removed

  EFI_UNSUPPORTED      - The variable store was inaccessible

  EFI_OUT_OF_RESOURCES - The temporary buffer was not available

  EFI_NOT_FOUND        - The variable was not found

--*/
{
  VOID        *VarBuf;
  EFI_STATUS  Status;

  VarBuf  = LibGetVariable (VarName, VarGuid);

  Status  = EFI_NOT_FOUND;

  if (VarBuf) {
    //
    // Delete variable from Storage
    //
    Status = RT->SetVariable (
                  VarName,
                  VarGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  0,
                  NULL
                  );
    ASSERT (!EFI_ERROR (Status));
    FreePool (VarBuf);
  }

  return Status;
}

EFI_STATUS
LibInsertToTailOfBootOrder (
  IN  UINT16  BootOption,
  IN  BOOLEAN OnlyInsertIfEmpty
  )
/*++

Routine Description:
  Insert BootOption to the end of VarBootOrder

Arguments:
  BootOption           - Boot option variable to add to VarBootOrder

  OnlyInsertIfEmpty    - If VarBootOrder is empty, then add if TRUE.  
  
Returns:

  EFI_SUCCESS          - The BootOption was added to the VarBootOrder
  
  EFI_UNSUPPORTED      - Variable store was inaccessible or (VarBootOrder & OnlyInsertIfEmpty)

  EFI_OUT_OF_RESOURCES - The temporary buffer was not available

--*/
{
  UINT16      *BootOptionArray;
  UINT16      *NewBootOptionArray;
  UINTN       VarSize;
  UINTN       Index;
  EFI_STATUS  Status;

  BootOptionArray = LibGetVariableAndSize (VarBootOrder, &gEfiGlobalVariableGuid, &VarSize);
  if (VarSize != 0 && OnlyInsertIfEmpty) {
    if (BootOptionArray) {
      FreePool (BootOptionArray);
    }

    return EFI_UNSUPPORTED;
  }

  VarSize += sizeof (UINT16);
  NewBootOptionArray = AllocatePool (VarSize);

  for (Index = 0; Index < ((VarSize / sizeof (UINT16)) - 1); Index++) {
    NewBootOptionArray[Index] = BootOptionArray[Index];
  }
  //
  // Insert in the tail of the array
  //
  NewBootOptionArray[Index] = BootOption;

  Status = RT->SetVariable (
                VarBootOrder,
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                VarSize,
                (VOID *) NewBootOptionArray
                );

  if (NewBootOptionArray) {
    FreePool (NewBootOptionArray);
  }

  if (BootOptionArray) {
    FreePool (BootOptionArray);
  }

  return Status;
}

BOOLEAN
ValidMBR (
  IN  MASTER_BOOT_RECORD     *Mbr,
  IN  EFI_BLOCK_IO_PROTOCOL  *BlkIo
  )
/*++

Routine Description:
  Determine if the Mbr pointer is pointing to a valid master boot record.

Arguments:
  Mbr              - A pointer to a buffer which contains a possible Master Boot Record
                     structure.

  BlkIo            - A pointer to the Block IO protocol

Returns:

  TRUE             - The Mbr pointer is pointing at a valid Master Boot Record

  FALSE            - The Mbr pointer is not pointing at a valid Master Boot Record

--*/
{
  UINT32  StartingLBA;

  UINT32  EndingLBA;
  UINT32  NewEndingLBA;
  INTN    i;
  INTN    j;
  BOOLEAN ValidMbr;
  
  ASSERT (Mbr != NULL);
  ASSERT (BlkIo != NULL);

  if (Mbr->Signature != MBR_SIGNATURE) {
    //
    // The BPB also has this signature, so it can not be used alone.
    //
    return FALSE;
  }

  ValidMbr = FALSE;
  for (i = 0; i < MAX_MBR_PARTITIONS; i++) {
    if (Mbr->Partition[i].OSIndicator == 0x00 || EXTRACT_UINT32 (Mbr->Partition[i].SizeInLBA) == 0) {
      continue;
    }

    ValidMbr    = TRUE;
    StartingLBA = EXTRACT_UINT32 (Mbr->Partition[i].StartingLBA);
    EndingLBA   = StartingLBA + EXTRACT_UINT32 (Mbr->Partition[i].SizeInLBA) - 1;
    if (EndingLBA > BlkIo->Media->LastBlock) {
      //
      // Compatability Errata:
      // Some systems try to hide drive space with thier INT 13h driver
      // This does not hide space from the OS driver. This means the MBR
      // that gets created from DOS is smaller than the MBR created from
      // a real OS (NT & Win98). This leads to BlkIo->LastBlock being
      // wrong on some systems FDISKed by the OS.
      //
      //
      if (BlkIo->Media->LastBlock < MIN_MBR_DEVICE_SIZE) {
        //
        // If this is a very small device then trust the BlkIo->LastBlock
        //
        return FALSE;
      }

      if (EndingLBA > (BlkIo->Media->LastBlock + MBR_ERRATA_PAD)) {
        return FALSE;
      }

    }

    for (j = i + 1; j < MAX_MBR_PARTITIONS; j++) {
      if (Mbr->Partition[j].OSIndicator == 0x00 || EXTRACT_UINT32 (Mbr->Partition[j].SizeInLBA) == 0) {
        continue;
      }

      NewEndingLBA = EXTRACT_UINT32 (Mbr->Partition[j].StartingLBA) + EXTRACT_UINT32 (Mbr->Partition[j].SizeInLBA) - 1;
      if (NewEndingLBA >= StartingLBA && EXTRACT_UINT32 (Mbr->Partition[j].StartingLBA) <= EndingLBA) {
        //
        // This region overlaps with the i'th region
        //
        return FALSE;
      }
    }
  }
  //
  // Non of the regions overlapped so MBR is O.K.
  //
  return ValidMbr;
}

UINT8
DecimaltoBCD (
  IN  UINT8 DecValue
  )
/*++

Routine Description:
  Function converts a decimal to a BCD value.

Arguments:
  DecValue         - An 8 bit decimal value

Returns:

  UINT8            - Returns the BCD value of the DecValue

--*/
{
  UINTN High;

  UINTN Low;

  High  = DecValue / 10;
  Low   = DecValue - (High * 10);

  return (UINT8) (Low + (High << 4));
}

UINT8
BCDtoDecimal (
  IN  UINT8 BcdValue
  )
/*++

Routine Description:
  Function converts a BCD to a decimal value.

Arguments:
  BcdValue         - An 8 bit BCD value

Returns:

  UINT8            - Returns the decimal value of the BcdValue

--*/
{
  UINTN High;

  UINTN Low;

  High  = BcdValue >> 4;
  Low   = BcdValue - (High << 4);

  return (UINT8) (Low + (High * 10));
}

CHAR16 *
LibGetImageName (
  EFI_LOADED_IMAGE_PROTOCOL *Image
  )
/*++

Routine Description:
  Get the image name 

Arguments:
  Image - Image to search

Returns:
  Pointer into the image name if the image name is found,
  Otherwise a pointer to NULL.
  
--*/
{
  EFI_STATUS                        Status;
  EFI_DEVICE_PATH_PROTOCOL          *DevPath;
  EFI_DEVICE_PATH_PROTOCOL          *DevPathNode;
  VOID                              *Buffer;
  UINTN                             BufferSize;
  UINT32                            AuthenticationStatus;
  EFI_GUID                          *NameGuid;
  EFI_FIRMWARE_VOLUME_PROTOCOL      *FV;
  EFI_FIRMWARE_VOLUME2_PROTOCOL     *FV2;

  FV          = NULL;
  FV2         = NULL;
  Buffer      = NULL;
  BufferSize  = 0;

  DevPath     = UnpackDevicePath (Image->FilePath);

  if (DevPath == NULL) {
    return NULL;
  }

  DevPathNode = DevPath;

  while (!IsDevicePathEnd (DevPathNode)) {
    //
    // Find the Fv File path
    //
    NameGuid = GetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)DevPathNode);
    if (NameGuid != NULL) {
      Status = BS->HandleProtocol (
                    Image->DeviceHandle,
                    &gEfiFirmwareVolumeProtocolGuid,
                    &FV
                    );
      if (!EFI_ERROR (Status)) {
        Status = FV->ReadSection (
                      FV,
                      NameGuid,
                      EFI_SECTION_USER_INTERFACE,
                      0,
                      &Buffer,
                      &BufferSize,
                      &AuthenticationStatus
                      );
        if (!EFI_ERROR (Status)) {
          break;
        }

        Buffer = NULL;
      } else {
        Status = BS->HandleProtocol (
                      Image->DeviceHandle,
                      &gEfiFirmwareVolume2ProtocolGuid,
                      &FV2
                      );
        if (!EFI_ERROR (Status)) {
          Status = FV2->ReadSection (
                          FV2,
                          NameGuid,
                          EFI_SECTION_USER_INTERFACE,
                          0,
                          &Buffer,
                          &BufferSize,
                          &AuthenticationStatus
                          );
          if (!EFI_ERROR (Status)) {
            break;
          }

          Buffer = NULL;
        }
      }
    }
    //
    // Next device path node
    //
    DevPathNode = NextDevicePathNode (DevPathNode);
  }

  FreePool (DevPath);
  return Buffer;
}

CHAR16 *
GetShellMode (
  VOID
  )
/*++

Routine Description:
  Get the shell version variable
  
Arguments:
  
Returns:

--*/
{
  CHAR16      *Mode;
  EFI_STATUS  Status;
  Status = SE2->GetShellMode (&Mode);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  return Mode;
}

EFI_STATUS
LibGetStringByToken (
  IN     EFI_HII_HANDLE        HiiHandle,
  IN     UINT16                Token,
  IN     CHAR16                *Lang,
  IN OUT CHAR16                **String
  )
/*++

Routine Description:

    Get string according the string token

Arguments:

    HiiHandle             - The HII handle
    Token                 - The string token
    Lang                  - The string language
    String                - The string from token

Returns:

    EFI_UNSUPPORTED       - Unsupported
    EFI_OUT_OF_RESOURCES  - Out of resources
    EFI_SUCCESS           - Success

--*/
{
  EFI_STATUS        Status;
  UINTN             StringSize;
#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
  EFI_HII_PROTOCOL  *HiiProt;
#endif

  ASSERT (String);

#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
  Status = LibLocateProtocol (&gEfiHiiProtocolGuid, (VOID **) &HiiProt);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
#endif

  StringSize  = 0;
  *String     = NULL;

#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
  Status = HiiProt->GetString (
                      HiiProt,
                      HiiHandle,
                      Token,
                      FALSE,
                      NULL,
                      &StringSize,
                      *String
                      );
#else
  Status = LibGetString (HiiHandle, Token, *String, &StringSize);
#endif
  if (EFI_BUFFER_TOO_SMALL == Status) {
    *String = AllocatePool (StringSize);
    if (NULL == *String) {
      return EFI_OUT_OF_RESOURCES;
    }
#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
    Status = HiiProt->GetString (
                        HiiProt,
                        HiiHandle,
                        Token,
                        FALSE,
                        NULL,
                        &StringSize,
                        *String
                        );
#else
  Status = LibGetString (HiiHandle, Token, *String, &StringSize);
#endif
  }

  return Status;
}

EFI_STATUS
LibCmdGetStringByToken (
  IN     UINT8                *StringPack,
  IN     EFI_GUID             *StringPackGuid,
  IN     UINT16               Token,
  IN OUT CHAR16               **Str
  )
{
  EFI_STATUS      Status;
  CHAR16          *String;
  EFI_HII_HANDLE  HiiHandle;

  ASSERT (Str);

  Status = LibInitializeStrings (&HiiHandle, StringPack, StringPackGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Str = NULL;
  Status = LibGetStringByToken (
            HiiHandle,
            Token,
            NULL,
            &String
            );
  if (!EFI_ERROR (Status)) {
    Status = LibUnInitializeStrings ();
    if (!EFI_ERROR (Status)) {
      *Str = String;
    }
  }

  return Status;
}

VOID
QSort (
  IN OUT VOID                       *Buffer,
  IN     UINTN                      EleNum,
  IN     UINTN                      EleSize,
  IN     INTN                       (*Compare) (VOID *, VOID *)
  )
{
  VOID  *Ele;
  UINTN High;
  UINTN Low;

  if (EleNum <= 1) {
    return ;
  }

  Ele = AllocatePool (EleSize);
  CopyMem (Ele, Buffer, EleSize);
  High  = EleNum - 1;
  Low   = 0;

  while (TRUE) {
    while (Low < High && Compare (Ele, (UINT8 *) Buffer + High * EleSize) < 0) {
      High--;
    }

    if (Low < High) {
      CopyMem ((UINT8 *) Buffer + Low * EleSize, (UINT8 *) Buffer + High * EleSize, EleSize);
      Low++;
    } else {
      CopyMem ((UINT8 *) Buffer + Low * EleSize, Ele, EleSize);
      break;
    }

    while (Low < High && Compare (Ele, (UINT8 *) Buffer + Low * EleSize) > 0) {
      Low++;
    }

    if (Low < High) {
      CopyMem ((UINT8 *) Buffer + High * EleSize, (UINT8 *) Buffer + Low * EleSize, EleSize);
      High--;
    } else {
      CopyMem ((UINT8 *) Buffer + High * EleSize, Ele, EleSize);
      break;
    }
  }

  QSort (Buffer, Low, EleSize, Compare);
  QSort ((UINT8 *) Buffer + (High + 1) * EleSize, EleNum - High - 1, EleSize, Compare);
  FreePool (Ele);
}

VOID
LibFreeArgInfo (
  IN EFI_SHELL_ARG_INFO       *ArgInfo
  )
{
  //
  // in case there gonna be changes of EFI_SHELL_ARG_INFO
  //
  return ;
}

EFI_STATUS
LibFilterNullArgs (
  VOID
  )
/*++

Routine Description:

    Filter the NULL arguments

Arguments:

    None

Returns:

    EFI_OUT_OF_RESOURCES  - Out of resource
    EFI_SUCCESS           - Success

--*/
{
  CHAR16              **Argv;
  EFI_SHELL_ARG_INFO  *ArgInfo;
  UINTN               Argc;
  UINTN               Index;
  UINTN               Index2;
  EFI_STATUS          Status;

  Status  = EFI_SUCCESS;
  Argc    = SI->Argc;
  Index2  = 0;
  for (Index = 0; Index < SI->Argc; Index++) {
    if (!SI->Argv[Index][0]) {
      Argc--;
    }
  }

  Argv    = AllocateZeroPool (Argc * sizeof (CHAR16 *));
  ArgInfo = AllocateZeroPool (Argc * sizeof (EFI_SHELL_ARG_INFO));
  if (NULL == Argv || NULL == ArgInfo) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  for (Index = 0; Index < SI->Argc; Index++) {
    if (!SI->Argv[Index][0]) {
      continue;
    }

    Argv[Index2] = StrDuplicate (SI->Argv[Index]);
    if (NULL == Argv[Index2]) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    ArgInfo[Index2].Attributes = SI->ArgInfo[Index].Attributes;
    Index2++;
  }

  for (Index = 0; Index < SI->Argc; Index++) {
    LibFreeArgInfo (&SI->ArgInfo[Index]);
    FreePool (SI->Argv[Index]);
  }

  FreePool (SI->ArgInfo);
  FreePool (SI->Argv);
  SI->ArgInfo = ArgInfo;
  SI->Argv    = Argv;
  SI->Argc    = Argc;
  ArgInfo     = NULL;
  Argv        = NULL;

Done:
  if (ArgInfo) {
    for (Index = 0; Index < Index2; Index++) {
      LibFreeArgInfo (&ArgInfo[Index]);
    }

    FreePool (ArgInfo);
  }

  if (Argv) {
    FreePool (Argv);
  }

  return Status;
}

VOID
ValueToString (
  IN CHAR16   *Buffer,
  IN BOOLEAN  Comma,
  IN INT64    v
  )
{
  CHAR8         str[40];
  CHAR8         *p1;
  CHAR16        *p2;
  UINTN         c;
  UINTN         r;

  ASSERT (Buffer != NULL);

  if (!v) {
    Buffer[0] = '0';
    Buffer[1] = 0;
    return ;
  }

  p1  = str;
  p2  = Buffer;

  if (v < 0) {
    *(p2++) = '-';
    v       = -v;
  }

  while (v) {
    v       = (INT64) DivU64x32 ((UINT64) v, 10, &r);
    *(p1++) = (CHAR8) ((CHAR8) r + '0');
  }

  c = (Comma ? ca[(p1 - str) % 3] : 999) + 1;
  while (p1 != str) {

    c -= 1;
    if (!c) {
      *(p2++) = ',';
      c       = 3;
    }

    *(p2++) = *(--p1);
  }

  *p2 = 0;
}

VOID
TimeToString (
  OUT CHAR16      *Buffer,
  IN EFI_TIME     *Time
  )
{
  UINTN   Hour;

  UINTN   Year;
  CHAR16  AmPm;

  ASSERT (Buffer != NULL);

  AmPm  = 'a';
  Hour  = Time->Hour;
  if (Time->Hour == 0) {
    Hour = 12;
  } else if (Time->Hour >= 12) {
    AmPm = 'p';
    if (Time->Hour >= 13) {
      Hour -= 12;
    }
  }

  Year = Time->Year % 100;

  SPrint (
    Buffer,
    0,
    L"%02d/%02d/%02d  %02d:%02d%c",
    (UINTN) Time->Month,
    (UINTN) Time->Day,
    (UINTN) Year,
    (UINTN) Hour,
    (UINTN) Time->Minute,
    AmPm
    );
}

VOID
DumpHex (
  IN UINTN        Indent,
  IN UINTN        Offset,
  IN UINTN        DataSize,
  IN VOID         *UserData
  )
{
  UINT8 *Data;

  CHAR8 Val[50];

  CHAR8 Str[20];

  UINT8 c;
  UINTN Size;
  UINTN Index;
  
  ASSERT (UserData != NULL);

  Data = UserData;
  while (DataSize) {
    Size = 16;
    if (Size > DataSize) {
      Size = DataSize;
    }

    for (Index = 0; Index < Size; Index += 1) {
      c                   = Data[Index];
      Val[Index * 3 + 0]  = Hex[c >> 4];
      Val[Index * 3 + 1]  = Hex[c & 0xF];
      Val[Index * 3 + 2]  = (CHAR8) ((Index == 7) ? '-' : ' ');
      Str[Index]          = (CHAR8) ((c < ' ' || c > 'z') ? '.' : c);
    }

    Val[Index * 3]  = 0;
    Str[Index]      = 0;
    Print (L"%*a%X: %-.48a *%a*\n", Indent, "", Offset, Val, Str);

    Data += Size;
    Offset += Size;
    DataSize -= Size;
  }
}

BOOLEAN
PrivateDumpHex (
  IN UINTN        Indent,
  IN UINTN        Offset,
  IN UINTN        DataSize,
  IN VOID         *UserData
  )
/*++

Routine Description:

  Add page break feature to the DumpHex  

Arguments:
  Indent           - The indent space

  Offset           - The offset

  DataSize         - The data size
  
  UserData         - The data

Returns:

  TRUE             - The dump is broke
  FALSE            - The dump is completed
  
--*/
{
  UINTN DispSize;
  UINT8 *DispData;

  DispSize  = EFI_HEX_DISP_SIZE;
  DispData  = (UINT8 *) UserData;

  while (DataSize) {
    if (GetExecutionBreak ()) {
      return TRUE;
    }

    if (DataSize > EFI_HEX_DISP_SIZE) {
      DataSize -= EFI_HEX_DISP_SIZE;
    } else {
      DispSize  = DataSize;
      DataSize  = 0;
    }

    DumpHex (Indent, Offset + DispData - (UINT8 *) UserData, DispSize, DispData);
    DispData += DispSize;
  }

  return FALSE;
}

UINT16 *
LibGetMachineTypeString (
  IN UINT16   MachineType
  )
/*++

Routine Description:

  Get Machine Type string according to Machine Type code

Arguments:
  MachineType      - The Machine Type code

Returns:
  The Machine Type String
  
--*/
{
  switch (MachineType) {
  case EFI_IMAGE_MACHINE_EBC:
    return L"EBC";
  case EFI_IMAGE_MACHINE_IA32:
    return L"IA32";
  case EFI_IMAGE_MACHINE_X64:
    return L"X64";
  case EFI_IMAGE_MACHINE_IA64:
    return L"IA64";
  default:
    return L"UNKNOWN";
  }
}

EFI_STATUS
LibGetImageHeader (
  IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath,
  OUT EFI_IMAGE_DOS_HEADER        *DosHeader,
  OUT EFI_IMAGE_FILE_HEADER       *ImageHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER   *OptionalHeader
  )
/*++

Routine Description:

  Get the headers (dos, image, optional header) from an image

Arguments:
  DevicePath       - Location where the file locates
  DosHeader        - Pointer to dos header
  ImageHeader      - Pointer to image header
  OptionalHeader   - Pointer to optional header

Returns:
  EFI_SUCCESS           - Successfully get the machine type.
  EFI_NOT_FOUND         - The file is not found.
  EFI_LOAD_ERROR        - File is not a valid image file.
  
--*/
{
  EFI_STATUS           Status;
  EFI_FILE_HANDLE      ThisFile;
  UINT32               Signature;
  UINTN                BufferSize;
  UINT64               FileSize;

  Status = EFI_SUCCESS;

  ThisFile = LibOpenFilePath (DevicePath, EFI_FILE_MODE_READ);
  if (ThisFile == NULL) {
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  //
  // Get the file size
  //
  Status = LibGetFileSize (ThisFile, &FileSize);
  if (EFI_ERROR (Status)) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read dos header
  //
  BufferSize = sizeof (EFI_IMAGE_DOS_HEADER);
  Status = LibReadFile (ThisFile, &BufferSize, DosHeader);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_DOS_HEADER) ||
      FileSize <= DosHeader->e_lfanew ||
      DosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Move to PE signature
  //
  Status = LibSetPosition (ThisFile, DosHeader->e_lfanew);
  if (EFI_ERROR (Status)) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read and check PE signature
  //
  BufferSize = sizeof (Signature);
  Status = LibReadFile (ThisFile, &BufferSize, &Signature);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (Signature) ||
      Signature != EFI_IMAGE_NT_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read image header
  //
  BufferSize = EFI_IMAGE_SIZEOF_FILE_HEADER;
  Status = LibReadFile (ThisFile, &BufferSize, ImageHeader);
  if (EFI_ERROR (Status) ||
      BufferSize < EFI_IMAGE_SIZEOF_FILE_HEADER) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read optional header
  //
  BufferSize = ImageHeader->SizeOfOptionalHeader;
  Status = LibReadFile (ThisFile, &BufferSize, OptionalHeader);
  if (EFI_ERROR (Status) ||
      BufferSize < ImageHeader->SizeOfOptionalHeader) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Check PE32 or PE32+ magic
  //  
  if (OptionalHeader->Magic != EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
      OptionalHeader->Magic != EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

 Done:
  if (ThisFile != NULL) {
    LibCloseFile (ThisFile);
  }
  return Status;
}

EFI_STATUS
LibGetComponentNameProtocol (
  IN EFI_HANDLE                      DriverBindingHandle,
  OUT EFI_COMPONENT_NAME_PROTOCOL    **ComponentName,
  OUT EFI_COMPONENT_NAME2_PROTOCOL   **ComponentName2
  )
/*++

  Routine Description:

    Get the ComponentName or ComponentName2 protocol according to the driver binding handle

  Arguments:

    DriverBindingHandle   - The Handle of DriverBinding
    ComponentName         - Pointer to the ComponentName protocl pointer
    ComponentName2        - Pointer to the ComponentName2 protocl pointer

  Returns:

    EFI_INVALID_PARAMETER - The ComponentName and ComponentName2 parameters are invalid.
    EFI_NOT_FOUND         - Neither ComponentName nor ComponentName2 has been installed.

--*/
{
  EFI_STATUS                         Status;

  if ((ComponentName == NULL) || (ComponentName2 == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *ComponentName  = NULL;
  *ComponentName2 = NULL;

  Status = BS->OpenProtocol (
                 DriverBindingHandle,
                 &gEfiComponentName2ProtocolGuid,
                 (VOID **) ComponentName2,
                 NULL,
                 NULL,
                 EFI_OPEN_PROTOCOL_GET_PROTOCOL
                 );
  if (EFI_ERROR(Status)) {
    Status = BS->OpenProtocol (
                   DriverBindingHandle,
                   &gEfiComponentNameProtocolGuid,
                   (VOID **) ComponentName,
                   NULL,
                   NULL,
                   EFI_OPEN_PROTOCOL_GET_PROTOCOL
                   );
  }

  return Status;
}

CHAR8 *
LibConvertSupportedLanguage (
  IN CHAR8                           *SupportedLanguages,
  IN CHAR8                           *LangCode
  )
/*++

  Routine Description:

    Do some convertion for the ComponentName2 supported language. It do 
    the convertion just for english language code currently.

  Arguments:

    SupportedLanguages    - Pointer to SupportedLanguages of ComponentName2/ComponentName protocl.
    LangCode              - The language code in variable "PlatformLang" or "Lang".

  Returns:

    Return the duplication of Language if it is not english otherwise return 
    the supported english language code.

--*/
{
  CHAR8                              *Languages;
  CHAR8                              *Lang;
  UINTN                              Index;

  Lang  = NULL;
  Languages = NULL;

  //
  // treat all the english language code (en-xx or eng) equally
  //
  if ((strncmpa(LangCode, "en-", 3) == 0) || (strcmpa(LangCode, "eng") == 0)) {
    Languages = strstra(SupportedLanguages, "en-");
    if (Languages == NULL) {
      Languages = strstra(SupportedLanguages, "eng");
    }
  }

  //
  // Return NULL if it is not english
  //
  if (Languages == NULL) {
    Languages = LangCode;
  }

  //
  // duplicate the returned language code.
  //
  if (strstra(SupportedLanguages, "-") != NULL) {
    Lang = AllocateZeroPool(32);
    for(Index = 0; (Index < 31) && (Languages[Index] != '\0') && (Languages[Index] != ';'); Index++) {
      Lang[Index] = Languages[Index];
    }
    Lang[Index] = '\0';
  } else {
    Lang = AllocateZeroPool(4);
    for(Index = 0; (Index < 3) && (Languages[Index] != '\0'); Index++) {
      Lang[Index] = Languages[Index];
    }
    Lang[Index] = '\0';
  }
  return Lang;
}
