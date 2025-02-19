/*
 * FifoBuffer.c
 *
 *  Created on: Dec 17, 2024
 *      Author: Lenovo
 */

#include "FifoBuffer.h"



void* PBufferPointer(  buffer_t* pSBuffer )
{
	return pSBuffer->pBuffer;
}



static void  ResetParameters(); // used when init parameter must not used for user code

void* ReturnCurrentPositionBuffer( buffer_t* pParameters  , uint8_t itrType ); 	// return current position from circular buffer
void IncIterator( itrBuffer_t* pIterator , const itrBuffer_t  limitIterator  ); // increment iterator variable not available for user

void DecIterator( itrBuffer_t* pIterator  );


void InitBufferNoDynamic(  buffer_t* pSBuffer , void* pBuffer , sizeBuffer numberOfElements  ,size_t kindBuffer )
{


	pSBuffer->pBuffer = pBuffer ;
	pSBuffer->typeVariable = kindBuffer ;
	pSBuffer->Parameters[ SIZE_BUFFER ] = numberOfElements ;

	ResetBuffer( pSBuffer );
}


void ResetBuffer( buffer_t* pSBuffer )
{
	memset( pSBuffer->pBuffer , '\0' , (pSBuffer->Parameters[ SIZE_BUFFER ]) * pSBuffer->typeVariable );


	pSBuffer->Parameters[ WRITE_ITERATOR ] = 0 ;
	pSBuffer->Parameters[ READ_ITERATOR  ] = 0 ;
	pSBuffer->Parameters[ NUMBER_OF_ELEMENTS  ] = 0 ;
}

buffer_t* CreateBuffer(sizeBuffer numberOfElements ,size_t kindBuffer )
{

	void* pBuffer = malloc( numberOfElements * kindBuffer);//create N Element from ARRAY

	buffer_t* pStructTemporal = (buffer_t*)malloc( sizeof( buffer_t )); // create a struct

	ResetParameters( pStructTemporal->Parameters ); //reset Parameter to use

	pStructTemporal->pBuffer = pBuffer; // save Buffer
	pStructTemporal->Parameters[ SIZE_BUFFER ] = numberOfElements - 1;
	pStructTemporal->typeVariable = kindBuffer ;


	return pStructTemporal ;
}

void WriteBuffer( buffer_t* pSBuffer , void* pElement )
{

	memcpy( ReturnCurrentPositionBuffer( pSBuffer , pSBuffer->Parameters[WRITE_ITERATOR]  ) , pElement ,  pSBuffer->typeVariable );

	IncIterator( &pSBuffer->Parameters[ WRITE_ITERATOR ] , pSBuffer->Parameters[ SIZE_BUFFER ]);
	IncIterator( &pSBuffer->Parameters[ NUMBER_OF_ELEMENTS ] , pSBuffer->Parameters[ SIZE_BUFFER ]);

}

void* ReadBuffer( buffer_t* pSBuffer )
{
	void* pTemporalData = NULL ;

	//save pointer variable for buffer
	pTemporalData = ReturnCurrentPositionBuffer( pSBuffer , pSBuffer->Parameters[ READ_ITERATOR ] );

	IncIterator( &pSBuffer->Parameters[ READ_ITERATOR ] , pSBuffer->Parameters[ SIZE_BUFFER ]);
	DecIterator( &pSBuffer->Parameters[ NUMBER_OF_ELEMENTS ] );

	return pTemporalData;
}

void* ReturnCurrentPositionBuffer( buffer_t* pParameters  , uint8_t itrType )
{

	void* pCurrentPosition = NULL;
	uint8_t*	pCurretnBufferByte = NULL;
	uint16_t* 	pCurretnBuffer2Bytes = NULL;
	uint32_t* 	pCurretnBuffer4Bytes = NULL;
	uint64_t* 	pCurretnBuffer8Bytes = NULL;

	switch( pParameters->typeVariable )
		{
			case 1 :
				pCurretnBufferByte = (uint8_t*)pParameters->pBuffer ;
				pCurrentPosition = (void*)&pCurretnBufferByte[ itrType ];
				break;

			case 2 :
				pCurretnBuffer2Bytes = (uint16_t*)pParameters->pBuffer ;
				pCurrentPosition = (void*)&pCurretnBuffer2Bytes[ itrType ];
				break;

			case 4 :
				pCurretnBuffer4Bytes = (uint32_t*)pParameters->pBuffer ;
				pCurrentPosition = (void*)&pCurretnBuffer4Bytes[itrType ];
				break;

			case 8 :
				pCurretnBuffer8Bytes = (uint64_t*)pParameters->pBuffer ;
				pCurrentPosition = (void*)&pCurretnBuffer8Bytes[ itrType ];

			default:
				//posible Code For Structs being Careful

		}
	return pCurrentPosition;
}


void IncIterator( itrBuffer_t* pIterator , const itrBuffer_t  limitIterator  )
{

	(  *pIterator >= limitIterator ) ? *pIterator = 0 : (*pIterator)++;

}

void DecIterator( itrBuffer_t* pIterator  )
{

	(  *pIterator == 0 ) ? *pIterator = 0 : (*pIterator)--;

}

void ResetParameters(itrBuffer_t* pParameters )
{
	uint8_t itrParameters = 0 ;

	for( itrParameters = 0 ; itrParameters < LAST_PARAMETER ; itrParameters++ )
	{
		*pParameters++ = 0;
	}
}


void EraseBuffer( buffer_t* pParameters )
{
	free( pParameters->pBuffer );
	free( pParameters );
}



itrBuffer_t NumberOfElemnts( buffer_t* pParameters)
{
	return pParameters->Parameters[ NUMBER_OF_ELEMENTS ] ;
}


void writeStringToBuffer( buffer_t* pSBuffer , char* pStringToBuffer )
{
	size_t nElemnt = strlen(pStringToBuffer);
	size_t itrElement = 0 ;


	for( itrElement = 0 ; itrElement < nElemnt ; itrElement++ )
	{
		WRITE_BUFFER( pSBuffer , &pStringToBuffer[ itrElement ] );

	}


}



