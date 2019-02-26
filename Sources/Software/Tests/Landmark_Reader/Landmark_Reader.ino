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

#define TYPE_BAR_LENGTH    byte

typedef enum
  {
  BLACK,
  WHITE
  }TYPE_LINE_COLOR;

typedef struct
  {
  TYPE_LINE_COLOR Color;
  STR_ENCODER     Encoder;
  }STR_LANDMARK_STACK;

typedef struct
  {
  QueueArray <STR_LANDMARK_STACK>   Stack;
  TYPE_LINE_COLOR                   Color[MAX_BARS];
  STR_ENCODER                       Encoder[MAX_BARS];
  TYPE_LINE_DIRECTION               ReadDirection;
  byte                              Index;
  }STR_LANDMARK;

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
    BarLength = abs(LastEncoder.LinePosition - PreEncoder.LinePosition);
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
      G_Landmark.Color[Index] = Color;
      G_Landmark.Encoder[Index] = Encoder;
      #ifdef LOG
      /**/sprintf(TmpMsg,"First Index = %d BarColor %d BarPosition =%d ",Index,G_Landmark.Color[Index],G_Landmark.Encoder[Index].LinePosition );
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
    /**/sprintf(TmpMsg, "BarLength = %d ",BarLength );
    /**/Serial.println(TmpMsg);
    #endif
    //Check if this length is coherent
    if ((BarLength >= SMALL_BAR_LENGTH-MARGIN_ERROR) && (BarLength <= BIG_BAR_LENGTH+MARGIN_ERROR))
      {
      #ifdef LOG  
      /**/Serial.println(" BarLength is OK");  
      #endif
      /*
      When a first bar was read
      we compute the direction of the line
      */
      if (Index == 1)
        {
        G_Landmark.ReadDirection = GetLineDirection(Encoder, G_Landmark.Encoder[Index - 1]);
        #ifdef LOG 
        /**/Serial.println(" GetLineDirection");  
        #endif
        }
      /*
      Check that the line didn't change direction
      */
      if (GetLineDirection(Encoder, G_Landmark.Encoder[Index - 1]) == G_Landmark.ReadDirection)
        {
        #ifdef LOG
        /**/Serial.println(" LineDirection is Ok");
        #endif 
        //it looks like a valid landmark
        G_Landmark.Color[Index] = Color;
        G_Landmark.Encoder[Index] = Encoder;
        #ifdef LOG
        /**/sprintf(TmpMsg, "Index = %d BarColor %d BarPosition =%d ",Index,G_Landmark.Color[Index],G_Landmark.Encoder[Index].LinePosition );
        /**/Serial.println(TmpMsg);
        #endif
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
      Serial.println(">>>>ERROR Landmark = 3 (bar length)");
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
      /**/sprintf(TmpMsg, "Correction new Index = %d BarColor %d BarPosition =%d ",Index,G_Landmark.Color[Index],G_Landmark.Encoder[Index].LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      Index = 1;
      }
    }
  //Update the Landmark Index
  G_Landmark.Index = Index;
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
  Landmark_ID=1(Incremental +1000cm)
  */
  Data.Color=BLACK;
  Data.Encoder.LinePosition=40;
  Data.Encoder.Ticks=50;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif 
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=8;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  G_Encoder.LinePosition=Data.Encoder.LinePosition+30;

  
  /*
  Landmark_ID=1(Absolut 100 cm)
  */
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=G_Encoder.LinePosition+50;
  Data.Encoder.Ticks=50;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=8;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=150;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  G_Encoder.LinePosition=Data.Encoder.LinePosition+30;
  
  
  /*
  Landmark_ID=3(Absolut 100000 cm)
  */
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=G_Encoder.LinePosition+50;
  Data.Encoder.Ticks=50;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=8;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=150;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=150;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  G_Encoder.LinePosition=Data.Encoder.LinePosition+30;
  
  
  
  /*
  Landmark_ID=4(Absolut 15000 cm)
  */
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=G_Encoder.LinePosition+50;
  Data.Encoder.Ticks=50;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=8;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=150;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=150;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=BLACK;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=150;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color=WHITE;
  Data.Encoder.LinePosition+=4;
  Data.Encoder.Ticks=57;
  G_Landmark.Stack.push(Data);
  #ifdef LOG
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu and Ticks= %d", Data.Color, Data.Encoder.LinePosition,Data.Encoder.Ticks);
  /**/Serial.println(TmpMsg);
  #endif
  G_Encoder.LinePosition=Data.Encoder.LinePosition+30;
  }

/*
 * GetLandmarkId
 * Check if a new landmark has is available if so it sends the landmark's ID
 * IN  -> this function will use globlal variables : NbTicks and IrSensor
 * OUT <- the ID of the landmark : 0 if any landmark is available
 *                 > 0 the land mark ID
 */
byte GetLandmarkId()
  {
  byte ii,NbBars;
  byte Index = G_Landmark.Index;
  byte BlackBarSize[MAX_BLACK_BARS];
  byte MyLandmarkPosition[MAX_BARS];
  volatile byte LandmarkCorruption;
  //Check if Index is even and is not null 
  if (Index >=MIN_BARS)
    {
    #ifdef LOG
    /**/sprintf(TmpMsg,"GetLandmarkId\n***\nIndex =%d",Index );
    /**/Serial.println(TmpMsg);    
    #endif
    //Chek if the distance from the last bar is big enough to suppose that the landmark is over
    if(GetBarLength(G_Encoder, G_Landmark.Encoder[Index-1])>=END_WHITE_BAR_LENGTH)
      {
      //Delete the copeid data
      G_Landmark.Index=0; 
      #ifdef LOG
      /**/sprintf(TmpMsg,"GetBarLength= %d \n",GetBarLength(G_Encoder, G_Landmark.Encoder[Index-1]));
      /**/Serial.print(TmpMsg);
      #endif
      //Check if the last color was white
      if(G_Landmark.Color[Index-1]==WHITE)
        {
        #ifdef LOG
        /**/sprintf(TmpMsg,"It looks like a new landmark \n" );
        /**/Serial.print(TmpMsg);
        #endif  
        /*
        Copy the IrSensor.Landmark 
        */
        //When line is going up the data must be copied starting by the head    
        if(G_Landmark.ReadDirection==UP)
          {
          #ifdef LOG  
          /**/Serial.println("Direction = UP");  
          #endif
          //First item first
          for(ii=0;ii<Index;ii++)
            {
            MyLandmarkPosition[ii]=G_Landmark.Encoder[ii].LinePosition;
            }
          }
        //When line is going down the data must be copied starting by the tail
        else
          {
          #ifdef LOG  
          /**/Serial.print("Direction = DOWN");  
          #endif
          //Last item first
          for(ii=Index;ii>0;ii--)
            {
            MyLandmarkPosition[Index-ii]=G_Landmark.Encoder[ii-1].LinePosition;
            }
          }
        #ifdef LOG
        for(ii=0;ii<Index;ii++)
          {
          /**/sprintf(TmpMsg,"Position[%d]=%d Color[%d] =%d \n" ,ii,G_Landmark.Encoder[ii].LinePosition,ii,G_Landmark.Color[ii]);
          /**/Serial.print(TmpMsg);
          }
        #endif    
        /*
        Get the black bars sizes and  numbers 
        */
        NbBars=0;
        for(ii=0;ii<Index;ii+=2)
          {
          BlackBarSize[NbBars]=MyLandmarkPosition[NbBars+1]-MyLandmarkPosition[NbBars];
          #ifdef LOG
          /**/sprintf(TmpMsg,"BlackBarSize[%d] =%d \n" ,NbBars,BlackBarSize[NbBars]);
          /**/Serial.print(TmpMsg);
          #endif
          NbBars++;
          }
        #ifdef LOG
        /**/sprintf(TmpMsg,"total NbBars =%d \n" ,NbBars);
        /**/Serial.print(TmpMsg);
        #endif
        /*
        Check all bars have a good length
        */
        LandmarkCorruption==false;
        //Check if the first black bar has a coherent length  
        //If the first bar length is ok 
        ii=0;
        if(BlackBarSize[0]<=BIG_BAR_LENGTH+ MARGIN_ERROR && BlackBarSize[0]>=SMALL_BAR_LENGTH+ MARGIN_ERROR)  
          {
          while(ii<NbBars && BlackBarSize[ii]<=BIG_BAR_LENGTH+MARGIN_ERROR && BlackBarSize[ii]>=SMALL_BAR_LENGTH-MARGIN_ERROR )ii++;
          }
        //All black bars have a correct length
        if(ii==NbBars)
          {
          #ifdef LOG  
          /**/sprintf(TmpMsg,"A new land mark has been found LandmarkID =%d \n",NbBars);
          /**/Serial.print(TmpMsg);
          #endif  
          return NbBars;
          }
        //At least one black bar doesn't have a correct length
        else
          {
          #ifdef LOG  
          /**/Serial.print("Any valid landmark has been found");   
          #endif
          return 0; 
          }         
        } 
      }
    }
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
  /**/sprintf(TmpMsg, "*** Starting Landmark Reader prorgam ***");
  /**/Serial.println(TmpMsg);
  #endif
  }

/*
  Infinty loop
*/
void loop()
  {
  SimLandmark();
  delay(2000);
  STR_LANDMARK_STACK Stack;
  if (!G_Landmark.Stack.isEmpty())
    {
    /**/Serial.println("*********\n\n\n*********");  
    while (!G_Landmark.Stack.isEmpty())
      {
      Stack = G_Landmark.Stack.pop();
      #ifdef LOG  
      /**/sprintf(TmpMsg, "\n\n\nColor=%d \t| LinePosition=%hu \t| Ticks=%d\n", Stack.Color, Stack.Encoder.LinePosition, Stack.Encoder.Ticks);
      /**/Serial.print(TmpMsg);
      #endif 
      SaveNewBar(Stack.Color,Stack.Encoder);
      }
    /**/Serial.println("*********\n\n\n*********");
    }
  while(1)
    {
    GetLandmarkId();
    delay(2000);
    }
  }
