#define LOG

/***
if line direction is UP then : 
***/
	
	/*
	---------------------
	INCREMENTAL LANDMARK 
    LandmarkID=INCREMENTAL
	---------------------	
	WHITE|BLACK|BLACK|WHITE|WHITE|WHITE
		 0           1          
	   begin                           end                          
	*/

	/*
	---------------------
	LANDMARK for 0 m 
	LandmarkID=ABSOLUTE_0
	---------------------
	WHITE|BLACK|BLACK|WHITE|BLACK|WHITE|WHITE|WHITE
		 0           1     2     3     
	   begin                                       end 
   */

	/*
	---------------------
	LANDMARK for 100 m
	LandmarkID=ABSOLUTE_100	
	---------------------
	WHITE|BLACK|BLACK|WHITE|BLACK|WHITE|BLACK|WHITE|WHITE|WHITE
		 0           1     2     3     4     5     
	   begin                                                   end 	
	*/

	/*
	---------------------
	LANDMARK for 200 m
	LandmarkID=ABSOLUTE_200
	---------------------
	WHITE|BLACK|BLACK|WHITE|BLACK|WHITE|BLACK|WHITE|BLACK|WHITE|WHITE|WHITE
		 0           1     2     3     4     5     6     7
	   begin                                                               end 	
	*/

/***
if line direction is DOWN then : 
***/
	//todo




	
	
	












#include <StackArray.h>
















	




/*
#####################################################################################################################################
#####################################################################################################################################
#################							byte CheckNewLandmark(void)												#################
#####################################################################################################################################
#####################################################################################################################################
*/	

/*
 * CheckNewLandmark
 * Check if a new landmark has is available
 * IN  -> this function will use globlal variables : NbTicks and IrSensor
 * OUT <- the ID of the landmark : 0 if any landmark is available
 *								 > 0 the land mark ID
 */
byte CheckNewLandmark(void);
	{
	byte DiffTicks,ii;
	byte Index = IrSensor.Landmark.Index;
	byte NbBars=0;
	byte BlackBarSize[MAX_BLACK_BARS];
	byte MyLandmarkTicks[MAX_BARS];
	byte LandmarkCorruption;
	//Check if Index is even and is not null 
	if ((Index >0) && (Index % 2==0))
		{		
		DiffTicks=DiffTicks(G_NbTicks,IrSensor.Landmark.Ticks[Index]))
		//Chek if the distance from the last bar is big enough to suppose that the landmark is over
		if(abs(DiffTicks)> END_LENGHT_LANDMARK)
			{
			//Check if the last color was white
			if(IrSensor.Landmark.Color[Index]==WHITE)
				{
				#ifdef LOG
				/**/sprintf(TmpMsg,"It looks like a new landmark \n" );
				/**/Serial1.print(TmpMsg);
				#endif	
				/*
				Copy the IrSensor.Landmark 
				*/
				//Depond on the line direction MyLandmarkTicks is filled in order to get always the first bar as a first item in the array		
				if(IrSensor.Landmark.Direction==UP)
					{
					//First item first
					for(ii=0;ii<Index;ii++)
						{
						MyLandmarkTicks[ii]=IrSensor.Landmark.Ticks[ii];
						}
					}
				//When line is going down the data must be copied starting by the end
				else
					{
					//Last item first
					for(ii=Index;ii>0;ii--)
						{
						MyLandmarkTicks[Index-ii]=IrSensor.Landmark.Ticks[ii-1];
						}
					}
				#ifdef LOG
				for(ii=0;ii<Index;ii++)
					{
					/**/sprintf(TmpMsg,"IrSensor.Landmark.Ticks[%d] =%d \n" ,ii,IrSensor.Landmark.Ticks[ii]);
					/**/Serial1.print(TmpMsg);
					/**/sprintf(TmpMsg,"IrSensor.Landmark.Color[%d] =%d \n" ,ii,IrSensor.Landmark.Color[ii]);
					/**/Serial1.print(TmpMsg);
					}
				#endif		
				/*
				Get the blak bars sizes and  numbers 
				*/
				for(ii=0;ii<Index;ii+=2)
					{
					BlackBarSize[NbBars]=abs(DiffTicks(MyLandmarkTicks[ii+1]-MyLandmarkTicks[ii]));
					#ifdef LOG
					/**/sprintf(TmpMsg,"BlackBarSize[%d] =%d \n" ,NbBars,BlackBarSize[NbBars]);
					/**/Serial1.print(TmpMsg);
					#endif
					NbBars++;
					}
				#ifdef LOG
				/**/sprintf(TmpMsg,"NbBars =%d \n" ,NbBars);
				/**/Serial1.print(TmpMsg);
				#endif
				/*
				Check all bars have a valid length
				*/
				LandmarkCorruption==FALSE;
				//Check if the first black bar has a coherent lenght 	
				if(abs((BlackBarSize[0]/2)-BAR_LENGHT) > ERROR_MARGIN) 
					{
					LandmarkCorruption=TRUE;
					#ifdef LOG
					/**/sprintf(TmpMsg,"LandmarkCorruption ii =0 \n" );
					/**/Serial1.print(TmpMsg);
					#endif
					}
				//If the first bar is ok we check the rest of the bars
				else	
					{
					for (ii=1;ii<NbBars;ii++)
						{
						//Check if the black bar has a coherent lenght 	
						if(abs((BlackBarSize[ii])-BAR_LENGHT) > ERROR_MARGIN) 
							{
							LandmarkCorruption=TRUE;
							#ifdef LOG
							/**/sprintf(TmpMsg,"LandmarkCorruption ii =%d \n",ii);
							/**/Serial1.print(TmpMsg);
							#endif
							break;
							}
						}
					}
				//All black bars have a correct lenght
				if(LandmarkCorruption==FALSE)
					{
					return NbBars;
					}
				//At least one black bar doesn't have a correct lenght
				else
					{
					return 0;	
					}					
				}	
			}
		}
	}
	
/*
#####################################################################################################################################
#####################################################################################################################################
#################				void SaveNewLandmark(byte Color, byte NbTicks)										#################
#####################################################################################################################################
#####################################################################################################################################
*/	
/*
 * SaveNewLandmark
 * Read and save a line landmark (it 's look like a barcode)
 * IN  -> Color : Color of the bar (could be black or white)
 *		  NbTicks : The position of the bar
 * OUT <- -
 */
void SaveNewLandmark(byte Color, byte NbTicks)
	{
	byte Index = IrSensor.Landmark.Index;
	byte DiffTicks,CurrentDirection,BarLenght ; 
	//Check if the system allows read a new landmark	
	/*
	The Start of a new Landmark
	*/
	if (Index == 0 )
		{
		//This look like a valid landmark
		if(Color == BLACK)
			{				
			IrSensor.Landmark.Color[Index]=Color;
			IrSensor.Landmark.Ticks[Index]=NbTicks;
			Index=1;
			}
		//Something is worng with this landmark
		else
			{
			//delete the current landmark
			Index =0;
			//log error
			Serial.println("ERROR Landmark = 1 "); //error log
			}
		}
	/*
	Whithin a landmark		
	Index > 0
	*/
	else
		{
		//get the lenght of one bar of landmark
		DiffTicks = DiffTicks(NbTicks,IrSensor.Landmark.Ticks[Index-1]);
		//check that this lenght is coherent
		BarLenght = abs(DiffTicks);
		if (BarLenght > MIN_LENGHT_LANDMARK && BarLenght < MAX_LENGHT_LANDMARK)
			{
			/*
			When a first bar was read 
			we compute the direction of the line 	
			*/
			if (Index == 1)
				{
				if (DiffTicks(NbTicks,IrSensor.Landmark.Ticks[Index-1]) > 0 ) IrSensor.Landmark.Direction = UP;
				else IrSensor.Landmark.Direction = DOWN;				
				}
			/*
			Compute the current direction of the line
			*/
			if (DiffTiks>0)CurrentDirection=UP;
			else CurrentDirection=DOWN;
			//check that the line didn't change direction and there is a color change  
			if(CurrentDirection==IrSensor.Landmark.Direction && Color!=IrSensor.Landmark.Color[Index-1])
				{
				IrSensor.Landmark.Color[Index]=Color;
				IrSensor.Landmark.Ticks[Index]=NbTicks;
				Index++;
				}
			//if the line changes direction while reading a landmark
			/*
			The current system does not yet support line direction change while reading a landmark
			ToDo !!!
			*/
			else 
				{
				//delete the current landmark
				Index =0;
				//log error
				Serial.println("ERROR Landmark = 2 "); 	
				}
			}
		//strange landmark lenght
		//it  could be because of line direction change or maybe it's just a dust in the line
		else
			{
			//delete the current landmark
			Index =0;
			//log error
			Serial.println("ERROR Landmark = 3 "); 
			}
		/*
		If somothing went wonrg
		this could be the start of a new good landmark
		do I decide to save it
		*/
		if(Index==0 Color==BLACK)
			{
			IrSensor.Landmark.Color[Index]=Color;
			IrSensor.Landmark.Ticks[Index]=NbTicks;
			Index=1;
			}
		}
	//update the Landmark Index
	IrSensor.Landmark.Index=Index;
	}

	
	
	
	
	
	
	
	
	
	
	
	
	
/*
#####################################################################################################################################
#####################################################################################################################################
#################									DATA STUCTURE													#################
#####################################################################################################################################
#####################################################################################################################################
*/

	
/*
///////////////////////////////////////////////////////
####			SYSTEM CONSTANTS					###
///////////////////////////////////////////////////////
*/
	
	
#define MAX_BLACK_BARS 		4			//nb max of black bars that syteme can handle
#define MAX_BARS			8			//nb max of total bars (black and white)


#define BAR_LENGHT 			?
#define ERROR_MARGIN 		?			// = 0.25 * BAR_LENGHT
#define END_LENGHT_LANDMARK ?			// =  3   * BAR_LENGHT	
#define MIN_LENGHT_LANDMARK ?			// = 0.75 * BAR_LENGHT
#define MAX_LENGHT_LANDMARK	?			// = 1.25 * BAR_LENGHT

/*
///////////////////////////////////////////////////////
####			NEW DATA TYPE						###
///////////////////////////////////////////////////////
*/

typedef enum
	{
	BLACK,
	WHITE
	}TYPE_LINE_COLOR;

#define TYPE_TICKS	 ?	

typedef enum
	{
	UP,
	DOWN
	}TYPE_LINE_DIRECTION;

typedef struct
	{
	TYPE_LINE_COLOR Color;
	TYPE_TICKS	    Ticks;	
	}STR_LANDMARK_STACK;	

typedef struct
	{
	StackArray <STR_LANDMARK_STACK> 	Stack;
	TYPE_LINE_COLOR						Color[MX_BARS];
	TYPE_TICKS 							Ticks[MX_BARS];
	TYPE_LINE_DIRECTION					Direction;
	byte 								Index;
	}STR_LANDMARK;

/*
///////////////////////////////////////////////////////
####			GOLOBAL VARIABLES					###
///////////////////////////////////////////////////////
*/
STR_LANDMARK 	Landmark    ? ;
TYPE_TICKS		G_NbTicks =0;  

#ifdef LOG
char TmpMsg[100];
#endif
	
/*
#####################################################################################################################################
#####################################################################################################################################
#################								HARDWARE SETTINGS													#################
#####################################################################################################################################
#####################################################################################################################################
*/
/*
 * Init_IO_ISR
 * Initialise input output and set interrups
 * IN  -> -
 * OUT <- -
 */
Init_IO_ISR()
  {
  #define IR_SENSOR_SIG_PIN   2
  #define ENCODER_A_PIN       3
  #define ENCODER_B_PIN       4
  pinMode(IR_SENSOR_SIG_PIN,INPUT_PULLUP);
  pinMode(ENCODER_A_PIN,INPUT_PULLUP);
  pinMode(ENCODER_B_PIN,INPUT_PULLUP);
  //Set ISR_IrSensor interrupt to be triggered as soon as  the IrSensor's signal changes
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_SIG_PIN), ISR_IrSensor, CHANGE );
  //Set ISR_Encoder interrupt to be triggered as soon as ENCODER_A_PIN raises
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), ISR_Encoder, RISING  );
  }

/*
#####################################################################################################################################
#####################################################################################################################################
#################							INTERRUP ROUTINES SYSTEM												#################
#####################################################################################################################################
#####################################################################################################################################
*/

/*
 * ISR_IrSensor
 * An interrupt system routine triggered as soon as the IR sensor detects a new color
 * IN  -> -
 * OUT <- -
 */
ISR_IrSensor ()
	{
	if(Landmark.Stack.isFull())
		{
		#ifdef LOG
		/**/sprintf(TmpMsg,"System error  : Landmark.Stack.isFull Landmark.Stack.count=%d \n",Landmark.Stack.count);
		/**/Serial1.print(TmpMsg);
		#endif	
		//Empty the landmark stack
		while(!Landmark.StackisEmpty ())Landmark.Stack.pop();
		#ifdef LOG
		/**/sprintf(TmpMsg,"Landmark.Stack has been emptied");
		/**/Serial1.print(TmpMsg);
		#endif	
		}
	//Get the landmark's color	
	if(digital.read(IR_SENSOR_SIG_PIN) = ?)Landmark.Stack.Color = BLACK;
	else Landmark.Stack.Color = WHITE;
	//Get the current ticks number
	Landmark.Stack.Ticks=G_NbTicks;
	/*
	As i can not call the SaveNewLandmark function because the isr should be short and fast
	I decided to push this data in a stack
	*/
	//Push the color and the ticks
	Landmark.Stack.Color.push(Landmark.Stack);
	}

/*
 * ISR_Encoder
 * An interrupt system routine triggered as soon as the encoder detects a new tick
 * IN  -> -
 * OUT <- -
 */
ISR_Encoder ()
	{
	//Depond on the pulley's rotation the G_NbTicks will increase or decrease
	if (digital.read(ENCODER_B_PIN))G_NbTicks++;
	else G_NbTicks--;
	}

/*
#####################################################################################################################################
#####################################################################################################################################
#################								MAIN PROGRAM														#################
#####################################################################################################################################
#####################################################################################################################################
*/



/*
Setup
*/
setup()
	{
	Init_Data();
	Init_IO_ISR();
	#ifdef LOG
	//Set the serial comm at 115200 bauds 
	/**/Serial1.begin(115200); 
	/**/sprintf(TmpMsg,"*** Starting Line position prorgam ***");
	/**/Serial1.print(TmpMsg);
	/**/Landmark.Stack.setPrinter (Serial1);// set the printer of the landmark stack.
	#endif
	}

/*
Infinty loop
*/
loop()
	{
	//If the IrSensor send a new data
	if (!Landmark.Stack.isEmpty ())
		{
		//Save the landmark data	
		SaveNewLandmark(Landmark.Stack.Color.pop(Color),Landmark.Stack.Ticks.pop(Color));
		}
	//Check if a new full landmark is available
	if (CheckNewLandmark)
		{
		//Based on the Lanmdrak we can correct the line position	
		CorrectLinePosition();
		}
	//The system updates the line position permanetly
	if (TRUE)
		{
		UpdateLinePosition();
		}
	//Check if the flight control ask for the line position
	if(FlightControlRequest())
		{
		//Answer Flight Control
		AnswerFlightControl();
		}
	}	