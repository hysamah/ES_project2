#include "stdio.h" 
#include "main.h"
#include <string.h>

char data[16];
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern volatile int rec_flag;
volatile uint8_t dagu_message[4];


void dagu_digest(void)
{
		sscanf(&data[1], "%u %u %u %u", &dagu_message[0], &dagu_message[1], &dagu_message[2], &dagu_message[3]);
		HAL_UART_Transmit(&huart2, (unsigned char *)dagu_message, sizeof(dagu_message), HAL_MAX_DELAY);
}

void receive_data(void)
{
		volatile char allignment_byte;
		HAL_UART_Receive(&huart1, &allignment_byte, 1, 1);
		if(allignment_byte == 'A')
		{
				HAL_UART_Receive(&huart1, data, 16, HAL_MAX_DELAY);
				dagu_digest();
		}
		rec_flag = 0;
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}




