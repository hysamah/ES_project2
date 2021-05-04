#include "main.h"
#include "bno055.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

volatile char driver_msg[] = {0xCA, 0x00, 0xC2, 0x00};
char init_msg[] = {'A'};
char end_msg[] = {'T'};
char term_val[] = {'\n'};
char endl[] = {'\r', '\n'};
char asciiMsg[3];
uint8_t byte;
char fixed_msg[18];
uint8_t fbyte;


void command_dagu(void)
{
		int i;
		volatile uint16_t flex_val, pitch, roll, speed_common;
		volatile bno055_vector_t v;
		volatile short direction;
		flex_val = read_flex();
		fixed_msg[0]=init_msg[0];
	
	
		if(flex_val < 500)	// Read the IMU and move DAGU
		{
				v = read_imu();
				pitch = abs((int)v.y);
				roll = abs((int)v.z);
			
				// Prioritize forward over rotate
				if(pitch > 5 && pitch > roll) // Threshold magnitude to consider motion
				{
						direction = (v.y > 0);
						speed_common = MIN((pitch / 90.0) * 127, 127);
						driver_msg[1] = speed_common;
						driver_msg[3] = speed_common;
						if(direction)
						{
								driver_msg[0] = 0xCA; 
								driver_msg[2] = 0xC2;
						}
						else
						{	
								driver_msg[0] = 0xC9; 
								driver_msg[2] = 0xC1;
						}
				}
				else if (roll > 5) // if no forward check rotation
				{
						direction = (v.z > 0);
						speed_common = MIN((roll / 90.0) * 127, 127);
						driver_msg[1] = speed_common;
						driver_msg[3] = speed_common;
						if(direction)
						{
								driver_msg[0] = 0xCA; 
								driver_msg[2] = 0xC1;
						}
						else
						{	
								driver_msg[0] = 0xC9; 
								driver_msg[2] = 0xC2;
						}
				}
				else 
				{
						driver_msg[0] = 0xCA; 
						driver_msg[2] = 0xC1;
						driver_msg[1] = 0;
						driver_msg[3] = 0;
					
				}
		}
		else // Stop Dagu
		{
				driver_msg[1] = 0;
				driver_msg[3] = 0;
		}	
		
		
		sprintf(&fixed_msg[1], " %03u %03u %03u %03u", driver_msg[0], driver_msg[1], driver_msg[2], driver_msg[3]);
		HAL_UART_Transmit(&huart1, (uint8_t*)fixed_msg, strlen(fixed_msg), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)fixed_msg, strlen(fixed_msg), HAL_MAX_DELAY);
		
}
