/*
 * Simulator.cpp
 *
 * Created: 05/03/2019 17:52:26
 *  Author: User
 */ 
#include "../Include/LandmarkReader_INCLUDE.h"


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
  #ifdef LOG_SIM
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color = WHITE;
  Data.Encoder.LinePosition = 140;
  G_Landmark.Stack.push(Data);
  #ifdef LOG_SIM
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color = BLACK;
  Data.Encoder.LinePosition = 136;
  G_Landmark.Stack.push(Data);
  #ifdef LOG_SIM
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  Data.Color = WHITE;
  Data.Encoder.LinePosition = 128;
  G_Landmark.Stack.push(Data);
  #ifdef LOG_SIM
  /**/sprintf(TmpMsg, "push color =%d and LinePosition = %hu", Data.Color, Data.Encoder.LinePosition);
  /**/Serial.println(TmpMsg);
  #endif
  G_Encoder.LinePosition = Data.Encoder.LinePosition - 30;
  }

/*
SimLandmark
Test the simulator
IN  -> -
OUT <- - 
*/
void Test()
  {
  SimLandmark();
  STR_LANDMARK_STACK  Stack;
  TYPE_LINE_POSITION  LandmarkPosition;
  TYPE_LANDMARK_ID    LandmarkId;
  if (!G_Landmark.Stack.isEmpty())
    {
    #ifdef LOG_STACK  
    /**/Serial.println("************\n\n\n************");
    #endif
    while (!G_Landmark.Stack.isEmpty())
      {
      Stack = G_Landmark.Stack.pop();
      #ifdef LOG_STACK
      /**/sprintf(TmpMsg, "\n----pull Color=%d \t| LinePosition=%hu \t\n", Stack.Color, Stack.Encoder.LinePosition);
      /**/Serial.print(TmpMsg);
      #endif
      SaveNewBar(Stack.Color, Stack.Encoder);
      }
    #ifdef LOG_STACK
    /**/Serial.println("*********\n\n\n*********");
    #endif
    }
  //Get the landmark Id
  LandmarkId=GetLandmarkId(&LandmarkPosition);
  //#ifdef LOG_RESULT
  /**/sprintf(TmpMsg, "\n\n result LandmarkId =%d  LandmarkPosition =%hu \n", LandmarkId,LandmarkPosition);
  /**/Serial.print(TmpMsg);
  //#endif
  if(LandmarkId)
    {
    G_LinePosition= 100;
    #ifdef LOG_RESULT  
    /**/sprintf(TmpMsg, "\n\n Befor : G_LinePosition =%hu \n", G_LinePosition);
    /**/Serial.print(TmpMsg);  
    #endif
    CorrectLinePosition(LandmarkId,LandmarkPosition);  
    #ifdef LOG_RESULT
    /**/sprintf(TmpMsg, "\n\n After : G_LinePosition =%hu \n", G_LinePosition);
    /**/Serial.print(TmpMsg);
    #endif
    }
  }
    
