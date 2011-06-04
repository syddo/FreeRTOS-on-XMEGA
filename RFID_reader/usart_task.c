/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA USART interrupt driven driver example source.
 *
 *      This file contains an example task that demonstrates the
 *      interrupt driven USART driver. The task continiously sends 'L' bytes and
 *		listens to the port. If any data is recieved it sends it back. 
 *
 * \par Application note:
 *      AVR1307: Using the XMEGA USART
 *
 * \par Documentation
 *     http://www.FreeRTOS.org - Documentation, latest information.
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * \author
 *      Universität Erlangen-Nürnberg
 *		LS Informationstechnik (Kommunikationselektronik)
 *		Yuriy Kulikov
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/


/* Compiler definitions include file. */
#include "avr_compiler.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* File headers. */
#include "led.h"
#include "usart_driver_RTOS.h"

#include "usart_task.h"
#include <string.h>

/*! \brief Example task
 *
 *  Example task. This task configures USARTD0 for with the parameters:
 *      - 8 bit character size
 *      - No parity
 *      - 1 stop bit
 *      - 9600 Baud
 *
 *  This function then sends three bytes and tests if the received data is
 *  equal to the sent data. The code can be tested by connecting PC3 to PC2. If
 *  the variable 'success' is true at the end of the function, the three bytes
 *  have been successfully sent and received.
*/

void vUSARTTask( void *pvParameters )
{
	//do a cast t local variable, because eclipse does not provide suggestions otherwise
	USARTTaskParameters_struct_t * parameters = (USARTTaskParameters_struct_t *)pvParameters;
	//store pointer to usart for convenience
	USART_buffer_struct_t * usart_buffer_t = parameters->usartBuffer;
	char commandsBufferSize=parameters->commandsBufferSize;

	char receivedChar='#';
	char * str = (char *) pvPortMalloc( sizeof(char)*commandsBufferSize);

	/* Task loops forever*/
	for (;;)
	{
		//Continuously send
		//USART_Buffer_PutString(usart_buffer_t, "g",200);
		//We could have blocked on the queue instead, but this is better for debug
		vTaskDelay(500);


		//Empty the string first
		strcpy(str,"");
		//Read string from queue, while data is available and put it into string
		while (USART_Buffer_GetByte(usart_buffer_t, &receivedChar,0))
		{
			strncat(str,&receivedChar,1);
			//TODO need some checking in case of a really big string coming out
		}
		USART_Buffer_PutString(usart_buffer_t, str,200);
		//now check the string for content
		if (strcmp(str,"req_blink")==0)
		{
			LED_queue_put(parameters->debugLed,RED,500);
			LED_queue_put(parameters->debugLed,ORANGE,500);
			LED_queue_put(parameters->debugLed,PINK,500);
			LED_queue_put(parameters->debugLed,WHITE,500);
			USART_Buffer_PutString(usart_buffer_t,"resp_blink",200);
		}
		if (strcmp(str,"req_r_tags")==0)
		{
			//Put response to the queue, ,might wait up to 200ms if there are no place in queue
			USART_Buffer_PutString(usart_buffer_t,
					"resp_r_tags 3 0x2608198818111987 -20 0x1122334455667788 -25 0x7766554433221100 -30",200);
		}

	}

}

