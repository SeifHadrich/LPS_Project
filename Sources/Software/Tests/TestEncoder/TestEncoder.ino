#define LOG
/*
///////////////////////////////////////////////////////
####              HARDWARE_CONNECTION              ####
///////////////////////////////////////////////////////
*/

#define ENCODER_A_PIN       3
#define ENCODER_B_PIN       4

/*
///////////////////////////////////////////////////////
####                  NEW DATA TYPE                ####
///////////////////////////////////////////////////////
*/
#define TYPE_TICKS   unsigned long   

typedef enum
  {
  UP,
  DOWN
  }TYPE_LINE_DIRECTION;
  
typedef struct
  {
  TYPE_TICKS          Ticks;
  TYPE_LINE_DIRECTION Direction;
  }STR_ENCODER;    

/*
///////////////////////////////////////////////////////
####        ENCODER_GOLOBAL VARIABLES              ####
///////////////////////////////////////////////////////
*/

STR_ENCODER G_Encoder;  
#ifdef LOG
char TmpMsg[100];//Debug msg
#endif

/*
///////////////////////////////////////////////////////
####              ENCODER_DRIVER                   ####
///////////////////////////////////////////////////////
*/

/*
 * Init_ENCODER_IO_ISR
 * Initialise input output and set interrups for encoder
 * IN  -> -
 * OUT <- -
 */
void Init_ENCODER_IO_ISR()
  {
  pinMode(ENCODER_A_PIN       ,INPUT_PULLUP);
  pinMode(ENCODER_B_PIN       ,INPUT_PULLUP);
  //Set ISR_Encoder interrupt to be triggered as soon as ENCODER_A_PIN raises
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), ISR_Encoder, FALLING  );
  }

/*
 * ISR_Encoder
 * An interrupt system routine triggered as soon as the encoder detects a new tick
 * IN  -> -
 * OUT <- -
 */
void ISR_Encoder ()
  {
  //Depond on the pulley's rotation the G_NbTicks will increase or decrease
  if (digitalRead(ENCODER_B_PIN))
    {
    G_Encoder.Ticks++;
    G_Encoder.Direction =UP;
    }
  else 
    {
    G_Encoder.Ticks--;
    G_Encoder.Direction =DOWN;
    }
  }

/*
Setup
*/
void setup() 
  {
  //Init_Data();
  Init_ENCODER_IO_ISR();
  #ifdef LOG
  //Set the serial comm at 115200 bauds 
  /**/Serial.begin(115200); 
  /**/sprintf(TmpMsg,"*** Starting Encoder prorgam ***");
  /**/Serial.println(TmpMsg);
  #endif
  }


/*
Infinty loop
*/
void loop() 
  {
  delay(1000);  
  sprintf(TmpMsg,"Encoder.Ticks=%lu",G_Encoder.Ticks);
  Serial.println(TmpMsg);
  /*
  Display line direction
  */
  if(G_Encoder.Direction==UP)Serial.println("UP");
  else Serial.println("DOWN");
  /*
  Display line position
  */
  }
