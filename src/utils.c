#include "main.h"
#include "bno055.h"

extern ADC_HandleTypeDef hadc1;

uint16_t read_flex(void)
{
		uint16_t raw;

    // Get ADC value
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    raw = HAL_ADC_GetValue(&hadc1);
	
		return raw;
}

bno055_vector_t read_imu()
{
	bno055_vector_t c = bno055_getVectorEuler();
	return c;
}

