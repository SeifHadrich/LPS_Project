#define LOG
#ifdef LOG
char TmpMsg[100];//Debug msg
#endif
/*
  #############################################################################################################################################################
  #############################################################################################################################################################
                                                                          TEST_ENCODER
  #############################################################################################################################################################
  #############################################################################################################################################################
*/

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
} TYPE_LINE_DIRECTION;

typedef struct
{
  TYPE_TICKS                Ticks;
  TYPE_LINE_POSITION        LinePosition;
  TYPE_LINE_DIRECTION       Direction;
} STR_ENCODER;

/*
  ///////////////////////////////////////////////////////
  ####        ENCODER_GOLOBAL VARIABLES              ####
  ///////////////////////////////////////////////////////
*/

STR_ENCODER G_Encoder;

/*
  ///////////////////////////////////////////////////////
  ####              ENCODER_DRIVER                   ####
  ///////////////////////////////////////////////////////
*/
/*
   Init_ENCODER_DATA
   Initialise global data used for the encoder
   IN  -> -
   OUT <- -
*/
void Init_ENCODER_DATA()
{
  G_Encoder.Ticks = 0;
  G_Encoder.LinePosition = (MAX_LINE_POSITION - MIN_LINE_POSITION) / 2;
  //G_Encoder.Direction=0;//we don't need to init this value
}

/*
   Init_ENCODER_IO_ISR
   Initialise input output and set interrups for encoder
   IN  -> -
   OUT <- -
*/
void Init_ENCODER_IO_ISR()
{
  pinMode(ENCODER_A_PIN       , INPUT_PULLUP);
  pinMode(ENCODER_B_PIN       , INPUT_PULLUP);
  //Set ISR_Encoder interrupt to be triggered as soon as ENCODER_A_PIN fall
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), ISR_Encoder, FALLING  );
}

/*
   ISR_Encoder
   An interrupt system routine triggered as soon as the encoder detects a new tick
   IN  -> -
   OUT <- -
*/
void ISR_Encoder ()
{
  //Depond on the pulley's rotation the G_NbTicks will increase or decrease
  if (digitalRead(ENCODER_B_PIN))
  {
    /*
      Direction is UP
    */
    //Ticks' board management
    if (G_Encoder.Ticks == NB_TICKS_CM)
    {
      //Reset Ticks counter
      G_Encoder.Ticks = 0;
      //Increase LinePosition counter
      G_Encoder.LinePosition++;
    }
    else
    {
      G_Encoder.Ticks++;
    }
    //Update Direction
    G_Encoder.Direction = UP;
  }
  /*
     Direction is DOWN
  */
  else
  {
    //Ticks' board management
    if (G_Encoder.Ticks == 0)
    {
      //Reset Ticks counter
      G_Encoder.Ticks = NB_TICKS_CM;
      //Decrease roations counter
      G_Encoder.LinePosition--;
    }
    else
    {
      G_Encoder.Ticks--;
    }
    //Update Direction
    G_Encoder.Direction = DOWN;
  }
}
/*
  #############################################################################################################################################################
  #############################################################################################################################################################
                                                                          TEST_IR_SENSOR_SATCK
  #############################################################################################################################################################
  #############################################################################################################################################################
*/
/*
  This QueeArray lib was modified by me
  You can research for sha in the QueueArray.h file to find ou what I have done
*/
#include <QueueArray.h>
/*
  ///////////////////////////////////////////////////////
  ####              HARDWARE_CONNECTION              ####
  ///////////////////////////////////////////////////////
*/

#define IR_SENSOR_SIG_PIN   2

/*
  ///////////////////////////////////////////////////////
  ####              DATA STUCTURE                    ####
  ///////////////////////////////////////////////////////
*/

#define MAX_BLACK_BARS            4        //nb max of black bars that syteme can handle
#define MAX_BARS                  8        //nb max of total bars (black and white)

#define SMALL_BAR_LENGHT          4        //4cm
#define BIG_BAR_LENGHT            8        //8cm
#define ERROR_MARGIN              1        //1cm
#define END_WHITE_BAR_LENGHT      30       //30 cm of white distance between 2 bars  

#define TYPE_BAR_LENGHT    byte

typedef enum
{
  BLACK,
  WHITE
} TYPE_LINE_COLOR;

typedef struct
{
  TYPE_LINE_COLOR Color;
  STR_ENCODER     Encoder;
} STR_LANDMARK_STACK;

typedef struct
{
  QueueArray <STR_LANDMARK_STACK>   Stack;
  TYPE_LINE_COLOR                   Color[MAX_BARS];
  STR_ENCODER                       Encoder[MAX_BARS];
  TYPE_LINE_DIRECTION               ReadDirection;
  byte                              Index;
} STR_LANDMARK;

/*
  ///////////////////////////////////////////////////////
  ####        ENCODER_GOLOBAL VARIABLES              ####
  ///////////////////////////////////////////////////////
*/

STR_LANDMARK  G_Landmark;

/*
  ///////////////////////////////////////////////////////
  ####                ENCODER_LIB                    ####
  ///////////////////////////////////////////////////////
*/

/*
   GetBarLenght
   Compute the lenght of a bar with a precision of 1 cm (if the lenght is 1<cm => BarLenght =0 )
   IN  -> LastEncoder, PreEncoder
   OUT <- BarLenght
*/
TYPE_BAR_LENGHT GetBarLenght(STR_ENCODER LastEncoder , STR_ENCODER PreEncoder)
{
  TYPE_BAR_LENGHT BarLenght;
  /*
    If it has the same position in cm we don't care about ticks and we consider the lenght is null
  */
  if (LastEncoder.LinePosition == PreEncoder.LinePosition)
  {
    BarLenght = 0;
  }
  //we just care about the distance we don't care about the sign
  else
  {
    BarLenght = abs(LastEncoder.LinePosition - PreEncoder.LinePosition);
  }
  return BarLenght;
}


/*
   GetLineDirection
   Compute the lenght of a bar with a precision of 1 cm (if the lenght is 1<cm => BarLenght =0 )
   IN  -> LastEncoder, PreEncoder
   OUT <- BarLenght
*/
TYPE_LINE_DIRECTION GetLineDirection(STR_ENCODER LastEncoder , STR_ENCODER PreEncoder)
{
  TYPE_LINE_DIRECTION LineDirection;
  //If it's the same position in cm we compare the ticks number
  if (LastEncoder.LinePosition == PreEncoder.LinePosition)
  {
    if (LastEncoder.Ticks >= PreEncoder.Ticks)LineDirection = UP;
    else LineDirection = DOWN;
    //Log error
    if (LastEncoder.Ticks == PreEncoder.Ticks)
      /**/Serial.println("Warning we detect a same distance for a a different landmark");
  }
  //Direction UP
  else if (LastEncoder.LinePosition > PreEncoder.LinePosition)
  {
    LineDirection = UP;
  }
  //irection DOWN
  else LineDirection = DOWN;
  return LineDirection;
}

/*
  ///////////////////////////////////////////////////////
  ####            IR_SENSOR_DRIVER                   ####
  ///////////////////////////////////////////////////////
*/

/*
   Init_IR_SENSOR_IO_ISR
   Initialise input output and set interrups for Ir sensor
   IN  -> -
   OUT <- -
*/
void Init_IR_SENSOR_IO_ISR()
{
  pinMode(IR_SENSOR_SIG_PIN, INPUT_PULLUP);
  //Set ISR_IrSensor interrupt to be triggered as soon as  the IrSensor's signal changes
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_SIG_PIN), ISR_IrSensor, CHANGE );
}

/*
   ISR_IrSensor
   An interrupt system routine triggered as soon as the IR sensor detects a new color
   IN  -> -
   OUT <- -
*/
void ISR_IrSensor ()
{
  /*
    todo: Check why the interrup trigged sometimes twice with the same color
  */
  static STR_LANDMARK_STACK Data;
  static TYPE_LINE_COLOR    LastColor;
  /*
    Check that stack is not full
  */
  //Get the landmark's color
  if (digitalRead(IR_SENSOR_SIG_PIN))Data.Color = BLACK;
  else Data.Color = WHITE;
  //Delete the measurement noise
  if (Data.Color != LastColor)
  {
    //Get the current ticks number
    Data.Encoder.Ticks = G_Encoder.Ticks;
    Data.Encoder.LinePosition = G_Encoder.LinePosition;
    /*
      As i can not call the SaveNewLandmark function because the isr should be short and fast
      I decided to push this data in a stack
    */
    //Push the color and the ticks
    G_Landmark.Stack.push(Data);
    //Update the last color
    LastColor = Data.Color;
  }
}

/*
   SaveNewLandmark
   Read and save a line landmark (it looks like a barcode)
   IN  -> Color   : Bar color (could be black or white)
          NbTicks : The position of the bar
   OUT <- -
*/
void SaveNewLandmark(byte Color, byte NbTicks)
{
  byte Index = G_Landmark.Index;
  TYPE_BAR_LENGHT BarLenght ;
  //Check if the system allows read a new landmark
  /*
    The Start of a new Landmark
  */
  if (Index == 0 )
  {
    //This look like a valid landmark
    if (Color == BLACK)
    {
      G_Landmark.Color[Index] = Color;
      G_Landmark.Encoder[Index] = G_Encoder;
      Index = 1;
    }
    //Something is wrong with this landmark
    else
    {
      //Delete the current landmark
      Index = 0;
      //Log error
      Serial.println("ERROR Landmark = 1 (It could be the first landmark readen by the system) "); //error log
    }
    //return;
  }
  /*
    Whithin a landmark
    Index > 0
  */
  else
  {
    //Get the lenght of one bar of landmark
    BarLenght = GetBarLenght(G_Encoder, G_Landmark.Encoder[Index - 1]);
    //Check if this lenght is coherent
    if (BarLenght > SMALL_BAR_LENGHT && BarLenght < BIG_BAR_LENGHT)
    {
      /*
        When a first bar was read
        we compute the direction of the line
      */
      if (Index == 1)
      {
        G_Landmark.ReadDirection = GetLineDirection(G_Encoder, G_Landmark.Encoder[Index - 1]);
      }
      /*
        Check that the line didn't change direction
      */
      else if (GetLineDirection(G_Encoder, G_Landmark.Encoder[Index - 1]) == G_Landmark.ReadDirection)
      {
        //it looks like a valid landmark
        G_Landmark.Color[Index] = Color;
        G_Landmark.Encoder[Index] = G_Encoder;
        Index++;
      }
      //if the line changes direction while reading a landmark
      /*
        The current system does not yet support line direction change while reading a landmark
        ToDo !!!
      */
      else
      {
        //Delete the current landmark
        Index = 0;
        //Log error
        Serial.println("ERROR Landmark = 2 (readDirection) ");
      }
    }
    //strange landmark lenght
    //it  could be because of line direction change or maybe it's just a dust in the line
    else
    {
      //delete the current landmark
      Index = 0;
      //log error
      Serial.println("ERROR Landmark = 3 (bar lenght)");
    }
    /*
      If somothing went wrong
      I delete the saved landmark and because it
      could be the start of a new good landmark
      do I decide to save it
    */
    if (Index == 0 && Color == BLACK)
    {
      G_Landmark.Color[0] = Color;
      G_Landmark.Encoder[0] = G_Encoder;
      Index = 1;
    }
  }
  //Update the Landmark Index
  G_Landmark.Index = Index;
}












/*
   ISR_IrSensor
   An interrupt system routine triggered as soon as the IR sensor detects a new color
   IN  -> -
   OUT <- -
*/
void SimLandmark()
{
  static STR_LANDMARK_STACK Data;
  /*
    Check that stack is not full
  */


  //Get the landmark's color
  if (digitalRead(IR_SENSOR_SIG_PIN))Data.Color = BLACK;
  else Data.Color = WHITE;
  //Get the current ticks number
  Data.Encoder = G_Encoder;
  /*
    As i can not call the SaveNewLandmark function because the isr should be short and fast
    I decided to push this data in a stack
  */
  //Push the color and the ticks
  G_Landmark.Stack.push(Data);
#ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  /**/Serial.flush();
#endif
#ifdef LOG
  G_Encoder.Ticks++;
#endif
}










/*
  Setup
*/
void setup()
{
  G_Encoder.Ticks=50;
  G_Encoder.LinePosition=2;
  //Init_IR_SENSOR_IO_ISR();
  //Set printer debbug for the Stack
  G_Landmark.Stack.setPrinter (Serial);
#ifdef LOG
  //Set the serial comm at 115200 bauds
  /**/Serial.begin(115200);
  /**/sprintf(TmpMsg, "*** Starting Landmark Reader prorgam ***");
  /**/Serial.println(TmpMsg);
#endif
}

/*
  Infinty loop
*/
void loop()
  {
  for(byte ii=0;ii<5;ii++)SimLandmark();
  delay(1000);
  STR_LANDMARK_STACK Stack;
  if (!G_Landmark.Stack.isEmpty())
    {
    while (!G_Landmark.Stack.isEmpty())
      {
      Stack = G_Landmark.Stack.pop();
      /**/sprintf(TmpMsg, "Color=%d \t| LinePosition=%hu \t| Ticks=%d\n", Stack.Color, Stack.Encoder.LinePosition, Stack.Encoder.Ticks);
      /**/Serial.print(TmpMsg);
      }
    /**/Serial.println("*********\n\n\n*********");
    }
}
