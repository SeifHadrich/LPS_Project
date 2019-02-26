#define LOG
#ifdef  LOG
#define LOG_RESULT
#define LOG_ERROR
#define LOG_DEBUG
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
#define NB_TICKS_CM         200 //how many pulses (ticks) are generated for a 1 cm line deplacement
#define MAX_LINE_POSITION   20000L
#define MIN_LINE_POSITION   50

#define TYPE_TICKS          unsigned short
#define TYPE_LINE_POSITION  unsigned short


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
#define MIN_BARS                  2        //nb main of total bars (black and white)

#define SMALL_BAR_LENGTH          4        //4cm
#define BIG_BAR_LENGTH            8        //8cm
#define MARGIN_ERROR              1        //1cm
#define END_WHITE_BAR_LENGTH      30       //30 cm of white distance between 2 bars  

#define TYPE_BAR_LENGTH    unsigned short

#define BAD_LINE_POSITION 0
#define ABSOLUTE_0_POSITION     20          //20cm the LANDMARK_ABSOLUTE_0 must to be higher than BAD_LINE_POSITION
#define ABSOLUTE_1_POSITION     10000       //10000cm = 100m the LANDMARK_ABSOLUTE_0 must to be higher than BAD_LINE_POSITION
#define ABSOLUTE_2_POSITION     20000       //20000cm = 200m the LANDMARK_ABSOLUTE_0 must to be higher than BAD_LINE_POSITION


  

typedef enum
{
  BLACK,
  WHITE
} TYPE_LINE_COLOR;

typedef enum
  {
  LANDMARK_UNKNOWN,
  LANDMARK_INCREMENTAL,
  LANDMARK_ABSOLUTE_0,
  LANDMARK_ABSOLUTE_1,
  LANDMARK_ABSOLUTE_2,
  }TYPE_LANDMARK_ID;

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
  GetBarLength
  Compute the length of a bar with a precision of 1 cm (if the length is 1<cm => BarLength =0 )
  IN  -> LastEncoder, PreEncoder
  OUT <- BarLength
*/
TYPE_BAR_LENGTH GetBarLength(STR_ENCODER LastEncoder , STR_ENCODER PreEncoder)
{
  TYPE_BAR_LENGTH BarLength;
  /*
    If it has the same position in cm we don't care about ticks and we consider the length is null
  */
  if (LastEncoder.LinePosition == PreEncoder.LinePosition)
  {
    BarLength = 0;
  }
  //we just care about the distance we don't care about the sign
  else
  {
    BarLength = LastEncoder.LinePosition > PreEncoder.LinePosition ?
                LastEncoder.LinePosition - PreEncoder.LinePosition :
                PreEncoder.LinePosition - LastEncoder.LinePosition;
  }
  return BarLength;
}


/*
   GetLineDirection
   Compute the length of a bar with a precision of 1 cm (if the length is 1<cm => BarLength =0 )
   IN  -> LastEncoder, PreEncoder
   OUT <- BarLength
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
  //Direction DOWN
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
  SaveNewBar
  Save one bar of the landmark
  IN  -> Color   : Bar color (could be black or white)
        NbTicks : Bar position
  OUT <- -
*/
void SaveNewBar(byte Color, STR_ENCODER Encoder)
{
  byte Index = G_Landmark.Index;
  TYPE_BAR_LENGTH BarLength ;
  //Check if the system allows read a new landmark
  /*
    The Start of a new Landmark
  */
  if (Index == 0 )
  {
    //This looks like a valid landmark
    if (Color == BLACK)
    {
      G_Landmark.Color[Index]   = Color;
      G_Landmark.Encoder[Index] = Encoder;
#ifdef LOG_DEBUG
      /**/sprintf(TmpMsg, "Index = %d BarColor %d BarPosition =%d ", Index, G_Landmark.Color[Index], G_Landmark.Encoder[Index].LinePosition );
      /**/Serial.println(TmpMsg);
#endif
      Index = 1;
    }
    //Something is wrong with this landmark
    else
    {
      //Delete the current landmark
      Index = 0;
      //Log error
      Serial.println(">>>>ERROR Landmark = 1 (It could be the first landmark readen by the system) "); //error log
    }
    //return;
  }
  /*
    Whithin a landmark
    Index > 0
  */
  else
  {
    //Get the length of one bar of landmark
    BarLength = GetBarLength(Encoder, G_Landmark.Encoder[Index - 1]);
#ifdef LOG
    /**/sprintf(TmpMsg, "\tBarLength = %d ", BarLength );
    /**/Serial.println(TmpMsg);
#endif
    //Check if this length is coherent
    if ((BarLength >= SMALL_BAR_LENGTH - MARGIN_ERROR ) && (BarLength <= SMALL_BAR_LENGTH + MARGIN_ERROR) ||
        (BarLength >= BIG_BAR_LENGTH - MARGIN_ERROR   ) && (BarLength <= BIG_BAR_LENGTH + MARGIN_ERROR))
    {
#ifdef LOG
      /**/Serial.println("\tBarLength is OK");
#endif
      /*
        When a first bar was read
        we compute the direction of the line
      */
      if (Index == 1)
      {
        G_Landmark.ReadDirection = GetLineDirection(Encoder, G_Landmark.Encoder[Index - 1]);
#ifdef LOG
        /**/sprintf(TmpMsg, "\tFirst LineDirection = %d", G_Landmark.ReadDirection);
        /**/Serial.println(TmpMsg);
#endif
      }
      /*
        Check that the line didn't change direction
        This should be always true because the length was approved
      */
      if (GetLineDirection(Encoder, G_Landmark.Encoder[Index - 1]) == G_Landmark.ReadDirection)
      {
#ifdef LOG
        /**/Serial.println("\tLineDirection is Ok");
#endif
        //it looks like a valid landmark
        G_Landmark.Color[Index] = Color;
        G_Landmark.Encoder[Index] = Encoder;
#ifdef LOG
        /**/sprintf(TmpMsg, "Index = %d BarColor %d BarPosition =%d ", Index, G_Landmark.Color[Index], G_Landmark.Encoder[Index].LinePosition );
        /**/Serial.println(TmpMsg);
        #endif
        Index++;
      }
      //if the line changes direction while reading a landmark
      /*
        The current system does not yet support line direction change while reading a landmark
        This is really a strange error case
        ToDo !!!
      */
      else
        {
        //Delete the current landmark
        Index = 0;
        //Log error
        Serial.println(">>>>ERROR Landmark = 2 (readDirection) ");
        }
    }
    //strange landmark length
    //it  could be because of line direction change or maybe it's just a dust in the line
    else
      {
      //delete the current landmark
      Index = 0;
      //log error
      Serial.println(">>>>ERROR Landmark = 3 (bar length is not correct )");
      }
    /*
      If somothing went wrong
      I delete the saved landmark and because it
      could be the start of a new good landmark
      do I decide to save it
    */
    if (Index == 0 && Color == BLACK)
      {
      G_Landmark.Color[0]   = Color;
      G_Landmark.Encoder[0] = Encoder;
      #ifdef LOG
      /**/sprintf(TmpMsg, "Correction : Index = %d BarColor %d BarPosition =%d ", Index, G_Landmark.Color[Index], G_Landmark.Encoder[Index].LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      Index = 1;
      }
  }
  //Update the Landmark Index
  G_Landmark.Index = Index;
}




#define INCREMENTAL_DISTANCE  10000 //100m
#define MAX_DIFF_CORRECTION   50//50cm  

/*
   CorrectLinePosition
   Correct the line position
   IN  -> LandmarkID, LandmarkPosition;
   OUT <- CorrectLinePosition
*/
TYPE_LINE_POSITION G_LinePosition ;

void CorrectLinePosition(byte LandmarkID, TYPE_LINE_POSITION LandmarkPosition)
  {
  TYPE_LINE_POSITION Diff  ;
  static TYPE_LINE_POSITION  PreIncrePosition = 0;
  switch (LandmarkID)
    {
    case LANDMARK_ABSOLUTE_0 :
      G_LinePosition = G_LinePosition + (ABSOLUTE_0_POSITION - LandmarkPosition);
      PreIncrePosition = LandmarkPosition;
      #ifdef LOG_RESULT
      /**/sprintf(TmpMsg, "LANDMARK_ABSOLUTE_0 : LandmarkPosition = %hu,\tABSOLUTE_0_POSITION=%hu,\tG_LinePosition=%hu\t", LandmarkPosition,
                  ABSOLUTE_0_POSITION, G_LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      break;
    case LANDMARK_ABSOLUTE_1 :
      G_LinePosition = G_LinePosition + (ABSOLUTE_1_POSITION - LandmarkPosition);
      PreIncrePosition = LandmarkPosition;
      #ifdef LOG_RESULT
      /**/sprintf(TmpMsg, "LANDMARK_ABSOLUTE_1 : LandmarkPosition = %hu,\tABSOLUTE_0_POSITION=%hu,\tG_LinePosition=%hu", LandmarkPosition,
                  LANDMARK_ABSOLUTE_1, G_LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      break;
    case LANDMARK_ABSOLUTE_2 :
      G_LinePosition = G_LinePosition + (ABSOLUTE_2_POSITION - LandmarkPosition);
      PreIncrePosition = LandmarkPosition;
      #ifdef LOG_RESULT
      /**/sprintf(TmpMsg, "LANDMARK_ABSOLUTE_2 : LandmarkPosition = %hu,\tABSOLUTE_0_POSITION=%hu,\tG_LinePosition=%hu",
                  LandmarkPosition, LANDMARK_ABSOLUTE_2, G_LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      break;
    case LANDMARK_INCREMENTAL :
      if (G_LinePosition > BAD_LINE_POSITION)
        {
        Diff = (PreIncrePosition + INCREMENTAL_DISTANCE) > LandmarkPosition ?
             (PreIncrePosition + INCREMENTAL_DISTANCE) - LandmarkPosition   :
             LandmarkPosition - (PreIncrePosition + INCREMENTAL_DISTANCE);
        //An acceptable difference
        if (Diff < MAX_DIFF_CORRECTION)
          {
          #ifdef LOG_RESULT
          /**/sprintf(TmpMsg, "Acceptable Diff : Diff = %hu,\t PreIncrePosition+INCREMENTAL_DISTANCE=%hu,\t LandmarkPosition=%hu",
                      Diff, PreIncrePosition + INCREMENTAL_DISTANCE, LandmarkPosition);
          /**/Serial.println(TmpMsg);
          G_LinePosition = G_LinePosition + (ABSOLUTE_2_POSITION - LandmarkPosition);
          PreIncrePosition = LandmarkPosition;
          #endif
          }
        //There is a big difference between a current line position and what it has to be !
        else
          {
          G_LinePosition = BAD_LINE_POSITION;
          #ifdef LOG_ERROR
          /**/sprintf(TmpMsg, "Big Diff : Diff = %hu,\t PreIncrePosition+INCREMENTAL_DISTANCE=%hu,\t LandmarkPosition=%hu",
                      Diff, PreIncrePosition + INCREMENTAL_DISTANCE, LandmarkPosition);
          /**/Serial.println(TmpMsg);
          /**/Serial.println("=> BAD_LINE_POSITION");
          #endif
          }
      }
    else
      {
      #ifdef LOG_RESULT
      /**/Serial.println("Cannot Inctrement it's a bad position");
      #endif  
      }
    break;
  }
}


/*
InitLinePosition
Initialise line position
IN  -> -
OUT <- -
*/
void InitLinePosition()
  {
  G_LinePosition = 0; //This is not a valid line position
  //The sytem has to detect an absolute landmark to correct the line position
  }
  
/*
GetLandmarkId
Check if a new landmark has is available if so it sends the landmark's ID
IN  -> this function will use globlal variables : NbTicks and IrSensor
OUT <- the ID of the landmark : 0 if any landmark is available
               > 0 the land mark ID
*/

byte GetLandmarkId(TYPE_LINE_POSITION *LandmarkPosition)
  {
  byte ii, NbBars;
  byte Index = G_Landmark.Index;
  byte BlackBarSize[MAX_BLACK_BARS];
  byte MyLandmarkPosition[MAX_BARS];
  volatile byte LandmarkCorruption;
  //Check if Index is even and is not null
  if (Index >= MIN_BARS)
    {
    #ifdef LOG_RESULT
    /**/sprintf(TmpMsg, "GetLandmarkId\n************\nIndex =%d", Index );
    /**/Serial.println(TmpMsg);
    #endif
    //Chek if the distance from the last bar is big enough to suppose that the landmark is over
    if (GetBarLength(G_Encoder, G_Landmark.Encoder[Index - 1]) >= END_WHITE_BAR_LENGTH)
      {
      //Delete the copeid data
      G_Landmark.Index = 0;
      #ifdef LOG_RESULT
      /**/sprintf(TmpMsg, "GetBarLength= %d \n", GetBarLength(G_Encoder, G_Landmark.Encoder[Index - 1]));
      /**/Serial.print(TmpMsg);
      #endif
      //Check if the last color was white
      if (G_Landmark.Color[Index - 1] == WHITE)
        {
        #ifdef LOG_RESULT
        /**/sprintf(TmpMsg, "It looks like a new landmark \n" );
        /**/Serial.print(TmpMsg);
        #endif
        /*
        Copy the IrSensor.Landmark
        */
        //When line is going up the data must be copied starting by the head
        for (ii = 0; ii < Index; ii++)
          {
          //When line is going UP first item first
          if (G_Landmark.ReadDirection == UP)MyLandmarkPosition[ii] = G_Landmark.Encoder[ii].LinePosition;
          //When line is going DOWn the data must be copied starting by the tail
          else MyLandmarkPosition[ii] = G_Landmark.Encoder[Index - ii - 1].LinePosition;
          }
        #ifdef LOG_RESULT
        for (ii = 0; ii < Index; ii++)
          {
          /**/sprintf(TmpMsg, "Position[%d]=%d\n" , ii, MyLandmarkPosition[ii]);
          /**/Serial.print(TmpMsg);
          }
        #endif
        /*
          Get the black bars sizes and  numbers
        */
        NbBars = 0;
        for (ii = 0; ii < Index; ii += 2)
          {
          BlackBarSize[NbBars] = MyLandmarkPosition[ii + 1] - MyLandmarkPosition[ii];
          #ifdef LOG_RESULT
          /**/sprintf(TmpMsg, "BlackBarSize[%d] =%d \n" , NbBars, BlackBarSize[NbBars]);
          /**/Serial.print(TmpMsg);
          #endif
          NbBars++;
          }
        /*
          Check all bars have a good length
        */
        LandmarkCorruption == false;
        //Check if the first black bar has a coherent length
        if (BlackBarSize[0] <= BIG_BAR_LENGTH + MARGIN_ERROR && BlackBarSize[0] >= BIG_BAR_LENGTH - MARGIN_ERROR)
          {
          ii = 1;
          //Check if the rest of black bar lenghth
          while (ii < NbBars && BlackBarSize[ii] <= SMALL_BAR_LENGTH + MARGIN_ERROR && BlackBarSize[ii] >= SMALL_BAR_LENGTH - MARGIN_ERROR )ii++;
          }
        //All black bars have a correct length
        if (ii == NbBars)
          {
          #ifdef LOG_RESULT
          /**/sprintf(TmpMsg, "A new land mark has been found LandmarkID =%d Position=%hu \n", NbBars, MyLandmarkPosition[0]);
          /**/Serial.print(TmpMsg);
          #endif
          return NbBars;
          }
        //At least one black bar doesn't have a correct length
        else
          {
          #ifdef LOG_RESULT
          /**/Serial.print("Any valid landmark has been found");
          /**/Serial.flush();
          #endif
          return 0;
          }
        }
      }
    }
  }





/*
SimLandmark
A simulater of ISr_Encoder and ISR_IrSensor
IN  -> -
OUT <- -
*/
void SimLandmark()
  {
  static STR_LANDMARK_STACK Data;
  /*
  Landmark_ID=1(Absolut 100 cm) DOWN
  */
  Data.Color = BLACK;
  Data.Encoder.LinePosition = 144;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color = WHITE;
  Data.Encoder.LinePosition = 140;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color = BLACK;
  Data.Encoder.LinePosition = 136;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color = WHITE;
  Data.Encoder.LinePosition = 128;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  G_Encoder.LinePosition = Data.Encoder.LinePosition - 30;
  }




/*
Setup
*/
void setup()
  {
  //Set printer debbug for the Stack
  G_Landmark.Stack.setPrinter (Serial);
  #ifdef LOG
  //Set the serial comm at 115200 bauds
  /**/Serial.begin(115200);
  /**/sprintf(TmpMsg, "*** Starting Landmark Reader prorgam ***\n****************************************");
  /**/Serial.println(TmpMsg);
  #endif
  }



/*
Infinty loop
*/
void loop()
  {
  SimLandmark();
  STR_LANDMARK_STACK  Stack;
  TYPE_LINE_POSITION  LandmarkPosition;
  TYPE_LANDMARK_ID    LandmarkId;
  if (!G_Landmark.Stack.isEmpty())
    {
    #ifdef LOG_DEBUG  
    /**/Serial.println("************\n\n\n************");
    #endif
    while (!G_Landmark.Stack.isEmpty())
      {
      Stack = G_Landmark.Stack.pop();
      #ifdef LOG_DEBUG
      /**/sprintf(TmpMsg, "\n----pull Color=%d \t| LinePosition=%hu \t\n", Stack.Color, Stack.Encoder.LinePosition);
      /**/Serial.print(TmpMsg);
      #endif
      SaveNewBar(Stack.Color, Stack.Encoder);
      }
    #ifdef LOG_DEBUG
    /**/Serial.println("*********\n\n\n*********");
    #endif
    }
  while (1)
    {
    LandmarkId=GetLandmarkId(&LandmarkPosition);
    #ifdef LOG_RESULT
    /**/sprintf(TmpMsg, "\n\n result LandmarkId =%d\n", LandmarkId);
    /**/Serial.print(TmpMsg);
    #endif
    if(LandmarkId)
      {
      #ifdef LOG_RESULT  
      /**/sprintf(TmpMsg, "\n\n Befor : G_LinePosition =%lu \n", G_LinePosition);
      /**/Serial.print(TmpMsg);  
      #endif
      CorrectLinePosition(LandmarkId,LandmarkPosition);  
      #ifdef LOG_RESULT
      /**/sprintf(TmpMsg, "\n\n After : G_LinePosition =%lu \n", G_LinePosition);
      /**/Serial.print(TmpMsg);
      #endif
      }
    }
  }
