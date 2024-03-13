/*
 * ADC_Driver.c
 *
 *  Created on: 11-Mar-2024
 *      Author: Kumar Harsh
 */

/*********************************************************************
 * Includes
 *********************************************************************/
#include "ADC_Driver.h"

/*********************************************************************
 * Variables Declarations & Definitions
 *********************************************************************/

/*********************************************************************
 * Function Definitions
 *********************************************************************/
/**
 * Brief : Re-maps a number from one range to another range.
 * Param : 	value: the number to map.
 * 		in_min : the lower bound of the value current range.
 * 		in_max : the upper bound of the value current range.
 * 		out_min : the lower bound of the value target range.
 * 		out_max : the upper bound of the value target range.
 * RetVal : (float) The mapped value.
 */
float map(long x, long in_min, long in_max, long out_min, long out_max)
{
	float mapped = 0.0;
	mapped = ((float)((x-in_min)*(out_max-out_min)/(float)(in_max-in_min)) + out_min);
	return mapped;
}

