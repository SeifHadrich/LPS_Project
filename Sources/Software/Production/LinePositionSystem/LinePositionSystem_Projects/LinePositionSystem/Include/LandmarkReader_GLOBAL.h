/*
 * Landmark_GLO.h
 *
 * Created: 05/03/2019 06:54:18
 *  Author: dell
 */ 


#ifndef LANDMARK_GLO_H_
#define LANDMARK_GLO_H_


#ifdef MAIN_FILE
		#define EXTERN 
#else	
		#define EXTERN extern
#endif

EXTERN STR_ENCODER				G_Encoder;
EXTERN STR_LANDMARK				G_Landmark;
EXTERN TYPE_LINE_POSITION		G_LinePosition;
EXTERN char TmpMsg[100];//Debug msg

#endif /* LANDMARK_GLO_H_ */