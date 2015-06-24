/*------------------------------------------------------------------------------
 * void saftyCheck()
 * check the current mode for any errors with priority 1 or 2 
 * if priority 1 put system into OFF mode
 * if priority 2 start shutdown counter
 *----------------------------------------------------------------------------*/
void saftyCheck(){
  int tempoPriority= getPriority(bmuSA.get_mode());
  if(tempoPriority == 0){
    modeTimeLeft=-1;              // make sure time left in mode is truned off
    shutdownTimerOn = false;      // make sure shutdown timer is trund off
  }
  else if(tempoPriority == 1) turnSysOff();  // check if error is priority 1 
  else if(tempoPriority == 2 && !shutdownTimerOn){  // check if error is priority 2 and not already started shutdown timer
    shutdownTimerOn = true;
    shutdownTimer.reset();
  }
  else if(shutdownTimerOn){
    if(shutdownTimer.check()) turnSysOff();
    else modeTimeLeft = (ONEMINUTE - shutdownTimer.elapsed())/100;
  }
}

/*------------------------------------------------------------------------------
 * void overrideFlag(int device, int flagNum)
 * removes a flagNum on a given device 
 *----------------------------------------------------------------------------*/
 void overrideFlags(){
  overrideOn = false;
  Mode overMode=bmuSA.get_mode();  // get current mode
  modeTimeLeft=-1;              // make sure time left in mode is truned off
  shutdownTimerOn = false;      // make sure shutdown timer is trund off
  bmuSA.set_flag_over(bmuPriority2[overMode]);   // override bmu priority 2 flags
  bmesCh1.set_flag_over(bmePriority2[overMode]);  // override bme priority 1 flags
 }

/*------------------------------------------------------------------------------
 * void turnSysOff()
 * turns off contactors on bmu and balancing on bmes
 * makes sure priority 2 timer is truned off
 *----------------------------------------------------------------------------*/
void turnSysOff(){ 
  bmuSA.set_mode(SYS_OFF);  // go directly to off mode if requested
  bmesCh1.set_balancing(false); // make sure balancing is off
  modeTimeLeft=-1;              // make sure time left in mode is truned off
  shutdownTimerOn = false;      // make sure shutdown timer is trund off
}

/*------------------------------------------------------------------------------
 * void processReq(Mode modein)
 * process the request from the BMC. If no priority 1 for the requested mode then
 * put the bmu into the requested mode
 *----------------------------------------------------------------------------*/
void processReq(){
  newMode=false;
  bmuSA.reset_flags();
  bmesCh1.reset_flags();
  Mode currentMode = bmuSA.get_mode();
  
  if(currentMode != SYS_OFF){  // system can only go to off mode if not in off mode
    modeReq = SYS_OFF;  
    turnSysOff();
  }
  else if(modeReq != currentMode) 
  {
    byte reqPriority = getPriority(modeReq);  // get the priority of the requested mode
    if(reqPriority != 1){                    // check if the priority is not 1
      bmuSA.set_mode(modeReq);
      if(modeReq == BALANCE){
        bmesCh1.set_bal2vol(bmesCh1.cal_min_of_bmes());
        bmesCh1.set_balancing(true);
      }
    }
  }
}

/*------------------------------------------------------------------------------
 * byte getPriority(Mode modein)
 * given a mode, bmu and bme flags returns a priority
 *----------------------------------------------------------------------------*/
 byte getPriority(Mode modein){
   
   byte tempoPriority=0;
   
   // get bmu flags
   int bmuFlag = bmuSA.get_flag();
   
   // get all bme flags
   int bmeFlag = bmesCh1.cal_bme_flag();

   if(modein >= SYS_ON && modein <= BALANCE){
     if(((bmeFlag & bmePriority1[modein]) | (bmuFlag & bmuPriority1[modein])) !=0) tempoPriority=1;
     else if((bmeFlag & bmePriority2[modein]) | (bmuFlag & bmuPriority2[modein]) !=0) tempoPriority=2;
   }  
   
   return tempoPriority;
 }
