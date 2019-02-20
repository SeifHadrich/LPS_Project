#define LOG
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

#define MAX_BLACK_BARS        4        //nb max of black bars that syteme can handle
#define MAX_BARS              8        //nb max of total bars (black and white)

#define BAR_LENGHT            ?
#define ERROR_MARGIN          ?        // = 0.25 * BAR_LENGHT
#define END_LENGHT_LANDMARK   ?        // =  3   * BAR_LENGHT  
#define MIN_LENGHT_LANDMARK   ?        // = 0.75 * BAR_LENGHT
#define MAX_LENGHT_LANDMARK   ?        // = 1.25 * BAR_LENGHT

#define TYPE_TICKS   unsigned long

typedef enum
{
  UP,
  DOWN
} TYPE_LINE_DIRECTION;

typedef struct
{
  TYPE_TICKS          Ticks;
  TYPE_LINE_DIRECTION Direction;
} STR_ENCODER;

typedef enum
{
  BLACK,
  WHITE
} TYPE_LINE_COLOR;

typedef struct
{
  TYPE_LINE_COLOR Color;
  TYPE_TICKS      Ticks;
} STR_LANDMARK_STACK;

typedef struct
{
  QueueArray <STR_LANDMARK_STACK>   Stack;
  TYPE_LINE_COLOR                   Color[MAX_BARS];
  TYPE_TICKS                        Ticks[MAX_BARS];
  TYPE_LINE_DIRECTION               Direction;
  byte                              Index;
} STR_LANDMARK;

/*
  ///////////////////////////////////////////////////////
  ####        ENCODER_GOLOBAL VARIABLES              ####
  ///////////////////////////////////////////////////////
*/

#ifdef LOG
char TmpMsg[100];//Debug msg
#endif

STR_LANDMARK  G_Landmark;
STR_ENCODER   G_Encoder;
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
  static STR_LANDMARK_STACK Data;
  /*
    Check that stack is not full
  */

  //if(G_Landmark.Stack.isFull())
  if (0)
  {
#ifdef LOG
    /**/sprintf(TmpMsg, " ##### System error Stack.count=%d \n", G_Landmark.Stack.count());
    /**/Serial.print(TmpMsg);
#endif
    //Empty the landmark stack
    while (!G_Landmark.Stack.isEmpty())G_Landmark.Stack.pop();
#ifdef LOG
    /**/sprintf(TmpMsg, " ##### Stack has been emptied");
    /**/Serial.println(TmpMsg);
#endif
  }
  //Get the landmark's color
  if (digitalRead(IR_SENSOR_SIG_PIN))Data.Color = BLACK;
  else Data.Color = WHITE;
  //Get the current ticks number
  Data.Ticks = G_Encoder.Ticks;
  /*
    As i can not call the SaveNewLandmark function because the isr should be short and fast
    I decided to push this data in a stack
  */
  //Push the color and the ticks
  G_Landmark.Stack.push(Data);
#ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and ticks = %lu", Data.Color, Data.Ticks);
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
  byte ii = 0;
  //max stack memory is 164 * 5 byte
  for (ii = 0; ii < 200; ii++) {
    ISR_IrSensor();
  }
  delay(1000);
  STR_LANDMARK_STACK Stack;
  /**/Serial.println(" \n\n\n\n\n");
  while (!G_Landmark.Stack.isEmpty())
  {
    Stack = G_Landmark.Stack.pop();
    /**/sprintf(TmpMsg, "GET = G_Landmark.Stack.Color = %d G_Landmark.Stack.Ticks = %lu", Stack.Color, Stack.Ticks);
    /**/Serial.println(TmpMsg);
  }
  /**/Serial.println(" \n\n\n\n\n");
}
