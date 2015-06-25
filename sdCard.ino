/*------------------------------------------------------------------------------
 * void SdInital(void)
 * initializes the pins and sets up serial communication related to the SD card
 *----------------------------------------------------------------------------*/
void SdInital(void){
  
//  pinMode(10, OUTPUT);
  if (!SD.begin(sdCardPin)) {
    if(uartPrint) Serial.println("SD card initialization failed!");
    sdCartIn = false;
    return;
  }
  sdCartIn = true;
  if(uartPrint) Serial.println("SD card initialization done.");
}

void getPriority(){
  File myFile;
  String priorityInfo="";
  char tempoChar[3];
  long index=0;

  
  // get bmu limits
  myFile = SD.open("flag.csv");
  if (myFile) {
    if(uartPrint) Serial.println("Gitting priority from flag.csv");

    //read all the file
    while (myFile.available()) {
      priorityInfo += (char)myFile.read();
    }
    // close the file:
    myFile.close();


    index = priorityInfo.indexOf("\r");
    priorityInfo = priorityInfo.substring(index+1);
    for(int i=0;i<29;i++){
      index = priorityInfo.indexOf(",");
      for(int j=0;j<3;j++) tempoChar[j]=priorityInfo.charAt(index+1+2*j);

      if (i==0) {
        for(int j=0;j<3;j++) {
          if(tempoChar[j] == '1') bmcPriority1[j+1]=true;
          else if(tempoChar[j] == '0') bmcPriority1[j+1]=false;
        }
      }
      else if(i < 16){
        for(int j=0;j<3;j++) {
          if(tempoChar[j] == '2') bmuPriority2[j+1] |= (1<<(i-1));
          else if(tempoChar[j] == '1') bmuPriority1[j+1] |= (1<<(i-1));
        }
      }
      else{
        for(int j=0;j<3;j++) {
          if(tempoChar[j] == '2') bmePriority2[j+1] |= (1<<(i-16));
          else if(tempoChar[j] == '1') bmePriority1[j+1] |= (1<<(i-16));
        }
      }
      priorityInfo = priorityInfo.substring(index+7);
    }
    
    if(uartPrint){
      Serial.print(" bmcPriority1: 0x");
      for(int j=1;j<4;j++) Serial.print(bmcPriority1[j]);
      Serial.print("\n bmuPriority1: 0x");
      for(int j=1;j<4;j++) Serial.print(bmuPriority1[j],HEX);
      Serial.print("\n bmuPriority2: 0x");
      for(int j=1;j<4;j++) Serial.print(bmuPriority2[j],HEX);
      Serial.print("\n bmePriority1: 0x");
      for(int j=1;j<4;j++) Serial.print(bmePriority1[j],HEX);
      Serial.print("\n bmePriority2: 0x");
      for(int j=1;j<4;j++) Serial.print(bmePriority2[j],HEX);
      Serial.println(" ");
    }
  } 
  else {
    // if the file didn't open, print an error:
    if(uartPrint) Serial.println("error opening flag_priorities.txt");
  }

  
}

void getLimits(void){
  File myFile;
  boolean endNow = false;
  String limitsString="";
  String tempo="";
  int index1=0, index2;
  float bmeLimits[13]={0};
  float bmuLimits[14]={0};

  // get bmu limits
  myFile = SD.open("bmuLim.txt");
  if (myFile) {
    if(uartPrint) Serial.println("Gitting bmu limits from bmuLim.txt");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      limitsString += (char)myFile.read();
    }
    // close the file:
    myFile.close();

    
    for(int i=0;i<14;i++){
      index1 = limitsString.indexOf("=");
      index2 = limitsString.indexOf(";");
      tempo = limitsString.substring(index1+1, index2);
      bmuLimits[i] = tempo.toFloat();
      index1 = limitsString.indexOf("\n");
      limitsString = limitsString.substring(index1+1);
      if(uartPrint)Serial.println(bmuLimits[i],3);
    }
    bmuSA.set_limits(bmuLimits);
  } 
  else {
    // if the file didn't open, print an error:
    if(uartPrint) Serial.println("error opening bmuLimits.txt");
  }

  // get bme limits
  myFile = SD.open("bmeLim.txt");
  limitsString="";
  if (myFile) {
    if(uartPrint) Serial.println("Gitting limits from bmeLim.txt");
     // read from the file until there's nothing else in it:
    while (myFile.available()) {
      limitsString += (char)myFile.read();
    }
    // close the file:
    myFile.close();

    
    for(int i=0;i<13;i++){
      index1 = limitsString.indexOf("=");
      index2 = limitsString.indexOf(";");
      tempo = limitsString.substring(index1+1, index2);
      bmeLimits[i] = tempo.toFloat();
      index1 = limitsString.indexOf("\n");
      limitsString = limitsString.substring(index1+1);
      if(uartPrint)Serial.println(bmeLimits[i],3);
    }
    bmesCh1.set_limits(bmeLimits);
  } 
  else {
    // if the file didn't open, print an error:
    if(uartPrint)Serial.println("error opening bmeLimits.txt");
  }
  
  SPI.setClockDivider(CLOCK_DIVIDER);
}




