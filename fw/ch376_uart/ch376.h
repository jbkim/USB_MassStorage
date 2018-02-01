#ifndef __CH376_H__
#define __CH376_H__

#ifndef		TRUE
#define		TRUE	1
#define		FALSE	0
#endif

#ifndef		NULL
#define		NULL	0
#endif

#ifndef		HIGH
#define		HIGH	1
#define		LOW	0
#endif

#ifndef UINT8
    typedef unsigned char                UINT8;
#endif
#ifndef UINT16
    typedef unsigned short               UINT16;
#endif
#ifndef UINT32
    typedef unsigned long                UINT32;
#endif
#ifndef PUINT8
    typedef unsigned char               *PUINT8;
#endif
#ifndef PUINT16
    typedef unsigned short              *PUINT16;
#endif
#ifndef PUINT32
    typedef unsigned long               *PUINT32;
#endif
#ifndef UINT8V
    typedef unsigned char volatile       UINT8V;
#endif
#ifndef PUINT8V
    typedef unsigned char volatile      *PUINT8V;
#endif


#define	CMD_GET_IC_VER		 0x01
#define	CMD_SET_BAUDRATE	 0x02
#define	CMD_ENTER_SLEEP    0x03
#define	CMD_RESET_ALL      0x05
#define	CMD_CHECK_EXIST    0x06
#define	CMD_SET_SDO_INT    0x0B
#define	CMD_GET_FILE_SIZE  0x0C
#define	CMD_SET_USB_MODE   0x15
#define	CMD_GET_STATUS     0x22
#define	CMD_RD_USB_DATA0   0x27
#define	CMD_WR_USB_DATA    0x2C
#define	CMD_WR_REQ_DATA    0x2D
#define	CMD_WR_OFS_DATA    0x2E
#define	CMD_SET_FILE_NAME  0x2F
#define	CMD_DISK_CONNECT   0x30
#define	CMD_DISK_MOUNT     0x31
#define	CMD_FILE_OPEN      0x32
#define	CMD_FILE_ENUM_GO   0x33
#define	CMD_FILE_CREATE    0x34
#define	CMD_FILE_ERASE     0x35
#define	CMD_FILE_CLOSE     0x36
#define	CMD_DIR_INFO_READ  0x37
#define	CMD_DIR_INFO_SAVE  0x38
#define	CMD_BYTE_LOCATE    0x39
#define	CMD_BYTE_READ      0x3A
#define	CMD_BYTE_RD_GO     0x3B
#define	CMD_BYTE_WRITE     0x3C
#define	CMD_BYTE_WR_GO     0x3D
#define	CMD_DISK_CAPACITY  0x3E
#define	CMD_DISK_QUERY     0x3F
#define	CMD_DIR_CREATE     0x40
#define	CMD_SEC_LOCATE     0x4A
#define	CMD_SEC_READ       0x4B
#define	CMD_SEC_WRITE      0x4C
#define	CMD_DISK_BOC_CMD   0x50
#define	CMD_DISK_READ      0x54
#define	CMD_DISK_RD_GO     0x55
#define	CMD_DISK_WRITE     0x56
#define	CMD_DISK_WR_GO     0x57

#define CMD_RET_SUCCESS    0x51
#define CMD_RET_ABORT      0x5F

#define USB_INT_SUCCESS     0x14  // USB transaction or transfer operation was successful
#define USB_INT_CONNECT     0x15  // USB device connection event detected, may be a new connection or reconnect after disconnecting
#define USB_INT_DISCONNECT  0x16  // USB device disconnect event detected
#define USB_INT_BUF_OVER    0x17  // USB data transmission error or too much data buffer overflow
#define USB_INT_USB_READY   0x18  // USB device has been initialized (USB address assigned)
#define USB_INT_DISK_READ   0x1D  // USB memory request data read out
#define USB_INT_DISK_WRITE  0x1E  // USB memory request data write
#define USB_INT_DISK_ERR    0x1F  // USB memory operation failed


// FAT data area file directory information
typedef struct _FAT_DIR_INFO {
  UINT8 DIR_Name [11];      // 00H, file name, a total of 11 bytes, fill in the gaps
  UINT8 DIR_Attr;           // 0BH, file attributes, refer to the following description
  UINT8 DIR_NTRes;          // 0CH
  UINT8 DIR_CrtTimeTenth;   // 0DH, file creation time, in 0.1 second units
  UINT16 DIR_CrtTime;       // 0EH, file creation time
  UINT16 DIR_CrtDate;       // 10H, the date the file was created
  UINT16 DIR_LstAccDate;    // 12H, the date of the last access operation
  UINT16 DIR_FstClusHI;     // 14H
  UINT16 DIR_WrtTime;       // 16H, file modification time, refer to the previous macro MAKE_FILE_TIME
  UINT16 DIR_WrtDate;       // 18H, file modification date, refer to the previous macro MAKE_FILE_DATE
  UINT16 DIR_FstClusLO;     // 1AH
  UINT32 DIR_FileSize;      // 1CH, file length
} FAT_DIR_INFO, * P_FAT_DIR_INFO; // 20H

// File attributes
#define ATTR_READ_ONLY      0x01 // file is read-only attribute
#define ATTR_HIDDEN         0x02 // file is an implied attribute
#define ATTR_SYSTEM         0x04 // file as a system property
#define ATTR_VOLUME_ID      0x08 // label
#define ATTR_DIRECTORY      0x10 // subdirectory (folder)
#define ATTR_ARCHIVE        0x20 // file as archive attribute
#define ATTR_LONG_NAME      (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID) // Long File Name Properties
#define ATTR_LONG_NAME_MASK (ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE)

// File attributes UINT8
// bit0 bit1 bit2 bit3 bit4 bit5 bit6 bit7
// Only implicit system volume is not defined
// Reading Tibetan Standard Recording
// File time UINT16
// Time = (Hour << 11) + (Minute << 5) + (Second >> 1)
#define MAKE_FILE_TIME (h, m, s) ((h << 11) + (m << 5) + (s >> 1)) // Generate file time data of the specified hour, minute, second *
// File date UINT16
// Date = ((Year-1980) << 9) + (Month << 5) + Day
#define MAKE_FILE_DATE (y, m, d) (((y-1980) << 9) + (m << 5) + d) // Generate file date data for the specified year, month, and day *

#define LONE_NAME_MAX_CHAR (255 * 2) // Long file name Maximum number of characters / bytes
#define LONG_NAME_PER_DIR (13 * 2) // Long filenames Number of characters / bytes in each file directory info structure

void resetAll(void);
UINT8 checkExist(void);
UINT8 wiatForResponse(void);


#endif
