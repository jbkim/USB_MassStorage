/* ===============================================================
      Project: CH376S USB Read/Write Module testing ground
       Author: Scott C
      Created: 1st May 2015
  Arduino IDE: 1.6.2
      Website: http://arduinobasics.blogspot.com/p/arduino-basics-projects-page.html
  Description: This project will allow you to perform many of the functions available on the CH376S module.
               Checking connection to the module, putting the module into USB mode, resetting the module,
               reading, writing, appending text to files on the USB stick. This is very useful alternative to
               SD card modules, plus it doesn't need any libraries.
================================================================== */

#include <SPI.h>
#include "ch376_spi.h"

#define	CMD_RESET_ALL      0x05
#define	CMD_CHECK_EXIST    0x06

const int chipSelectPin = 10;
const int BzPin = 8;

const int IntPin = 2;

// int timeOut = 2000;   // Timeout 2 sec
int timeOut = 5000;   // Timeout 5 sec


// hardware rest of the CH376
void resetAll(void)
{
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(CMD_RESET_ALL);
  digitalWrite(chipSelectPin, HIGH);
  delay(50);
}

UINT8 waitForResponse(void)
{
  UINT8   res;
  int cnt = 0;

  // wait for the BzPin is Low
  while(1) {
     cnt++;
     if (digitalRead(BzPin) == LOW) break;
     if(cnt > timeOut) break;
     delay(1);
  }
  res = SPI.transfer(0x00);

  return(res);
}

// check the communication between CH376 and MCU
UINT8 checkExist(void)
{
  UINT8 res;

  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(CMD_CHECK_EXIST);
  SPI.transfer(0x65);
  res = waitForResponse();
  digitalWrite(chipSelectPin, HIGH);

  // Serial.println(res, HEX);

  if (res == (255 - 0x65)) return(TRUE);
  else return(FALSE);
}

UINT8 getStatus()
{
  UINT8 res;

  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(CMD_GET_STATUS);
  res = waitForResponse();
  digitalWrite(chipSelectPin, HIGH);

  return(res);
}

UINT8 diskConnectionStatus()
{
  UINT8 res;

  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(CMD_DISK_CONNECT);
  res = waitForResponse();
  digitalWrite(chipSelectPin, HIGH);

  Serial.println(res, HEX);

  if (res == USB_INT_SUCCESS) {
    Serial.println("Coonection to USB ok");
  }else {
    Serial.println("Coonection to USB FAIL");
  }

  return(res);
}

// set the USB mode
// 0x05: USB Host, non-generate SOF
// 0x06: USB Host, SOF auto
// 0x07: USB Host, reset USB bus
UINT8 setUSBmode(UINT8 code)
{
  UINT8 res;

  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(CMD_SET_USB_MODE);
  SPI.transfer(code);
  res = waitForResponse();
  digitalWrite(chipSelectPin, HIGH);

  if (res == CMD_RET_SUCCESS) {
    Serial.println("set USB mode");

    // return(1);
    // res = waitForResponse();

    // res = SPI.transfer(0x00);
    // if (res == USB_INT_CONNECT) {
    //   // USB is present
    //   Serial.println("USB is present");
    //   return(1);
    // }else {
    //   // USB not present
    //   Serial.println(res, HEX);
    //   Serial.println("USB is NOT present");
    //   return(0);
    // }
  }else {
    // commnad fail
    Serial.println(res, HEX);
  }
  delay(20);

  return(res);
}

//==============================================================================================================================================
void setup() {

  // Debug serial
  Serial.begin(115200);
  // while (!Serial) {
  //    ;
  //  }

  // setup chip select & SPI busu pin
  pinMode(chipSelectPin, OUTPUT);
  pinMode(BzPin, INPUT);
  pinMode(IntPin, INPUT);

  pinMode(IntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IntPin), intFunction, FALLING);
  // setup SPI
  // TODO : need to check the SPI speed
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  Serial.println("\r\nStart USB MSD");

  // 1. CH376 chip reset
  resetAll();
  delay(100);
  Serial.println("CH376 ResetAll");

  // 2. test communication interface
  if (checkExist())
    Serial.println("CH376 CheckExist Ok");
  else
    Serial.println("CH376 CheckExist Error");

  // 3. set USB mode
  setUSBmode(0x06);

  // 4.
  diskConnectionStatus();
}

void intFunction()
{
  Serial.println("CH376 Interrupt");
}
//================================================================================================================================================
void loop() {

//  delay(100);
//  CH376ResetAll();

#if 0
  if(Debug.available()){
    //read any incoming bytes from the Serial monitor, and store this byte in the variable called computerByte
    computerByte = Debug.read();

    if(computerByte==49){               //1
      // Reset the USB device
      printCommandHeader("ResetALL");
      resetALL();
    }

    if(computerByte==50){               //2
      printCommandHeader("COMMAND1: check ch376 is exists");
      checkConnection(0x01);
    }

    if(computerByte==51){               //3
     printCommandHeader("COMMAND2: set_USB_Mode");
      set_USB_Mode(0x06);
    }

    if(computerByte==52){               //4
      printCommandHeader("COMMAND8: Read small File: 2.TXT");
      readFile("2.TXT");
    }

    if(computerByte==53){               //5
      printCommandHeader("COMMAND5: Read File: TEST4.TXT");
      readFile("TEST4.TXT");                           // Read the contents of this file on the USB disk, and display contents in the Serial Monitor
    }

    // if(computerByte==54){               //6
    //   printCommandHeader("COMMAND6: Append data to file: TEST4.TXT");
    //   appendFile("TEST4.TXT", wrData2);                // Append data to the end of the file.
    // }

    // if(computerByte==55){               //7
    //   printCommandHeader("COMMAND7: Delete File: TEST4.TXT");
    //   fileDelete("TEST4.TXT");                         // Delete the file named TEST4.TXT
    // }

    if(computerByte==56){               //8
      printCommandHeader("COMMAND8: Read File: TEST2.TXT");
      readFile("2.TXT");                           // Read the contents of the TEST2.TXT file on the USB disk, and display contents in the Serial Monitor
    }


    if(computerByte==57){               //9
      printCommandHeader("COMMAND9: Read File: TEST3.TXT");
      readFile("1.TXT");                           // Read the contents of the TEST3.TXT file on the USB disk, and display contents in the Serial Monitor
    }
  }

  if(Serial.available()){                                 // This is here to capture any unexpected data transmitted by the CH376S module
    Debug.print("CH376S has just sent this code:");
    Debug.println(Serial.read(), HEX);
  }
#endif
}
