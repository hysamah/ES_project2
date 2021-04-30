#include "main.h"
#include "bno055.h"
#include "utils.h"
#include <stdlib.h>

#define IGNORE_FLEX 1
// #define IGNORE_FLEX 0
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

char driver_msg[] = {0xCA, 0x00, 0xC2, 0x00};

void command_dagu(void)
{
		uint16_t flex_val, pitch, roll, speed_common;
		bno055_vector_t v;
		int direction;
		flex_val = read_flex();
	
		if(flex_val < 2000 || IGNORE_FLEX == 1)	// Read the IMU and move DAGU
		{
				v = read_imu();
				pitch = abs((int)v.y);
				roll = abs((int)v.z);
			
				// Prioritize forward over rotate
				if(pitch > 5) // Threshold magnitude to consider motion
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
		}
		else // Stop Dagu
		{
				driver_msg[1] = 0;
				driver_msg[3] = 0;
		}	
		HAL_UART_Transmit(&huart1, (uint8_t*)driver_msg, sizeof(driver_msg), HAL_MAX_DELAY);
}
