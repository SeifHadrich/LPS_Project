/*
 * LandmarkReader_PRO.h
 *
 * Created: 05/03/2019 00:53:20
 *  Author: dell
 */ 

#ifndef LANDMARKREADER_PRO_H_
#define LANDMARKREADER_PRO_H_
//Encoder driver prototypes
void Init_ENCODER_DATA();
void Init_ENCODER_IO_ISR();
void ISR_Encoder();
//Ir Sensor driver prototypes
void Init_IR_SENSOR_IO_ISR();
void ISR_IrSensor();
//Landmark Reader librairies
void SaveNewBar(TYPE_LINE_COLOR Color, STR_ENCODER Encoder);
TYPE_BAR_LENGTH GetBarLength(STR_ENCODER LastEncoder, STR_ENCODER PreEncoder);
TYPE_LINE_DIRECTION GetLineDirection(STR_ENCODER LastEncoder, STR_ENCODER PreEncoder);
TYPE_LANDMARK_ID GetLandmarkId(TYPE_LINE_POSITION* LandmarkPosition);
void CorrectLinePosition(byte LandmarkID, TYPE_LINE_POSITION LandmarkPosition);
//Debug and Simulators functions
void SimLandmark();
void Test();


#endif /* LANDMARKREADER_PRO_H_ */