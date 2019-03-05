/*
 * Encoder.cpp
 *
 * Created: 05/03/2019 06:53:09
 *  Author: dell
 */ 

#include "../Include/LandmarkReader_INCLUDE.h"

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