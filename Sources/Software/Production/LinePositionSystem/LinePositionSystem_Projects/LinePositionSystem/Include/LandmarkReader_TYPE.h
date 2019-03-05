/*
 * LandmarkReader_TYPE.h
 *
 * Created: 05/03/2019 00:56:53
 *  Author: dell
 */ 


#ifndef LANDMARKREADER_TYPE_H_
#define LANDMARKREADER_TYPE_H_

#define LOG
#ifdef  LOG
#define LOG_RESULT
#define LOG_CORRECTION
//#define LOG_SAVE
//#define LOG_STACK
//#define LOG_LANDMARK
//#define LOG_SIM
//#define LOG_ERROR
#endif






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
#define IR_SENSOR_SIG_PIN   2
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

#define TYPE_BAR_LENGTH           unsigned short

#define BAD_LINE_POSITION         0
#define ABSOLUTE_0_POSITION       20         //20cm the LANDMARK_ABSOLUTE_0 must to be higher than BAD_LINE_POSITION
#define ABSOLUTE_1_POSITION       10000      //10000cm  = 100m the LANDMARK_ABSOLUTE_1 must to be higher than BAD_LINE_POSITION
#define ABSOLUTE_2_POSITION       20000      //20000cm  = 200m the LANDMARK_ABSOLUTE_2 must to be higher than BAD_LINE_POSITION
#define INCREMENTAL_DISTANCE      1000       //1000cm   = 10m the Incremental distance is 10m
#define MAX_DIFF_CORRECTION       50         //50cm

typedef enum
	{
	LANDMARK_ERROR=0,	
	LANDMARK_UNKNOWN,
	LANDMARK_INCREMENTAL,
	LANDMARK_ABSOLUTE_0,
	LANDMARK_ABSOLUTE_1,
	LANDMARK_ABSOLUTE_2
	}TYPE_LANDMARK_ID;

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

#endif /* LANDMARKREADER_TYPE_H_ */