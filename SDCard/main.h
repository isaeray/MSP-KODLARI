/*
 * main.h
 *
 *  Created on: 06 Þub 2014
 *      Author: Asus
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <msp430F5438a.h>
#include "UART.h"
#include "pulsar.h"
#include "rena.h"
#include "ADC.h"
#include "ByteFunctions.h"
#include "AD9243.h"
#include "Timer.h"

#define CLOCK_DELAY 1000
void delay(unsigned int );


void ADCACQTimerInterrupt();
void CommandVector(unsigned char Phase, unsigned char* Data, unsigned int Length);

enum UART_Mode {HeaderError, DataError, Success, NoOp};

/*!\fn void UARTCommandHandler(UART_Mode mode)
 * \brief This function handles the received command and is regularly called in a while loop.
 *
 * Processes the received data and responds to the commands accordingly.
 * \param mode This parameter should be set to CurrentUARTMode.
 */
void UARTCommandHandler(UART_Mode mode);




#endif /* MAIN_H_ */
