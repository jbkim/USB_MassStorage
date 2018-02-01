
#include <SoftwareSerial.h>
#include "ch376.h"

byte computerByte;
byte USB_Byte;

#ifdef INT_ENABLE
const int IntPin = 2;
#endif

int timeOut = 2000;
UINT8 buf[64];

SoftwareSerial USB(10, 11);



//============================================================================
void setup()
{
  Serial.begin(115200);
  while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
   }

   // Setup serial communication with the CH376S module (using the default baud rate of 9600)
  USB.begin(9600);

#ifdef INT_ENABLE
  pinMode(IntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IntPin), intFunction, FALLING);
#endif

  // reset the module
  resetALL();
  delay(50);

  // 2. test communication interface
  checkConnection(0xa5);

  // 3. set USB mode
  set_USB_Mode(0x06);

  Serial.println("\r\nStart USB MSD");
}

//============================================================================
void loop()
{
  if(Serial.available()){
    computerByte = Serial.read();

    // Reset the USB device
    if(computerByte == '1'){               //1
      printCommandHeader("COMMAND3: resetALL");
      resetALL();
    }
    // Check for successful connection and communication with the CH376S module.
    if(computerByte == '2'){               //2
      printCommandHeader("COMMAND1: CHECK CONNECTION");
      checkConnection(0x01);
    }
    if(computerByte == '3'){               //3
     printCommandHeader("COMMAND2: set_USB_Mode");
      set_USB_Mode(0x06);
    }

    if(computerByte == '4'){               //4
      // printCommandHeader("COMMAND5: Read File: abc.TXT");
      // readFile("abc.TXT\0"); // not read
      printCommandHeader("COMMAND5: Read File: ABCD.TXT");
      // set_USB_Mode(0x06);
      readFile("/ABCD.TXT\0");
    }

    // read file under the folder. /G/3.TXT
    if(computerByte == '5'){               //5
      printCommandHeader("read /G/3.TXT");
      setFileName("/G\0");fileOpen();
      readFile("3.TXT\0");
    }
    // enumerate folder
    if(computerByte == '6'){               //5
      printCommandHeader("Enumerate folder");
      setFileName("*\0");fileOpen();
      // readFile("3.TXT");
    }

  }

  // Capture any unexpected data transmitted by the CH376S module
  if(USB.available()){
    Serial.print("CH376S has just sent this code:");
    Serial.println(USB.read(), HEX);
  }

}
//============================================================================

#ifdef INT_ENABLE
void intFunction()
{
  Serial.print("CH376 Interrupt - ");
  Serial.println(USB.read(), HEX);
}
#endif

//print Command header
void printCommandHeader(String header)
{
   Serial.println("======================");
   Serial.println("");
   Serial.println(header);
   Serial.println("----------------------");
}


// Check for successful communication with the CH376S.
void checkConnection(byte value)
{
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_CHECK_EXIST);
  USB.write(value);

  if(waitForResponse("checking connection")){       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
    if(getResponseFromUSB() == (255 - value)){
       Serial.println(">Connection to CH376S was successful.");
    } else {
      Serial.print(">Connection to CH376S - FAILED.");
    }
  }
}


// Make sure that the USB is inserted.
void set_USB_Mode (byte value)
{
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_SET_USB_MODE);
  USB.write(value);

  delay(20);
#if 1
  if(USB.available()){
    USB_Byte = USB.read();
    //Check to see if the command has been successfully transmitted and acknowledged.
    if(USB_Byte == CMD_RET_SUCCESS){                                   // If true - the CH376S has acknowledged the command.
        Serial.println("set_USB_Mode command acknowledged"); //The CH376S will now check and monitor the USB port
        USB_Byte = USB.read();

        //Check to see if the USB stick is connected or not.
        if(USB_Byte == USB_INT_CONNECT){                               // If true - there is a USB stick connected
          Serial.println("USB is present");
        } else {
          Serial.print("USB Not present. Error code:");   // If the USB is not connected - it should return an Error code = FFH
          Serial.print(USB_Byte, HEX);
          Serial.println("H");
        }

    } else {
        Serial.print("CH3765 error!   Error code:");
        Serial.print(USB_Byte, HEX);
        Serial.println("H");
    }
  }
  delay(20);
#endif
}

// Hardware reset of the CH376S
void resetALL(){
    USB.write(0x57);
    USB.write(0xAB);
    USB.write(CMD_RESET_ALL);
    Serial.println("The CH376S module has been reset !");
    delay(200);
}

//
void readFile(String fileName)
{
  // set_USB_Mode(0x06);

  diskConnectionStatus();
  USBdiskMount();
  setFileName(fileName);
  fileOpen();
  int fs = getFileSize();
  fileRead();
  fileClose(0x00);
}


//Check the disk connection status
void diskConnectionStatus()
{
  Serial.println("Checking USB disk connection status");
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_DISK_CONNECT);

  if(waitForResponse("Connecting to USB disk")){
    if(getResponseFromUSB() == USB_INT_SUCCESS){
       Serial.println(">Connection to USB OK");
    } else {
      Serial.print(">Connection to USB - FAILED.");
    }
  }
}


//initialise the USB disk and check that it is ready
void USBdiskMount(){
  Serial.println("Mounting USB disk");
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_DISK_MOUNT);

  if(waitForResponse("mounting USB disk")){
    if(getResponseFromUSB() == USB_INT_SUCCESS){
       Serial.println(">USB Mounted - OK");
    } else {
      Serial.print(">Failed to Mount USB disk.");
    }
  }
}

UINT8 ReadBlock( PUINT8 buf )
{
  UINT8 s, l;

  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_RD_USB_DATA0);

  if(waitForResponse("Read Block")){
    s = l = getResponseFromUSB();
  }

  // Serial.print("> Read Block - ");Serial.println(l);

  if ( l ) {
    do {
      if(waitForResponse("Read Block")){
        *buf = getResponseFromUSB();
      }
      buf ++;
    } while ( -- l );
  }
  
  return( s );
}


//opens the file for reading or writing
void fileOpen()
{
  byte ret = 0;
  P_FAT_DIR_INFO  pDir;
  UINT8 s;

  Serial.println("Opening file.");
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_FILE_OPEN);
  if(waitForResponse("file Open")){

#if 0
    if(getResponseFromUSB() == USB_INT_SUCCESS){
       Serial.println(">File opened successfully.");
    } else {
      Serial.print(">Failed to open file.");
    }
#else
    ret = getResponseFromUSB();
    if(ret == USB_INT_SUCCESS)
    {
       Serial.println("> File opened successfully.");
    } else if (ret == USB_INT_DISK_READ){
      Serial.println("> need to read more");

      // s = FileOpenPath(buf);
      while(1) {
        ReadBlock(buf);
        pDir = (P_FAT_DIR_INFO)buf;  
        if ( pDir->DIR_Name[0] != '.' ) {  
          if ( pDir->DIR_Name[0] == 0x05 ) pDir->DIR_Name[0] = 0xE5;
          pDir->DIR_Attr = 0;  
          for (int i = 0; i < 11; i++) {
            Serial.print(pDir->DIR_Name[i], HEX); Serial.print(' ');
          }
        }
        Serial.println(' ');

        USB.write(0x57);
        USB.write(0xAB);
        USB.write(CMD_FILE_ENUM_GO);
        if (waitForResponse(" read * ")){
          ret = getResponseFromUSB();
          if (ret != USB_INT_DISK_READ) {
            Serial.println("> Exit");
            break;
          }
        }
      }

      // USB.write(0x57);
      // USB.write(0xAB);
      // USB.write(CMD_RD_USB_DATA0);
      // if(waitForResponse("reading data")){
      //     Serial.print(USB.read(), HEX);Serial.print(" ");
      //     while(USB.available()){
      //       Serial.print(USB.read(), HEX);Serial.print(" ");
      //       delay(1);
      //     }
      // }


    } else {
      Serial.print(">Failed to open file.");
      Serial.println(ret, HEX);
    }
#endif

  }
}


//
boolean setByteRead(byte numBytes)
{
  boolean bytesToRead=false;
  int timeCounter = 0;

  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_BYTE_READ);
  USB.write((byte)numBytes);   //tells the CH376S how many bytes to read at a time
  USB.write((byte)0x00);

  if(waitForResponse("setByteRead")){
    if(getResponseFromUSB() == USB_INT_DISK_READ){
      bytesToRead=true;
    }
  }
  return(bytesToRead);
}

// writes the file size to the serial Monitor.
int getFileSize()
{
  int fileSize=0;

  Serial.println("Getting File Size");
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_GET_FILE_SIZE);
  USB.write(0x68);
  delay(100);

  Serial.print("FileSize =");
  if(USB.available()){
    fileSize = fileSize + USB.read();
  }
  if(USB.available()){
    fileSize = fileSize + (USB.read()*255);
  }
  if(USB.available()){
    fileSize = fileSize + (USB.read()*255*255);
  }
  if(USB.available()){
    fileSize = fileSize + (USB.read()*255*255*255);
  }
  Serial.println(fileSize);
  delay(10);
  return(fileSize);
}


//read the contents of the file
void fileRead()
{
  Serial.println("Reading file:");
  byte firstByte = 0x00;
  byte numBytes = 64; // maximum

  while(setByteRead(numBytes)){
    USB.write(0x57);
    USB.write(0xAB);
    USB.write(CMD_RD_USB_DATA0);
    if(waitForResponse("reading data")){
        firstByte = USB.read();
        while(USB.available()){
          Serial.write(USB.read());
          delay(1);
        }
    }
    if(!continueRead()){
      break;
    }
  }
  Serial.println();
  Serial.println("NO MORE DATA");
}

//continue to read the file
// TODO : check this working well
boolean continueRead()
{
  boolean readAgain = false;

  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_BYTE_RD_GO);

  if(waitForResponse("continueRead")){
     if(getResponseFromUSB() == USB_INT_SUCCESS){
       readAgain = true;
     }
  }
  return(readAgain);
}


//is used to set the file pointer position. true for beginning of file, false for the end of the file.
void filePointer(boolean fileBeginning)
{
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_BYTE_LOCATE);

  if(fileBeginning){
    USB.write((byte)0x00);             //beginning of file
    USB.write((byte)0x00);
    USB.write((byte)0x00);
    USB.write((byte)0x00);
  } else {
    USB.write((byte)0xFF);             //end of file
    USB.write((byte)0xFF);
    USB.write((byte)0xFF);
    USB.write((byte)0xFF);
  }
  if(waitForResponse("setting file pointer")){
     if(getResponseFromUSB() == USB_INT_SUCCESS){
       Serial.println("Pointer successfully applied");
     }
  }
}

//closes the file
void fileClose(byte closeCmd)
{
  Serial.println("Closing file:");
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_FILE_CLOSE);
  USB.write((byte)closeCmd);

  if(waitForResponse("closing file")){
     byte resp = getResponseFromUSB();
     if(resp == USB_INT_SUCCESS){
       Serial.println(">File closed successfully.");
     } else {
       Serial.print(">Failed to close file. Error code:");
       Serial.println(resp, HEX);
     }
  }
}

//This sets the name of the file to work with
void setFileName(String fileName)
{
  Serial.print("Setting filename to:");
  Serial.println(fileName);
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(CMD_SET_FILE_NAME);
  USB.print(fileName);     // "fileName" is a variable that holds the name of the file.  eg. TEST.TXT
  USB.write((byte)0x00);   // null byte indicates the end of the file name.
  delay(20);
}

//
boolean waitForResponse(String errorMsg){
  boolean bytesAvailable = true;
  int counter = 0;

  while(!USB.available()){     //wait for CH376S to verify command
    delay(1);
    counter++;
    if(counter > timeOut){
      Serial.print("TimeOut waiting for response: Error while: ");
      Serial.println(errorMsg);
      bytesAvailable = false;
      break;
    }
  }
  delay(1);
  return(bytesAvailable);
}

//
byte getResponseFromUSB(){
  byte response = byte(0x00);
  if (USB.available()){
    response = USB.read();
  }
  return(response);
}


#if 0
//writeFile========================================================================================
//is used to create a new file and then write data to that file. "fileName" is a variable used to hold the name of the file (e.g TEST.TXT). "data" should not be greater than 255 bytes long.
void writeFile(String fileName, String data){
  // resetALL();                     //Reset the module
  // set_USB_Mode(0x06);             //Set to USB Mode
  diskConnectionStatus();         //Check that communication with the USB device is possible
  USBdiskMount();                 //Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
  setFileName(fileName);          //Set File name
  if(fileCreate()){               //Try to create a new file. If file creation is successful
    fileWrite(data);              //write data to the file.
  } else {
    Serial.println("File could not be created, or it already exists");
  }
  fileClose(0x01);
}

//appendFile()====================================================================================
//is used to write data to the end of the file, without erasing the contents of the file.
void appendFile(String fileName, String data){
    // resetALL();                     //Reset the module
    // set_USB_Mode(0x06);             //Set to USB Mode
    diskConnectionStatus();         //Check that communication with the USB device is possible
    USBdiskMount();                 //Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
    setFileName(fileName);          //Set File name
    fileOpen();                     //Open the file
    filePointer(false);             //filePointer(false) is to set the pointer at the end of the file.  filePointer(true) will set the pointer to the beginning.
    fileWrite(data);                //Write data to the end of the file
    fileClose(0x01);                //Close the file using 0x01 - which means to update the size of the file on close.
}


//fileWrite=======================================================================================
//are the commands used to write to the file
void fileWrite(String data){
  Serial.println("Writing to file:");
  byte dataLength = (byte) data.length();         // This variable holds the length of the data to be written (in bytes)
  Serial.println(data);
  Serial.print("Data Length:");
  Serial.println(dataLength);
  delay(100);
  // This set of commands tells the CH376S module how many bytes to expect from the Arduino.  (defined by the "dataLength" variable)
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(0x3C);
  USB.write((byte) dataLength);
  USB.write((byte) 0x00);
  if(waitForResponse("setting data Length")){      // Wait for an acknowledgement from the CH376S module before trying to send data to it
    if(getResponseFromUSB()==0x1E){                // 0x1E indicates that the USB device is in write mode.
      USB.write(0x57);
      USB.write(0xAB);
      USB.write(0x2D);
      USB.print(data);                             // write the data to the file

      if(waitForResponse("writing data to file")){   // wait for an acknowledgement from the CH376S module
      }
      Serial.print("Write code (normally FF and 14): ");
      Serial.print(USB.read(),HEX);                // code is normally 0xFF
      Serial.print(",");
      USB.write(0x57);
      USB.write(0xAB);
      USB.write(0x3D);                             // This is used to update the file size. Not sure if this is necessary for successful writing.
      if(waitForResponse("updating file size")){   // wait for an acknowledgement from the CH376S module
      }
      Serial.println(USB.read(),HEX);              //code is normally 0x14
    }
  }
}

//fileCreate()========================================================================================
//the command sequence to create a file
boolean fileCreate(){
  boolean createdFile = false;
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(0x34);
  if(waitForResponse("creating file")){       //wait for a response from the CH376S. If file has been created successfully, it will return true.
     if(getResponseFromUSB()==0x14){          //CH376S will send 0x14 if this command was successful
       createdFile=true;
     }
  }
  return(createdFile);
}


//fileDelete()========================================================================================
//the command sequence to delete a file
void fileDelete(String fileName){
  setFileName(fileName);
  delay(20);
  USB.write(0x57);
  USB.write(0xAB);
  USB.write(0x35);
  if(waitForResponse("deleting file")){       //wait for a response from the CH376S. If file has been created successfully, it will return true.
     if(getResponseFromUSB()==0x14){          //CH376S will send 0x14 if this command was successful
       Serial.println("Successfully deleted file");
     }
  }
}

#endif
