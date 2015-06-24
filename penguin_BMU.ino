/*------------------------------------------------------------------------------
 * penguin_bmu.ino
 * Created by Nima Ghods
 * Data: 5/1/2015
 * made to run on the penguin battery system
 *----------------------------------------------------------------------------*/

//included headers
//#include <Arduino.h>
#include <stdint.h>
#include <Metro.h>    // Include Metro library for keeping time intervals
#include <SPI.h>      // used for communication
#include <Ethernet.h>      //use Ethernet to communicate to BMC, included in arduino environment
#include <SD.h>
#include "bmuShield_5.h"
#include "BME3CV6_2.h"
//#include "penguin_safety.h"
#include "penguin_bmu.h"


void setup() {
  
  if(uartPrint) debugInital();
  delay(100);
  
  // initialize sd card and get limits, priorities, and battery state from files on sd card
  SdInital();
  getLimits();
  // initialize the bmu shield
  bmuSA.bmuShield_initialize();
  
  //initilize BME spi communications
  bme_com_initialize(CLOCK_DIVIDER,SPI_MODE0);
  //set BME adc for both channals
  bmesCh1.set_adc(MD_NORMAL,DCP_DISABLED,CELL_CH_ALL,AUX_CH_ALL,STAT_CH_ALL);
    
  // initialize BMC communication
  bmcInital();
  
  getLimits();

  SPI.setClockDivider(CLOCK_DIVIDER);
//  SPI.setBitOrder(MSBFIRST);
//  bme_com_initialize(CLOCK_DIVIDER,SPI_MODE0);
}

void loop() {
  
  if (looptimer.check()) { // check if the metro has passed its interval 
    // measure, actuate, set flags for BMEs
    bmesCh1.meas_act_bmes();
    
    // meausre the sensors on bmu shield
    bmuSA.meas_bmuShield();
    
    // set bmu flags
    bmuSA.set_bme_sum(bmesCh1.cal_sum_of_bmes());
    bmuSA.set_bme_min(bmesCh1.cal_min_of_bmes());
    bmuSA.set_bme_max(bmesCh1.cal_max_of_bmes());
    bmuSA.set_flags();
    
//    Serial.println(bmesCh1.cal_sum_of_bmes());
//    if(uartPrint){
//      int tempo= bmuSA.get_flag();
//      Serial.print(bmuSA.get_pressure());
//      Serial.print(", ");
//      Serial.println(bmuSA.get_presRate());
//      if ( (tempo>>7 & 1)==1) Serial.println(bmuSA.get_current());
//    }
    
    // communicate with BMC. get command and send current data
    BMCcomm();   
//    if(uartPrint) Serial.println(bmuSA.get_current());
    // process request if key matches and command is not override
    if(newMode) processReq();
    
    // priority check and shutdown or start shutdown timer if needed
    saftyCheck();
    
    // actuate relays on bmu shield
    bmuSA.act_bmuShield();
  }
  
  if(self_test_timer.check()){
    // conduct BME self tests
//    bmesCh1.bme_self_test(); 
  }  

}