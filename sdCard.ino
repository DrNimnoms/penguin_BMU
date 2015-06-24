/*------------------------------------------------------------------------------
 * void SdInital(void)
 * initializes the pins and sets up serial communication related to the SD card
 *----------------------------------------------------------------------------*/
void SdInital(void){
  
//  pinMode(10, OUTPUT);
  if (!SD.begin(sdCardPin)) {
    if(uartPrint) Serial.println("SD card initialization failed!");
    return;
  }
  if(uartPrint) Serial.println("SD card initialization done.");
  

  
}

void getLimits(void){
  File myFile;
  int endNow = 0;
  //  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
//  myFile = SD.open("test.txt", FILE_WRITE);
//  
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//  
//  // re-open the file for reading:
  myFile = SD.open("limits.txt");
  if (myFile) {
    Serial.println("limits.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available() && endNow<16) {
        Serial.write(myFile.read());
        endNow ++;
    }
    // close the file:
    myFile.close();
  } 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  SPI.setClockDivider(CLOCK_DIVIDER);
  
}

