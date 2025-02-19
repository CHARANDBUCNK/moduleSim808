/*
 * simpruebe.h
 *
 *  Created on: Feb 1, 2025
 *      Author: Lenovo
 */

#ifndef INC_SIMPRUEBE_H_
#define INC_SIMPRUEBE_H_

#include "sim808.h"
#include "FifoBuffer.h"



void pruebeprintFunction(void* pParameters , const char *fmt , ...  );


void sim808pruebe( UART_HandleTypeDef* pParameter );

void simFunctionsPruebe( UART_HandleTypeDef* pParameter );

#endif /* INC_SIMPRUEBE_H_ */
