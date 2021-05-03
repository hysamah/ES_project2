#include "stdio.h" 
#include "main.h"

volatile uint8_t data_root[30];
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
volatile int alligned = 0;
volatile int data_index = 0;

volatile char asciiMsg[6];
volatile uint8_t polulu_message[4];

void send()
{
		//output for teraterm
		HAL_UART_Transmit (&huart2, "\n \r", sizeof("\n \r"),5);
		for (int q=0; q<sizeof(polulu_message); q++) 
		{
			snprintf(asciiMsg, sizeof(asciiMsg), "%u", polulu_message[q]);
			HAL_UART_Transmit (&huart2, (uint8_t*)asciiMsg, sizeof(asciiMsg), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit (&huart2, "\n \r", sizeof("\n \r"),5);
	
		//HAL_UART_Transmit (&huart2, motion, sizeof(motion), HAL_MAX_DELAY);   //output for dagu (don't forget setting the baudrate)
}

void dagu_digest(void)
{
		//uint8_t polulu_message[4];
		volatile int i = 0, conv_index = 0, tp_index = 0;
		uint8_t	byte;
		char to_convert[4];
		i = 0;
		conv_index = 0;
		tp_index = 0;
		while(data_root[i] != 'T')
		{
				if(data_root[i] == '\n')
				{
						to_convert[conv_index] = '\0';
						sscanf(to_convert, "%u", &byte);
						polulu_message[tp_index] = byte;
						tp_index++;
						conv_index = 0;
				}
				else
				{
						to_convert[conv_index] = data_root[i];
						conv_index++;
				}
				
				i++;
		}
		send();
}

void handle_byte(void)
{
		__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
		uint8_t data;
//		HAL_UART_Receive (&huart1, &data, sizeof(data), HAL_MAX_DELAY);
//		HAL_UART_Transmit (&huart2, &data, sizeof(data),5);
//		HAL_UART_Transmit (&huart2, "\n \r", sizeof("\n \r"),5);
		if(data != '\0')
		{
				if(alligned == 1)
				{
						data_root[data_index] = data;
						data_index++;
				}
				
				if(data_index == 3)
				{
						if(data_root[0] != '2' || data_root[1] !=  '0')
						{
								alligned = 0;
								data_index = 0;
						}
				}
				
				if(data == 'T' && alligned == 1)
				{
						alligned = 0;
						data_index = 0;
						dagu_digest();
				}
			
				if(data == 'A')
				{
						alligned = 1;
				}
		}
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}
