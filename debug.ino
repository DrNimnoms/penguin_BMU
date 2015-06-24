/*------------------------------------------------------------------------------
 * void debugInital(void)
 * initializes the debug parameters
 *----------------------------------------------------------------------------*/
 void debugInital(void){
   Serial.begin(9600);           // set up Serial library at 9600 bps
   Serial.println("\t\t Initalizing penguin BMU \n"); 
 }
