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


/*------------------------------------------------------------------------------
 * void storeIc(void)
 * get the falg priorities from the on board SD card
 *----------------------------------------------------------------------------*/
void storeIc(){
  File myFile;
  uint16_t intTempo[2];
    float fTempo[4];
  // Check to see if the file exists: 
  if (SD.exists("IC.txt")) {
    // delete the file:
    SD.remove("IC.txt");
    bmuSA.get_icInfo(&intTempo[0],&fTempo[0]);
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("IC.txt", FILE_WRITE);
  if (myFile) {
    myFile.print("cap = ");
    myFile.println(fTempo[0],4);
    myFile.print("totalDis = ");
    myFile.println(fTempo[1],4);
    myFile.print("avgDOD = ");
    myFile.println(fTempo[2],4);
    myFile.print("DOD = ");
    myFile.println(fTempo[3],4);
    myFile.print("cycleCount = ");
    myFile.println(intTempo[0]);
    myFile.print("chgCount = ");
    myFile.println(intTempo[1]);
    myFile.close();  
  }
  else{
    if(uartPrint) Serial.println("error opening IC.txt");
  }
  SPI.setClockDivider(CLOCK_DIVIDER);
}

/*------------------------------------------------------------------------------
 * void getIc(void)
 * get the battery initial conditions from the on board SD card
 *----------------------------------------------------------------------------*/
void getIc(){
  File myFile;
  String IcInfo="";
  String tempo="";
  int index1=0, index2=0;
  uint16_t intTempo[2];
  float fTempo[4];
  
  // get bmu limits
  myFile = SD.open("IC.txt");
  if (myFile) {
    if(uartPrint) Serial.println("Gitting Battery IC from IC.txt");
    //read all the file
    while (myFile.available()) {
      IcInfo += (char)myFile.read();
    }
    // close the file:
    myFile.close();
    
    if(uartPrint){
      Serial.println("Battery history: ");
      Serial.print(IcInfo);
    }

    for(int i=0;i<4;i++){
      index1 = IcInfo.indexOf("=");
      index2 = IcInfo.indexOf("\n");
      tempo = IcInfo.substring(index1+1, index2);
      fTempo[i] = tempo.toFloat();
      IcInfo = IcInfo.substring(index2+1);
    }
    for(int i=0;i<2;i++){
      index1 = IcInfo.indexOf("=");
      index2 = IcInfo.indexOf("\n");
      tempo = IcInfo.substring(index1+1, index2);
      intTempo[i] = tempo.toInt();
      IcInfo = IcInfo.substring(index2+1);
    }
    
    bmuSA.set_icInfo(&intTempo[0],&fTempo[0]);
    
  }
  else {
    // if the file didn't open, print an error:
    if(uartPrint)Serial.println("Battery has no history file ");
    SPI.setClockDivider(CLOCK_DIVIDER);
    bmesCh1.meas_act_bmes();
    // set bmu min, max, and sum
    bmuSA.set_bme_sum(bmesCh1.cal_sum_of_bmes());
    bmuSA.set_bme_min(bmesCh1.cal_min_of_bmes());
    bmuSA.set_bme_max(bmesCh1.cal_max_of_bmes());
    if(uartPrint) Serial.println(bmesCh1.cal_min_of_bmes());
    bmuSA.initalizeSoc();
  }
  
  SPI.setClockDivider(CLOCK_DIVIDER);
}

/*------------------------------------------------------------------------------
 * void getPriority(void)
 * get the falg priorities from the on board SD card
 *----------------------------------------------------------------------------*/
void getPriority(){
  File myFile;
  String priorityInfo="";
  char tempoChar[3];
  int index=0;

  
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
    
//    if(uartPrint){
//      Serial.print(" bmcPriority1: 0x");
//      for(int j=1;j<4;j++) Serial.print(bmcPriority1[j]);
//      Serial.print("\n bmuPriority1: 0x");
//      for(int j=1;j<4;j++) Serial.print(bmuPriority1[j],HEX);
//      Serial.print("\n bmuPriority2: 0x");
//      for(int j=1;j<4;j++) Serial.print(bmuPriority2[j],HEX);
//      Serial.print("\n bmePriority1: 0x");
//      for(int j=1;j<4;j++) Serial.print(bmePriority1[j],HEX);
//      Serial.print("\n bmePriority2: 0x");
//      for(int j=1;j<4;j++) Serial.print(bmePriority2[j],HEX);
//      Serial.println(" ");
//    }
  } 
  else {
    // if the file didn't open, print an error:
    if(uartPrint) Serial.println("error opening flag_priorities.txt");
  }
}

/*------------------------------------------------------------------------------
 * void getLimits(void)
 * get the falg limits from the on board SD card
 *----------------------------------------------------------------------------*/
void getLimits(void){
  File myFile;
  boolean endNow = false;
  String limitsString="";
  String tempo="";
  int index1=0, index2=0;
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
//      if(uartPrint)Serial.println(bmuLimits[i],3);
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
//      if(uartPrint)Serial.println(bmeLimits[i],3);
    }
    bmesCh1.set_limits(bmeLimits);
  } 
  else {
    // if the file didn't open, print an error:
    if(uartPrint)Serial.println("error opening bmeLimits.txt");
  }
}




