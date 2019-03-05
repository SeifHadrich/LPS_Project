/*
 * IrSensor.cpp
 *
 * Created: 05/03/2019 06:53:24
 *  Author: dell
 */ 

#include "../Include/LandmarkReader_INCLUDE.h"

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


