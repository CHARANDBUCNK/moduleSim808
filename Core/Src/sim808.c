/*
 * sim808.c
 *
 *  Created on: Dec 24, 2024
 *      Author: Jose Rodolfo
 *
 */


#include "sim808.h"

static const char * AtComandos[] =
{
	[At]                     = "AT"            ,
	[TurnOnGps]              = "AT+CGPSPWR=1"  ,
	[TurnOFFGps]             = "AT+CGPSPWR=0"  ,
    [GpStatus]               = "AT+CGPSSTATUS?",
    [ReadGps]                = "AT+CGPSINF=2"  ,
	[RequestNema]            = "AT+CGPSOUT=2"  ,
    [turnoffRequestNema]     = "AT+CGPSOUT=0"  ,
    [SetModeMessage]         = "AT+CMGF=1"     ,
    [ATGSM]                  = "AT+CMGS="      , //
    [checkStateRequesStatus] = "AT+CGPSPWR?"   ,
    [DeletElement]           = "AT+CMGD="      , //parameterCommnad
    [readGsm  ]              = "AT+CMGR="      , //read Message From Buffer side
	[FinalCommand]           = "dont match"		 //this just debugger timer function
};

static const char* AtEvents[]=
{
	[pgga]                        = "GPGGA"           ,
    [CGPS_STATUS_OK]              = "Location 3D Fix" ,
    [CGPS_STATUS_NOTFIX]          = "Location Not Fix",
    [CGPS_STATUS_LOCATION_UNKNOW] = "Location Unknown",
    [CGPS_STATUS_LOCATION_2D]     = "Location 2D Fix" ,
    [ok]                          = "OK"              ,
    [error]                       = "ERROR"           ,
	[Rdy]                         = "RDY"             ,
    [OKMESSAGE]                   = "+CMGR"           ,
    [CGPSPWR_ON]                  = "+CGPSPWR: 1"     ,
    [CGPSPWR_OFF]                 = "+CGPSPWR: 0"     ,
    [asterisk]                    = "*"               ,
	[ WRITE_MESSAGE ]  		      = ">" 		      ,
    [carryAndReturn]              = "\r\n"            ,
    [requestInfor]                = "+CGPSINF:"
};



static const AtEnumEvent_t ResponseSendMessage[]=
{
	WRITE_MESSAGE,
	'\0',
};

static const AtEnumEvent_t ResponseMessageEvents[] =
{
   OKMESSAGE,
   ok,
   error,
   '\0'
};


static const AtEnumEvent_t ResponseOkErrorAtEvents[] = {
    ok      ,
    error   ,
    '\0'
};

static const AtEnumEvent_t ResonseGps[]=
{
		pgga    ,
		'\0'
};

static const AtEnumEvent_t IsPowerOn[]=
{
    CGPSPWR_ON  ,
    CGPSPWR_OFF ,
    '\0'
};

static const AtEnumEvent_t RquestStatus[] =
{
    CGPS_STATUS_OK              ,
    CGPS_STATUS_NOTFIX          ,
    CGPS_STATUS_LOCATION_UNKNOW ,
    CGPS_STATUS_LOCATION_2D     ,
    '\0'
};

static const AtEnumEvent_t cgpsInf[] =
{
    requestInfor,
    '\0'
};

static const AtEnumEvent_t * AtCommandWihtPosibleEvent[] =
{
		[     At    ]              = ResponseOkErrorAtEvents ,
		[ TurnOnGps ]              = ResponseOkErrorAtEvents ,
        [ checkStateRequesStatus ] = IsPowerOn               ,
        [ TurnOFFGps             ] = ResponseOkErrorAtEvents ,
        [ GpStatus  ]              = RquestStatus            ,
        [ ReadGps   ]              = cgpsInf                 ,
        [ SetModeMessage ]         = ResponseOkErrorAtEvents ,
        [ readGsm     ]   	       = ResponseMessageEvents   ,
		[ ATGSM ] 				   = ResponseSendMessage
};

typedef enum
{
	SEQUENCE_END 		,
	EVENT_DATA  		,
	EVENT_GPGGA  		,
	MESSAGE
}format_t;


typedef struct
{
	AtCommand_t command ;
	AtEnumEvent_t* pAtResponse ;
	format_t* sequence ;
}stccommnad_t ;

format_t simpleSequence[] = { EVENT_DATA , SEQUENCE_END };
format_t dobleSequence [] = { EVENT_DATA , EVENT_DATA , SEQUENCE_END };



format_t* format[] =
{
		[ At ] = simpleSequence ,
		[ TurnOnGps ] = dobleSequence
};



char* DataBetweenCharacter( char* pData , char* pDataToCopy , char* pFirstElemnt , char* pSecondElemnt );





format_t* SequenseEvent( AtCommand_t commandToSim )
{
	return format[ commandToSim ];
}






//return posible event from AT COMMAND
const AtEnumEvent_t* ResponseMessage( AtCommand_t commandToSim )
{
	const AtEnumEvent_t* currentEvent = AtCommandWihtPosibleEvent[ commandToSim ] ;

	return ( currentEvent  );
}


//return string  Event
const char* EventAtCommand(AtEnumEvent_t pEvent )
{

	return AtEvents[ pEvent ];
}


const char* AtCommands( AtCommand_t commandToSim )
{

	return AtComandos[ commandToSim ];
}

void* GetSimBufferPointer( sim808_t* pSim808 )
{
	return pSim808->simBuffer->pBuffer;
}


void SetTxPrint( sim808_t* pSim808 ,PtxPrintf_t pTxFunciton , void* pUserParameter )
{
	pSim808->txFunction.Transmit = pTxFunciton ;
	pSim808->txFunction.pUserParameters = pUserParameter ;

}


void EmptBufferSim808( sim808_t* pSim808 )
{
	char pTemporalData = 0 ;
	while( NElemensFromBufferSim( pSim808 ) )
	{
		pTemporalData = ElementFromSim(pSim808);
	}
}



void SetRxBuffe( sim808_t* pSim808 , PrxInputUser_t pRxFunciton , PxThereElemnt_t pThereElement, void* pUserParameter )
{
	pSim808->rxFunciton.ReadBuffer = pRxFunciton ;
	pSim808->rxFunciton.IsthereElement = pThereElement ;
	pSim808->rxFunciton.pUserParameters = pUserParameter ;
}

// send Command checkThe list of commands ---> AtCommand_t <----
void SendCommand( sim808_t* pSim808 , AtCommand_t commandToSim )
{


	//send a command to SIM module without parameter just command
	SEND_TO_SIM_MODULE(  pSim808->txFunction.pUserParameters , "%s\n" , AtCommands( commandToSim ) );
	//pSim808->txFunction->Transmit( pSim808->txFunction->pUserParameters , "%s\n" , AtCommands( commandToSim ) );
}

int8_t SendAndSimpleResponse(  sim808_t* pSim808 , AtCommand_t commandToSim )
{
	//send the command
	SendCommand( pSim808 , commandToSim );
	uint32_t startTimer = GetTimer( pSim808 );
	int8_t status_sim = SIM_OK ;
	//read the buffer from sim808, user implemented

	//reset the buffer before using
	ResetBuffer( GetSimBufferPointer( pSim808 ) );

	// wait 4 times more just int case at least 200 ms
	while( (( GetTimer( pSim808 ) - startTimer ) < ( TIME_TO_WAIT_SIM_RESPONSE * 4 )) )
	{
		//read the buffer and save it
		ReadBufferFromUser( pSim808 );

		//Search for some pather and return position patter
		status_sim = FindPather( (char*)GetSimBufferPointer( pSim808 ), commandToSim );

		//if find something

		if( status_sim >= 0 ) break;

	}

	return status_sim ;
}



// some command needs parameter  checkThe list of commands ---> AtCommand_t <----, parameter up to user
void SendCommandWhitParameter( sim808_t* pSim808 , AtCommand_t commandToSim , const char* pParameter )
{
	//simparameter
	//string delimiter
	//at commnad
	// parameter
	pSim808->txFunction.Transmit( pSim808->txFunction.pUserParameters
									, "%s%s \n"
									, AtCommands( commandToSim )
									, pParameter );
}

uint8_t SendMessageToPhoneNumber(  sim808_t* pSim808 , const char* pPhoneNumber , const char* pMessage )
{
	return 0;
}



//time in milisecond
uint16_t NElemensFromBufferSim( sim808_t* pSim808  )
{

	return pSim808->rxFunciton.IsthereElement( pSim808->rxFunciton.pUserParameters ) ;
}

char ElementFromSim(  sim808_t* pSim808 )
{

	return pSim808->rxFunciton.ReadBuffer(  pSim808->rxFunciton.pUserParameters  );
}

bool ReadBufferUntillPather( sim808_t* pSim808 , char* pStarPosition  ,const char* pPathern )
{

	uint32_t counterTimer = GetTimer(pSim808);
	char* pFindPosition = NULL ;

	while( ( GetTimer( pSim808 ) - counterTimer ) < TIME_TO_WAIT_SIM_RESPONSE  )
	{
		ReadBufferFromUser( pSim808 );

		pFindPosition = strstr( pStarPosition , pPathern );

		if( pFindPosition ) break;
	}

	return (bool)pFindPosition;

}



void ReadBufferFromUser( sim808_t* pSim808 )
{


	uint32_t counter_timer = GetTimer(pSim808);
	char dataFromUsart = 0;
	bool flag = false ;

#ifndef SIM808_DEBUGGER
	while( (( GetTimer(pSim808) - counter_timer ) < TIME_TO_WAIT_SIM_RESPONSE )  )

#else
	while( NElemensFromBufferSim( pSim808  ) )
#endif
	{
		dataFromUsart = ElementFromSim( pSim808 );

		if( dataFromUsart != '\r' && dataFromUsart != '\n' && flag != true )
		{
			flag = true ;
		}
		else
		{
			char* pFindCarryAReturn = NULL ;

			pFindCarryAReturn = strstr( ( char* )pSim808->simBuffer , "\n\r");

			if( pFindCarryAReturn == NULL )
			{
				WRITE_BUFFER( pSim808->simBuffer , &dataFromUsart );
			}
			else
			{
				break;
			}




		}
/*
		// read all elements from user simModule and wait
		while( NElemensFromBufferSim( pSim808  ))
		{
			dataFromUsart = ElementFromSim( pSim808 );

			WRITE_BUFFER( pSim808->simBuffer , &dataFromUsart );

			counter_timer = GetTimer( pSim808 );
		}
*/

	}
}

int8_t FindPather( const char* pDataToFind  , AtCommand_t command )
{
	char* pThereElement = NULL;

	const AtEnumEvent_t* pEvents = ResponseMessage( command );

	do
	{
		pThereElement = strstr( pDataToFind , EventAtCommand( *pEvents) );

	}while( *pEvents++ && pThereElement != NULL );

	if( pThereElement )
	{
		pEvents--;

		return *pEvents;
	}

	return SIM_ERROR ;
}

int8_t SendAndWait()
{
	return 0 ;
}

void sim808Init()
{

}

sim808_t* Sim808InitDynamic( txPrint_t* pTxParameter , rxBuffer_t* pRxParameter )
{
	sim808_t* pTempSim = NULL ;

	buffer_t* pTempBuffer = NULL;

	pTempSim = (sim808_t*)malloc(sizeof(sim808_t));

	pTempBuffer = CreateBuffer( SIZE_BUFFER_SIM808 , sizeof( char ) );

	pTempSim->simBuffer = pTempBuffer ; //sim808 point Buffer


/*
	pTempSim->txFunction = pTxParameter ;

	pTempSim->rxFunciton = pRxParameter ;
*/
	return pTempSim ;

}

void ResetBufferUser(  sim808_t* pSim808 )
{
	ResetBuffer( pSim808->simBuffer );
}


typedef enum
{
	GET_PATTHER ,
	GET_END_PATTHER ,
	OUT_SIMPLE_STATE_MACHINE
}machineCasesSimple_t ;

int16_t SendAndResponse( sim808_t* pSim808 , AtCommand_t commandToSim )
{

	machineCasesSimple_t currenteCase = GET_PATTHER ;



	char* pCommandPosition  = NULL ;
	char* pFinalString = NULL;


	const AtEnumEvent_t* pResponse = ResponseMessage( commandToSim );

	int16_t CurrentResponse = SIM_ERROR ;

	SendCommand( pSim808 , commandToSim );


	while( currenteCase != OUT_SIMPLE_STATE_MACHINE )
	{
		ReadBufferFromUser( pSim808 );

		switch( currenteCase )
		{
			case GET_PATTHER :
				uint8_t itrEvetnCommadn = 0 ;

				for( itrEvetnCommadn = 0 ; pResponse[ itrEvetnCommadn ] != '\0' ; itrEvetnCommadn++ )
				{

					pCommandPosition = strstr( (char*)PBufferPointer(pSim808->simBuffer) , EventAtCommand( pResponse[ itrEvetnCommadn ] )) ;

					if( pCommandPosition != NULL)
					{
						currenteCase = GET_END_PATTHER;

						CurrentResponse = pResponse[ itrEvetnCommadn ];
						break;
					}

				}


				break;

			case GET_END_PATTHER:


				pFinalString = strstr( (char*)PBufferPointer(pSim808->simBuffer) , EventAtCommand( carryAndReturn )) ;

				if( pFinalString != NULL)
				{
					currenteCase = OUT_SIMPLE_STATE_MACHINE ;
				}

				break;

				//just in case
			default:

		}
	}




	return CurrentResponse ;
}

typedef enum
{
	PARAMETER_FROM_MESSAGE ,
	MESSAGE_ITSELFT ,
	EVENT_RESPONSE_MESSAGE
}message_parts_t;

enum
{
	DATE_FILD = 0  ,
	TIME_FILD ,
	PHONE_NUMBER_FILD
};

// +CMGR: "REC READ","xxxxxxxxxxxxx","","25/02/10,14:12:27-24"{0D}{0A}Hola{0D}{0A}{0D}{0A}OK{0D}{0A}
//return last position in string if find nothing return null

char* DataBetweenCharacter( char* pData , char* pDataToCopy , char* pFirstElemnt , char* pSecondElemnt )
{
	uint8_t itrTempString = 0 ;
	uint8_t itrPathers = 0 ;
	char* pFirstEndString[ 2 ] = { NULL , NULL }; // save first and second position
	char* pElemntsToFind [ ] = { pFirstElemnt , pSecondElemnt };// itr through

	while( (pData[ itrTempString ] != '\0') && (pData[ 1 + itrTempString ] != '\0') && (itrPathers != 2)  )
	{
		if( pData[ itrTempString++ ] == *pElemntsToFind[ itrPathers ] )
		{

			if(itrPathers == 1)
			{
				itrTempString -=2;
			}


			pFirstEndString[ itrPathers++ ] = &pData[ itrTempString ];
			//pFirstEndString[ itrPathers++ ] = &pData[ itrTemp ];

		}
	}

	if( itrPathers == 2)
	{
		size_t nElementsPosition =  pFirstEndString[1] - pFirstEndString[ 0 ] ;

		strncpy( pDataToCopy , pFirstEndString[ 0 ] , nElementsPosition + 1 );


		return pFirstEndString[ 1 ] + 1;
	}
	else
	{
		return NULL ;
	}
}

char* PFindnPositionIndex( char* pData , char* pElement , uint8_t nIndex )
{
	uint8_t itrString = 0 ;
	uint8_t counterMatch = 0 ;
	while( pData[ itrString ] != '\0'  && counterMatch != nIndex )
	{
		if( pData[ itrString++ ] ==  *pElement ) counterMatch++;
	}

	return ( counterMatch == nIndex )? &pData[ --itrString ] : NULL;
}

// +CMGR: "REC READ","xxxxxxxxxxxxx","","25/02/10,14:12:27-24"{0D}{0A}Hola{0D}{0A}{0D}{0A}OK{0D}{0A}
bool GetDateTimeAndPhone( char* pData , messageParts_t* pMessage  )
{
	uint8_t nComma = 0 ;
	uint8_t itrTrama = 0 ;
	bool flagTramaOk = true ;

	char* pParameterA[ ] ={ [ DATE_FILD ]pMessage->Date , [TIME_FILD]pMessage->Time , [ PHONE_NUMBER_FILD ]pMessage->PhoneNumber };

	STRING_T( tempData , 100 );

	strcpy( tempData.pBuffer , pData ) ;

	for( itrTrama = 0 ; tempData.pBuffer [ itrTrama ] != '\0' ; itrTrama++)
	{
		if( tempData.pBuffer [ itrTrama ] == ',') nComma++ ;
	}

	if( 4 == nComma)
	{
		char* pTrgBuffer = NULL ;

		pTrgBuffer = PFindnPositionIndex( tempData.pBuffer , "\"" , 3 );

		DataBetweenCharacter( pTrgBuffer  , pParameterA[ PHONE_NUMBER_FILD ] ,"\"" ,"\"" );

		pTrgBuffer = PFindnPositionIndex( tempData.pBuffer , "\"" , 7 );

		pTrgBuffer = DataBetweenCharacter( pTrgBuffer  , pParameterA[ DATE_FILD ] ,"\"" ,"," );

		pTrgBuffer = DataBetweenCharacter( pTrgBuffer  , pParameterA[ TIME_FILD ] ,"," ,"\"" );


	}
	else flagTramaOk = false;

	return flagTramaOk ;
}


uint8_t ReadMessageFromSim808( sim808_t* pSim808 , uint8_t indexMessage , messageParts_t* pMessage )
{
	STRING_T( messageIndex , 15 );
	int8_t statusSim = 0 ;


	EmptBufferSim808( pSim808 );

	sprintf( messageIndex.pBuffer , "%i" , indexMessage );

	SendCommandWhitParameter( pSim808 , readGsm , messageIndex.pBuffer);

	uint32_t saveTime = GetTimer( pSim808 ) ;

	while(( GetTimer( pSim808 ) - saveTime) < TIME_TO_WAIT_SIM_RESPONSE * 4)
	{
		//read and save in the buffer
		ReadBufferFromUser( pSim808 );
		statusSim = FindPather( ( char* )GetSimBufferPointer( pSim808 ) , readGsm );

		//if find the message will go into function
		if( statusSim == OKMESSAGE)
		{
			bool tempFlag = true ;
			//return parameter from messegger
			tempFlag = GetDateTimeAndPhone( ( char* )GetSimBufferPointer( pSim808 ) , pMessage );

			if( tempFlag )
			{
				char* temFinalLocation  = NULL ;
				char* tempStarLocation = (char* )GetSimBufferPointer( pSim808 ) ;

				ResetBufferUser( pSim808 );
				ReadBufferFromUser( pSim808 );

				temFinalLocation  = strstr( tempStarLocation , "\r\n" ) ;

				if( temFinalLocation != NULL)
				{
					uint32_t numberOfElementToCpy = tempStarLocation - temFinalLocation ;

					strncpy( pMessage->Message , tempStarLocation , numberOfElementToCpy ) ;

					break ;
				}

			}
		}
		else if( statusSim == ok || statusSim == error ) break;

	}


	return statusSim ;
}



bool SectionString(  const char* pString , const char* pInitString , const char* pFinalString , char* pPositionInit ,char* pFinalPosition )
{
	pPositionInit = strstr( pString , pInitString );
	return true;

}


void sendMessage(sim808_t* pPhones ,char* pMessage , char* pPhoneNumber  )
{

}

uint32_t GetTimer( sim808_t* pSim808 )
{
	//value set it in milliseconds
	return HAL_GetTick();
}




void ResetMessageParts( messageParts_t* ptempPartesMessage )
{
	memset( ptempPartesMessage , '\0' , sizeof(*ptempPartesMessage));
}














