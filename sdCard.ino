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
  boolean endNow = false;
  String limitsString=""
  myFile = SD.open("limits.txt");
  if (myFile) {
    Serial.println("Gitting limits from limits.txt");
    
    // read from the file until there's nothing else in it:
    while (myFile.available() && !endNow) {
        limitsString += myFile.read();
        if(limitsString.indexOf("//") >=0) endNow=true;
    }
    // close the file:
    myFile.close();
  } 
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening Limits.txt");
  }

  SPI.setClockDivider(CLOCK_DIVIDER);
  
}

