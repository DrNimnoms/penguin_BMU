/*------------------------------------------------------------------------------
 * penguin_bmu.h
 * Created by Nima Ghods
 * Data: 5/14/2015
 * setsup miscellaneous parameters for running BMU v6.X on penguin project
 *----------------------------------------------------------------------------*/
 
 // debug
 boolean uartPrint= true;

 
 //time constants
  #define ONEHOUR 36000000 //in milliseconds
  #define FIVEMINUTES 300000  // in milliseconds
  #define ONEMINUTE 60000  // in milliseconds
  #define TENSECONDS 10000 //in milliseconds
  #define FIVESECONDS 5000 //in milliseconds
  #define FIVEHZ 200   // in milliseconds
// timeres
  Metro looptimer = Metro(FIVEHZ);
  Metro self_test_timer = Metro(FIVESECONDS);
  
  
  // BME communication
  #define BME_CH1 3
  #define BME_CH2 2
  #define CLOCK_DIVIDER 100
  // BME channals
  bmes bmesCh1= bmes(BME_CH1);
  
  // BMU shield sensors and actators
  bmuShield bmuSA = bmuShield();
 
 
  // BMC Ethernet communication setup
  Metro bmcComTimer = Metro();
  long bmcComDt=0;
  boolean bmcComFalg=false;
  byte BMUNum=0;
  byte Add0=5;
  byte Add1=7;
  byte Add2=6;
  byte Add3=8;
  //byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
  byte mac[6] = { 0x90, 0xA2, 0xDA, 0x0E, 0xCE, 0x70 };
  byte ipadd[]={192,168,0, 153};
  // the router's gateway address:
  byte gateway[] = { 192, 168, 0, 1 };
  // the subnet:
  byte subnet[] = { 255, 255, 255, 0 };
  int port=40;
  EthernetServer server(port);
  // Initialize the Ethernet server library
  // with the IP address and port you want to use 
  
  // sd card pramerters
  byte sdCardPin = 4;
  
  
  // BMC mode request preameters
  byte key=0;
  byte keyIn;
  int modeTimeLeft=-1;
  Mode modeReq;
  boolean newMode=false;
  
  // safty preameters
  boolean shutdownTimerOn = false;
  Metro shutdownTimer = Metro(ONEMINUTE);
  
  const int bmeOnPriority1 = 0;
  const int bmeOnPriority2 = 0x05A0;
  const int bmeChgPriority1 = 0x0129;
  const int bmeChgPriority2 = 0x0616;
  const int bmeBalPriority1 = 0x27EB;
  const int bmeBalPriority2 = 0x1814;
  
  const int bmuOnPriority1 = 0X0080;
  const int bmuOnPriority2 = 0x0053;
  const int bmuChgPriority1 = 0x2517;
  const int bmuChgPriority2 = 0x0068;
  const int bmuBalPriority1 = 0x165B;
  const int bmuBalPriority2 = 0x0024;
