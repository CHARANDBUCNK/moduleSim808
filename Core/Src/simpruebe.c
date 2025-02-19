/*
 * simpruebe.c
 *
 *  Created on: Feb 1, 2025
 *      Author: Lenovo
 */


#include "simpruebe.h"



void pruebeprintFunction(void* pParameters , const char *fmt , ...  )
{
	UART_HandleTypeDef* pUsart = ( UART_HandleTypeDef* )pParameters;

	va_list argp;
	va_start(argp, fmt);
	vprint( pUsart ,fmt, argp); // Custom line could be interrupt funcion its up to programmer

    va_end(argp);
}

uint16_t numberofElements(void* pParameters )
{
	uint16_t TemporalData ;

	buffer_t* pUserParameter = ( buffer_t* )pParameters;
	TemporalData = NumberOfElemnts( pUserParameter );

	return TemporalData ;
}

char ReadElement(void* pParameters )
{
	char tempData = '\0' ;

	buffer_t* pUserParameter = ( buffer_t* )pParameters;

	tempData = READ_BUFFER( pUserParameter , char );

	return tempData ;
}

//+CMGR: "REC READ","xxxxxxxxxxxxx","","25/02/10,14:12:27-24"{0D}{0A}Hola{0D}{0A}{0D}{0A}OK{0D}{0A}
void simFunctionsPruebe( UART_HandleTypeDef* pParameter )
{
	char Message[] = "+CMGR: \"REC READ\",\"xxxxxxxxxxxxx\",\"\",\"25/02/10,14:12:27-24\"\r\nHola\r\nOK\r\n";

	messageParts_t tempPruebe ;

	my_printf(pParameter, " testing Functions ");

	GetDateTimeAndPhone( Message ,&tempPruebe );

	my_printf(pParameter, "%s" );
}

#define ARRAY_DEBUGGER 50
void sim808pruebe( UART_HandleTypeDef* pParameter )
{
	sim808_t temporalSim;


	buffer_t hola ;

	char ArryBuffer [ ARRAY_DEBUGGER ] ;

	temporalSim.txFunction.Transmit = &pruebeprintFunction ;
	temporalSim.txFunction.pUserParameters = pParameter ;


	InitBufferNoDynamic( &hola , ArryBuffer , ARRAY_DEBUGGER , sizeof( char ) );




	temporalSim.rxFunciton.IsthereElement = numberofElements ;
	temporalSim.rxFunciton.ReadBuffer = ReadElement;
	temporalSim.rxFunciton.pUserParameters = &hola ;


	writeStringToBuffer(&hola, " hola que paso ");



	temporalSim.txFunction.Transmit( pParameter , "hola mundo de la programacion de microcontroladores \r\n");

	temporalSim.txFunction.Transmit(temporalSim.txFunction.pUserParameters,"caracter Number %i ", temporalSim.rxFunciton.IsthereElement( temporalSim.rxFunciton.pUserParameters ) );

	SendCommand(&temporalSim, turnoffRequestNema );


}
