#include "Include/LandmarkReader_INCLUDE.h"

 
/*
Setup
*/
void setup()
  {
  #ifdef LOG
  //Set printer debug for the Stack
  G_Landmark.Stack.setPrinter (Serial);
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
  STR_LANDMARK_STACK  Stack;
  TYPE_LINE_POSITION  LandmarkPosition;
  TYPE_LANDMARK_ID    LandmarkId;  
  //If the Ir Sensor stack is not empty then we check if a new landmark is available
  if (!G_Landmark.Stack.isEmpty())
    {
    while (!G_Landmark.Stack.isEmpty())
      {
      Stack = G_Landmark.Stack.pop();
      SaveNewBar(Stack.Color, Stack.Encoder);
      }
    }
  //Ge the ID of the landmark
  LandmarkId=GetLandmarkId(&LandmarkPosition);
  //If a new landmark has been found
  //then we correct the line position
  if(LandmarkId)
    {
    CorrectLinePosition(LandmarkId,LandmarkPosition);  
    }
  }  
