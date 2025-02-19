/*
 * sim808.h
 *
 *  Created on: Dec 24, 2024
 *      Author: Jose Rodolfo
 */

#ifndef INC_SIM808_H_
#define INC_SIM808_H_

#include <string.h>
#include "myprint.h"
#include "main.h" //optional library
#include "FifoBuffer.h"
#include "stdbool.h"


#define SIM808_DEBUGGER


#define SIZE_BUFFER_SIM808 300 // sizeBuffer Sim Module if need more data chance this parameter

#define TIME_TO_WAIT_SIM_RESPONSE 50 //time that wait for data

#define SIM_ERROR 			-1

#define SIM_OK 				-2

#define NUMBER_OF_MESSAGE_SAVED 30 // Check datasheet for this number

//use this function instead
#define SEND_TO_SIM_MODULE( USER_ARGUMENT_FILD ,ARGUMENT_FILD , ...) pSim808->txFunction.Transmit( USER_ARGUMENT_FILD , ARGUMENT_FILD ,  __VA_ARGS__ )


typedef void ( *PtxPrintf_t)(void* pParameters , const char *fmt , ...  );
typedef char ( *PrxInputUser_t )( void* pParameters );
typedef uint16_t ( *PxThereElemnt_t )( void* pParameters );


typedef enum{
    TurnOnGps = 0,
	TurnOFFGps,
    checkStateRequesStatus,
    GpStatus ,
    RequestNema,
    turnoffRequestNema,
    ReadGps,
    SetModeMessage,
    ATGSM,					/*send Message*/
    DeletElement,
	readGsm,
	At,						//this value must be the second one dont chance position theelemen
	FinalCommand			//this element eigther
}AtCommand_t;


typedef enum{
	pgga = 0,
    CGPS_STATUS_OK,
    CGPSPWR_ON,
    CGPSPWR_OFF,
    CGPS_STATUS_NOTFIX,
    CGPS_STATUS_LOCATION_UNKNOW,
    CGPS_STATUS_LOCATION_2D,
    Rdy,
	ok,
    OKMESSAGE,
	error,
    asterisk,
    requestInfor,
    carryAndReturn,
	WRITE_MESSAGE,
    EVENT_IGNORE_EVENT,
	FinalEvent
}AtEnumEvent_t;


typedef struct
{
	char* pBuffer ;
	uint16_t sizeBuffer ;
}string_t;

#define STRING_T( BUFFER_NAME , N_ELEMENT )		string_t BUFFER_NAME ;\
												char BUFFER_NAME##BUFFER[ N_ELEMENT ];\
												BUFFER_NAME.pBuffer = BUFFER_NAME##BUFFER;\
												BUFFER_NAME.sizeBuffer = N_ELEMENT; \
												memset( BUFFER_NAME.pBuffer , '\0' , BUFFER_NAME.sizeBuffer )




typedef struct
{
	PtxPrintf_t Transmit ;
	void*    pUserParameters ;
} txPrint_t ;

typedef struct
{
	PrxInputUser_t 	ReadBuffer ;
	PxThereElemnt_t IsthereElement ;
	void*  		 	pUserParameters ;
} rxBuffer_t ;



typedef struct
{
	buffer_t* 	simBuffer ; // this must not implement for usertouch it,
	txPrint_t 	txFunction ;	// set parameter and function for transition Exameple Function USART
	rxBuffer_t 	rxFunciton ;	// set parameter and function for reception From Example USART
} sim808_t ;

typedef struct
{
	char PhoneNumber[ 20 ] ;
	char Time [ 12 ] ;
	char Date[ 12 ] ;

	char Message[ 300 ];
}messageParts_t ;



void ResetMessageParts( messageParts_t* ptempPartesMessage );

//cretate a pointer to variable sim808_t
sim808_t* Sim808InitDynamic( txPrint_t* pTxParameter , rxBuffer_t* pRxParameter );

void SetParametersFromStruct( sim808_t* pSim808 ); //empty function
void SetParameters( sim808_t* pSim808 );	// empty function
void SetTxPrint( sim808_t* pSim808 , PtxPrintf_t pTxFunciton , void* pUserParameter );
void SetRxBuffe( sim808_t* pSim808 , PrxInputUser_t pRxFunciton , PxThereElemnt_t pThereElement, void* pUserParameter );


void SendCommand( sim808_t* pSim808 , AtCommand_t commandToSim );

//this its only for OK , ERROR RESPONSE DON BE USED WHIT 2 OR MORE DATA RESPSONSE
int16_t SendAndResponse( sim808_t* pSim808 , AtCommand_t commandToSim );

// get laps timer from user
uint32_t GetTimer( sim808_t* pSim808 );


void ReadBufferFromUser( sim808_t* pSim808 );


int8_t FindPather( const char* pDataToFind  , AtCommand_t command );

char ElementFromSim(  sim808_t* pSim808 );

uint16_t NElemensFromBufferSim( sim808_t* pSim808  );


bool GetDateTimeAndPhone( char* pData , messageParts_t* pMessage  );


#endif /* INC_SIM808_H_ */
