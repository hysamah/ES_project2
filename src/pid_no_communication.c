#include "stdio.h" 
#include "main.h"
#include "pid_controller.h"
#include <string.h>
#include <stdlib.h>
#include "bno055.h"

int yaw_set_point = 0;
volatile int yaw = 0;

extern PIDControl dagu_pid_controller;
extern int pid_interval_counter;
extern UART_HandleTypeDef huart2;

float pid_output;
int err, err_magnitude;
int flag_direction = 0;

uint8_t dagu_msg[4] = {0xCA,0x00,0xC2,0x00};

void fix_me_with_pid(void)
{
		pid_interval_counter = 0;
		PIDSetpointSet(&dagu_pid_controller, yaw_set_point);
		
		yaw = bno055_getVectorEuler().x;
		err_magnitude = abs(yaw_set_point - yaw); 
		if(err_magnitude > 180) err_magnitude = 360 - err_magnitude; 
	
		err = -err_magnitude;
		PIDInputSet(&dagu_pid_controller, err);
		PIDCompute(&dagu_pid_controller); 
		pid_output = PIDOutputGet(&dagu_pid_controller);
	
		if((yaw - yaw_set_point < 0 && yaw - yaw_set_point >=-180) || yaw - yaw_set_point > 180)
		{
				flag_direction = 0; // I am on the left of the set point
		}
		else
		{
				flag_direction = 1; // I am on the right of the set point
		}	
		
		// Here we are moving forward and we apply PID error correction
		if(pid_output > 0)
		{
				if (flag_direction == 1)
				{
					dagu_msg[0] = 0xCA;
					dagu_msg[1] = pid_output;
					dagu_msg[2] = 0xC1;
					dagu_msg[3] = pid_output;
				}
				else 
				{
					dagu_msg[0] = 0xC9;
					dagu_msg[1] = pid_output;
					dagu_msg[2] = 0xC2;
					dagu_msg[3] = pid_output;
				}
		}
		else
		{
				dagu_msg[1] = 0;
				dagu_msg[3] = 0;
		}
		
		HAL_UART_Transmit(&huart2, (unsigned char *)dagu_msg, sizeof(dagu_msg), HAL_MAX_DELAY);
}