/*------------------------------------------------------------------------------
 * void BMCcomm(void)
 * Sends Battery information and receives commands over ethernet
 * data out: dt, SOC, total voltage, BME voltages, and BME temperatures
 * data in: OFF, ON, CHARGE, BALANCE, OVERRIDE
 *-----------------------------------------------------------------------------*/
void BMCcomm()
{  
  bmcComDt = bmcComTimer.elapsed();
  if(bmcComDt > TENSECONDS) bmcComFalg=true;
 // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    bmcComTimer.reset();
    bmcComFalg=false;
    byte data_out[116];
    getData(&data_out[0]);
    String command="";//initializ string for next read
    while (client.available()) {
      char c = client.read();
      if (command.length() < 500) {
        command += c; //store characters to string 
      }    
    }
    client.write(&data_out[0],116);
    client.write('\r');
    client.write('\n');
    client.write('\n');
    parseCommand(command);
//    if(uartPrint) {
//      Serial.print("bmc command: ");
//      Serial.print(command);
//    }
  }
}

/*------------------------------------------------------------------------------
 * void parseCommand()
 * parce the command from BMC
 *----------------------------------------------------------------------------*/
 void parseCommand(String cmdIn){
   int sVal=cmdIn.indexOf('_'); //find the end of the key
   String keyString=cmdIn.substring(0,sVal); //grab key from string
   keyIn=keyString.toInt();  
   
//    if(uartPrint) {
//      Serial.print("bmu key: ");
//      Serial.println(key);
//      Serial.print("bmu mode: ");
//      Serial.println(bmuSA.get_mode());
//      Serial.print("bmc key: ");
//      Serial.println(keyIn);
//    }
   
   if(key==keyIn){
     key= (key+1) & 0x0F;
     cmdIn = cmdIn.substring(sVal+1);

     if(cmdIn.indexOf("OVE") >=0){
      overrideOn=true;
       if(uartPrint) Serial.println("Request: Override");
     }   
     else if(cmdIn.indexOf("OFF") >=0){
       modeReq = SYS_OFF;
       newMode = true;
       if(uartPrint) Serial.println("Request: OFF");
     }
     else if(cmdIn.indexOf("ON") >=0){
       modeReq = SYS_ON;
       newMode = true;
       if(uartPrint) Serial.println("Request: ON");
     }
     else if(cmdIn.indexOf("CHG") >=0){
       modeReq = CHARGE;
       newMode = true;
       sVal=cmdIn.indexOf('_');
       float tempVal = parseVoltage(cmdIn.substring(sVal+1));
       bmuSA.set_chg2vol(tempVal);
       if(uartPrint){
         Serial.print("Request: CHARGE to");
         Serial.println(tempVal,3);
       }
     }
     else if(cmdIn.indexOf("BAL") >=0){
       modeReq = BALANCE;
       newMode = true;
       if(uartPrint) Serial.println("Request: BALANCE");
     }
     
   }
//   if(uartPrint) {
//     Serial.print("bmc reqMode: ");
//     Serial.println(modeReq);
//   }
   
 }
 
 /*------------------------------------------------------------------------------
 * void parseVoltage(String sVoltage)
 * parce a voltage out of the command from BMC
 *----------------------------------------------------------------------------*/
 float parseVoltage(String sVoltage){
   float volOut = sVoltage.toInt();
   return volOut/1000;
 }
 
/*------------------------------------------------------------------------------
 * void getData(&data_out[0])
 * get flag, dt, SOC, total voltage, current, cell voltage, temperaturs, ect.
 *----------------------------------------------------------------------------*/
void getData(byte *data_out){
  
  int idx=0;
  
  data_out[idx] = bmuSA.get_mode()<<6 | modeReq << 4 | key ; // bmu contactor info
  idx +=1;
//  if(uartPrint) Serial.println(modeReq);
  data_out[idx] = bmuSA.data_relay(); // bmu contactor info
  idx +=1;
  
  data_out[idx] = (byte)(modeTimeLeft & 0x00FF);
  data_out[idx+1] = (byte)(modeTimeLeft>>8 & 0x00FF);
  idx +=2;
  
  int dtTempo = (int) bmcComDt;
  data_out[idx] = (byte)(dtTempo & 0x00FF);
  data_out[idx+1] = (byte)(dtTempo>>8 & 0x00FF);
  idx +=2;
  
  // get BMU data
  bmuSA.data_bmu(&data_out[idx]);
  idx +=22;
  
  //get BME data

  //get voltage data
  bmesCh1.data_voltage(&data_out[idx]);
  idx += BME_NUM*VC_NUM*BYTE_INT;
  
  // get bme sum data
  bmesCh1.data_vtotal(&data_out[idx]);
  idx += BME_NUM*BYTE_INT;
  
  // get temerature data
  bmesCh1.data_temp(&data_out[idx]);
  idx += BME_NUM*TEMP_NUM*BYTE_INT;
  
  // get bme heat sink data
  bmesCh1.data_hst(&data_out[idx]);
  idx += BME_NUM*BYTE_INT;
   
  // get bme internal temperature data
  bmesCh1.data_itemp(&data_out[idx]);
  idx += BME_NUM*BYTE_INT;
  
  // get bme flag data
  bmesCh1.data_bme_flag(&data_out[idx]);
  idx += BME_NUM*BYTE_INT;
  
  // get bme flag override data
  bmesCh1.data_bme_flagOver(&data_out[idx]);
  idx += BME_NUM*BYTE_INT;

  
}


/*------------------------------------------------------------------------------
 * void bmcInital(void)
 * initializes the pins and sets up serial communication related to the BMC
 *----------------------------------------------------------------------------*/

void bmcInital(void){

  //************************ Ethernet setup *************************// 
  // start the Ethernet connection and the server:
  pinMode(Add0, INPUT);       // set pin to input
  digitalWrite(Add0, HIGH);  // turn on pullup resistors
  pinMode(Add1, INPUT);       // set pin to input
  digitalWrite(Add1, HIGH);  // turn on pullup resistors
  pinMode(Add2, INPUT);       // set pin to input
  digitalWrite(Add2, HIGH);  // turn on pullup resistors
  pinMode(Add3, INPUT);       // set pin to input
  digitalWrite(Add3, HIGH);  // turn on pullup resistors
  
  BMUNum = !digitalRead(Add0);
  BMUNum |= !digitalRead(Add1)<<1;
  BMUNum |= !digitalRead(Add2)<<2;
  BMUNum |= !digitalRead(Add3)<<3;
  ipadd[3]+=BMUNum;
  port = 40;
  mac[5] += BMUNum;
  
  if(uartPrint){
    Serial.print("ip_address: ");
    for(int i=0;i<3;i++){
      Serial.print(ipadd[i]);
      Serial.print(".");
    }
    Serial.println(ipadd[3]);
      
    Serial.print("mac_address: ");
    for(int i=0;i<5;i++){
      Serial.print(mac[i]);
      Serial.print(":");
    }
    Serial.println(mac[5]);
    Serial.print("port: ");
    Serial.println(port);
  }
  
  IPAddress ip(ipadd);
  server = EthernetServer(port);
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  delay(5); // waits 5 us for configuration
}
