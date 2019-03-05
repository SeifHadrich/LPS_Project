/*
 * LandmarkReader.cpp
 *
 * Created: 05/03/2019 07:01:47
 *  Author: dell
 */ 

#define MAIN_FILE
#include "../Include/LandmarkReader_INCLUDE.h"

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
SaveNewBar
Save one bar of the landmark
IN  -> Color   : Bar color (could be black or white)
      NbTicks : Bar position
OUT <- -
*/
void SaveNewBar(TYPE_LINE_COLOR Color, STR_ENCODER Encoder)
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
      #ifdef LOG_SAVE
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
    #ifdef LOG_SAVE
    /**/sprintf(TmpMsg, "\tBarLength = %d ", BarLength );
    /**/Serial.println(TmpMsg);
    #endif
    //Check if this length is coherent
    if ((BarLength >= SMALL_BAR_LENGTH - MARGIN_ERROR ) && (BarLength <= SMALL_BAR_LENGTH + MARGIN_ERROR) ||
        (BarLength >= BIG_BAR_LENGTH - MARGIN_ERROR   ) && (BarLength <= BIG_BAR_LENGTH + MARGIN_ERROR))
      {
      #ifdef LOG_SAVE
      /**/Serial.println("\tBarLength is OK");
      #endif
      /*
      When a first bar was read
      we compute the direction of the line
      */
      if (Index == 1)
        {
        G_Landmark.ReadDirection = GetLineDirection(Encoder, G_Landmark.Encoder[Index - 1]);
        #ifdef LOG_SAVE
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
        #ifdef LOG_SAVE
        /**/Serial.println("\tLineDirection is Ok");
        #endif
        //it looks like a valid landmark
        G_Landmark.Color[Index] = Color;
        G_Landmark.Encoder[Index] = Encoder;
        #ifdef LOG_SAVE
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
      #ifdef LOG_SAVE
      /**/sprintf(TmpMsg, "Correction : Index = %d BarColor %d BarPosition =%d ", Index, G_Landmark.Color[Index], G_Landmark.Encoder[Index].LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      Index = 1;
      }
    }
  //Update the Landmark Index
  G_Landmark.Index = Index;
  }


/*
CorrectLinePosition
Correct the line position
IN  -> LandmarkID, LandmarkPosition;
OUT <- -
*/

void CorrectLinePosition(byte LandmarkID, TYPE_LINE_POSITION LandmarkPosition)
  {
  TYPE_LINE_POSITION Diff  ;
  static TYPE_LINE_POSITION  PreIncrePosition = 0;
  switch (LandmarkID)
    {
    case LANDMARK_ABSOLUTE_0 :
      G_LinePosition = ABSOLUTE_0_POSITION + (G_Encoder.LinePosition- LandmarkPosition);
      PreIncrePosition = LandmarkPosition;
      #ifdef LOG_CORRECTION
      /**/sprintf(TmpMsg, "LANDMARK_ABSOLUTE_0 : LandmarkPosition = %hu,\tABSOLUTE_0_POSITION=%hu,\tG_LinePosition=%hu\t", LandmarkPosition,
                  ABSOLUTE_0_POSITION, G_LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      break;
    case LANDMARK_ABSOLUTE_1 :
      G_LinePosition = ABSOLUTE_1_POSITION + (G_Encoder.LinePosition- LandmarkPosition);
      PreIncrePosition = LandmarkPosition;
      #ifdef LOG_CORRECTION
      /**/sprintf(TmpMsg, "LANDMARK_ABSOLUTE_1 : LandmarkPosition = %hu,\tABSOLUTE_1_POSITION=%hu,\tG_LinePosition=%hu", LandmarkPosition,
                  LANDMARK_ABSOLUTE_1, G_LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      break;
    case LANDMARK_ABSOLUTE_2 :
      G_LinePosition = ABSOLUTE_2_POSITION + (G_Encoder.LinePosition- LandmarkPosition);
      PreIncrePosition = LandmarkPosition;
      #ifdef LOG_CORRECTION
      /**/sprintf(TmpMsg, "LANDMARK_ABSOLUTE_2 : LandmarkPosition = %hu,\tABSOLUTE_0_POSITION=%hu,\tG_LinePosition=%hu",
                  LandmarkPosition, LANDMARK_ABSOLUTE_2, G_LinePosition);
      /**/Serial.println(TmpMsg);
      #endif
      break;
    case LANDMARK_UNKNOWN:
    case LANDMARK_INCREMENTAL :
      if (G_LinePosition > BAD_LINE_POSITION)
        {
        Diff = (PreIncrePosition + INCREMENTAL_DISTANCE) > LandmarkPosition ?
             (PreIncrePosition + INCREMENTAL_DISTANCE) - LandmarkPosition   :
             LandmarkPosition - (PreIncrePosition + INCREMENTAL_DISTANCE);
        //An acceptable difference
        if (Diff < MAX_DIFF_CORRECTION)
          {
          #ifdef LOG_CORRECTION
          /**/sprintf(TmpMsg, "Acceptable Diff : Diff = %hu,\t PreIncrePosition+INCREMENTAL_DISTANCE=%hu,\t LandmarkPosition=%hu",
                      Diff, PreIncrePosition + INCREMENTAL_DISTANCE, LandmarkPosition);
          /**/Serial.println(TmpMsg);
          G_LinePosition = LANDMARK_INCREMENTAL + PreIncrePosition +(G_Encoder.LinePosition- LandmarkPosition);
          PreIncrePosition = LANDMARK_INCREMENTAL + PreIncrePosition;
          #endif
          }
        //There is a big difference between a current line position and what it has to be !
        else
          {
          G_LinePosition = BAD_LINE_POSITION;
          /**/sprintf(TmpMsg, "Error Big Diff : Diff = %hu,\t PreIncrePosition+INCREMENTAL_DISTANCE=%hu,\t LandmarkPosition=%hu",
                      Diff, PreIncrePosition + INCREMENTAL_DISTANCE, LandmarkPosition);
          /**/Serial.println(TmpMsg);
          /**/Serial.println("=> BAD_LINE_POSITION");
          }
        }
      else
        {
        #ifdef LOG_CORRECTION
        /**/Serial.println("Cannot Inctrement it's a bad position");
        #endif  
        }
      break;
    }
  }

  
/*
GetLandmarkId
Check if a new landmark has is available if so it sends the landmark's ID
IN  -> this function will use globlal variables : NbTicks and IrSensor
OUT <- the ID of the landmark : 0 if any landmark is available
               > 0 the land mark ID
*/
TYPE_LANDMARK_ID GetLandmarkId(TYPE_LINE_POSITION *LandmarkPosition)
  {
  byte ii, NbBars;
  byte Index = G_Landmark.Index;
  byte BlackBarSize[MAX_BLACK_BARS];
  TYPE_LINE_POSITION MyLandmarkPosition[MAX_BARS];
  volatile byte LandmarkCorruption;
  //Check if Index is even and is not null
  if (Index >= MIN_BARS)
    {
    #ifdef LOG_LANDMARK
    /**/sprintf(TmpMsg, "GetLandmarkId\n************\nIndex =%d", Index );
    /**/Serial.println(TmpMsg);
    #endif
    //Chek if the distance from the last bar is big enough to suppose that the landmark is over
    if (GetBarLength(G_Encoder, G_Landmark.Encoder[Index - 1]) >= END_WHITE_BAR_LENGTH)
      {
      //Delete the copeid data
      G_Landmark.Index = 0;
      #ifdef LOG_LANDMARK
      /**/sprintf(TmpMsg, "GetBarLength= %d \n", GetBarLength(G_Encoder, G_Landmark.Encoder[Index - 1]));
      /**/Serial.print(TmpMsg);
      #endif
      //Check if the last color was white
      if (G_Landmark.Color[Index - 1] == WHITE)
        {
        #ifdef LOG_LANDMARK
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
        #ifdef LOG_LANDMARK
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
          #ifdef LOG_LANDMARK
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
          *LandmarkPosition=MyLandmarkPosition[0];
          #ifdef LOG_LANDMARK
          /**/sprintf(TmpMsg, "A new landmark has been found LandmarkID =%d Position=%hu %hu\n", NbBars,*LandmarkPosition,MyLandmarkPosition[0]);
          /**/Serial.print(TmpMsg);
          #endif
          return (TYPE_LANDMARK_ID)NbBars;
          }
        //At least one black bar doesn't have a correct length
        else
          {
          #ifdef LOG_LANDMARK
          /**/Serial.print("No valid landmark has been found");
          /**/Serial.flush();
          #endif
          *LandmarkPosition=MyLandmarkPosition[0];  
          return LANDMARK_UNKNOWN;
          }
        }
      }
    }
  return LANDMARK_ERROR;
  }