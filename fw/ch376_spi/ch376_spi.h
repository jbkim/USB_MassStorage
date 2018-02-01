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


void resetAll(void);
UINT8 checkExist(void);
UINT8 wiatForResponse(void);


#endif
