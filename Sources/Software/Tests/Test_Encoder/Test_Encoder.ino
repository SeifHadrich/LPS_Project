#define LOG

/*
///////////////////////////////////////////////////////
####              HARDWARE_CONNECTION              ####
///////////////////////////////////////////////////////
*/
/*
Connection: 
Green = A phase, white = B phase, red = power +, black = V0 
Note: AB two -phase outputs must not be connected directly to VCC, otherwise, they will burn the output transistor.
1 x 600P/R Incremental Rotary Encoder
*/

#define ENCODER_A_PIN       3
#define ENCODER_B_PIN       4

/*
///////////////////////////////////////////////////////
####              DATA STUCTURE                    ####
///////////////////////////////////////////////////////
*/
#define NB_TICKS_CM         200 //how many pulse (ticks) are generated for a 1 cm line deplacement
#define MAX_LINE_POSITION   20000L
#define MIN_LINE_POSITION   50

#define TYPE_TICKS            unsigned short   
#define TYPE_LINE_POSITION    unsigned short

typedef enum
  {
  UP,
  DOWN
  }TYPE_LINE_DIRECTION;
  
typedef struct
  {
  TYPE_TICKS                Ticks;
  TYPE_LINE_POSITION        LinePosition;
  TYPE_LINE_DIRECTION       Direction;
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
 * Init_ENCODER_DATA
 * Initialise global data used for the encoder
 * IN  -> -
 * OUT <- -
 */
void Init_ENCODER_DATA()
  {
  G_Encoder.Ticks=0;
  G_Encoder.LinePosition=(MAX_LINE_POSITION-MIN_LINE_POSITION)/2;
  //G_Encoder.Direction=0;//we don't need to init this value
  } 
  
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
  //Set ISR_Encoder interrupt to be triggered as soon as ENCODER_A_PIN fall
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
    /*
    * Direction is UP
    */ 
    //Ticks' board management
    if(G_Encoder.Ticks==NB_TICKS_CM)
      {
      //Reset Ticks counter
      G_Encoder.Ticks=0;
      //Increase LinePosition counter
      G_Encoder.LinePosition++;
      }  
    else
      {
      G_Encoder.Ticks++;
      }
    //Update Direction 
    G_Encoder.Direction =UP;
    }
  /*
   * Direction is DOWN
  */
  else 
    {
    //Ticks' board management  
    if(G_Encoder.Ticks==0)
      {
      //Reset Ticks counter
      G_Encoder.Ticks=NB_TICKS_CM;
      //Decrease roations counter
      G_Encoder.LinePosition--;
      }  
    else
      {
      G_Encoder.Ticks--;
      }  
    //Update Direction
    G_Encoder.Direction =DOWN;
    }
  }

/*
Setup
*/
void setup() 
  {
  Init_ENCODER_DATA();
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
  static TYPE_TICKS Ticks=0;
  /*
  Display line direction
  */
  if(G_Encoder.Ticks!=Ticks)
    {
    sprintf(TmpMsg,"Ticks=%d \tLinePosition = %d cm\tDirection = ",G_Encoder.Ticks,G_Encoder.LinePosition);
    Serial.print(TmpMsg);
    //Serial.println(G_Encoder.Ticks,HEX);
    //Serial.println(G_Encoder.LinePosition,HEX);
    if(G_Encoder.Direction==UP)Serial.println("UP");
    else Serial.println("DOWN");
    Ticks=G_Encoder.Ticks;
    delay(1000);
    }
  /*
  Display line position
  */
  }
