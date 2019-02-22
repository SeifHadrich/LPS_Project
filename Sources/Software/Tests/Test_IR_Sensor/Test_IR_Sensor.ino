#define LOG
#ifdef LOG
char TmpMsg[100];//Debug msg
#endif

/*
///////////////////////////////////////////////////////
####              HARDWARE_CONNECTION              ####
///////////////////////////////////////////////////////
*/

#define IR_SENSOR_SIG_PIN   2

/*
///////////////////////////////////////////////////////
####                  NEW DATA TYPE                ####
///////////////////////////////////////////////////////
*/

typedef enum
  {
  BLACK,
  WHITE
  }TYPE_LINE_COLOR;
  
/*
///////////////////////////////////////////////////////
####        ENCODER_GOLOBAL VARIABLES              ####
///////////////////////////////////////////////////////
*/


/*
///////////////////////////////////////////////////////
####            IR_SENSOR_DRIVER                   ####
///////////////////////////////////////////////////////
*/
/*
 * Init_IR_SENSOR_IO_ISR
 * Initialise input output and set interrups for Ir sensor
 * IN  -> -
 * OUT <- -
 */
void Init_IR_SENSOR_IO_ISR()
  {
  pinMode(IR_SENSOR_SIG_PIN,INPUT_PULLUP);
  //Set ISR_IrSensor interrupt to be triggered as soon as  the IrSensor's signal changes
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_SIG_PIN), ISR_IrSensor, CHANGE );
  }

/*
 * ISR_IrSensor
 * An interrupt system routine triggered as soon as the IR sensor detects a new color
 * IN  -> -
 * OUT <- -
 */
void ISR_IrSensor()
  {
  //Get the landmark's color  
  if(digitalRead(IR_SENSOR_SIG_PIN))Serial.println("Black");
  else Serial.println("White");
  }

/*
Setup
*/
void setup()
  {
  Init_IR_SENSOR_IO_ISR();
  #ifdef LOG
  //Set the serial comm at 115200 bauds 
  /**/Serial.begin(115200); 
  /**/sprintf(TmpMsg,"*** Starting IR Sensor prorgam ***");
  /**/Serial.println(TmpMsg);
  #endif
  }

/*
Infinty loop
*/
void loop() 
  {
  }
