/*
 * a4988.h
 *
 *  Created on: 23.04.2019
 *  Author: Artur
 */

#ifndef A4988_H_
#define A4988_H_

#include "gpio.h"
#include "math.h"

/*************************************************************************************************
 *****************				> A4988_Result type definition <			     *****************
 *************************************************************************************************/
typedef enum {

	A4988_Status_Ok		=	0,
	A4988_Status_Error  =	1,

} A4988_Status;

/*************************************************************************************************
 *****************			  > A4988_Resolution type definition <			     *****************
 *************************************************************************************************/
typedef enum {

	A4988_Full_step   = 1,
	A4988_One_2_step  = 2,
	A4988_One_4_step  = 4,
	A4988_One_8_step  = 8,
	A4988_One_16_step = 16,

} A4988_Resolution;

/*************************************************************************************************
 *****************			  > A4988_Direction type definition <			     *****************
 *************************************************************************************************/
typedef enum {

	A4988_Left  = 0,
	A4988_Right = 1,

} A4988_Direction;

/*************************************************************************************************
 *****************					  > A4988 structure <				         *****************
 *************************************************************************************************/
struct A4988 {

	GPIO_TypeDef *SD_EN_Port;
	uint16_t	  SD_EN_Pin;

	GPIO_TypeDef *SD_STEP_Port;
	uint16_t 	  SD_STEP_Pin;

	GPIO_TypeDef *SD_DIR_Port;
	uint16_t 	  SD_DIR_Pin;

	GPIO_TypeDef *SD_MS1_Port;
	uint16_t	  SD_MS1_Pin;
	GPIO_TypeDef *SD_MS2_Port;
	uint16_t	  SD_MS2_Pin;
	GPIO_TypeDef *SD_MS3_Port;
	uint16_t	  SD_MS3_Pin;

	long int 	Resolution;
	double      Step_angle;
	long int    Steps_per_round;
	long int	Speed_RPM;
	long int 	Step_delay;

	uint32_t current_micros;
	uint32_t previous_micros;

	long int actual_speed;

	int soft_start;
	long double soft_start_ratio;
};

/* ************************************************************************************************************** */

void Micros_Init();

/* ************************************************************************************************************** */

uint32_t Get_Micros();

/*************************************************************************************************
 *****************					> A4988 initialization <			         *****************
 *************************************************************************************************/
A4988_Status A4988_Init(struct A4988 *a4988,
						GPIO_TypeDef *SD_EN_Port,	uint16_t SD_EN_Pin,
						GPIO_TypeDef *SD_STEP_Port, uint16_t SD_STEP_Pin,
						GPIO_TypeDef *SD_DIR_Port,  uint16_t SD_DIR_Pin,
						GPIO_TypeDef *SD_MS1_port,  uint16_t SD_MS1_Pin,
						GPIO_TypeDef *SD_MS2_port,  uint16_t SD_MS2_Pin,
						GPIO_TypeDef *SD_MS3_port,  uint16_t SD_MS3_Pin);

/*************************************************************************************************
 *****************				> A4988 set step resolution <			         *****************
 *************************************************************************************************/
A4988_Status A4988_Set_Resolution(struct A4988 *a4988, A4988_Resolution resolution);

/*************************************************************************************************
 *****************				> A4988 set rotate direction <			         *****************
 *************************************************************************************************/
A4988_Status A4988_Set_Direction(struct A4988 *a4988, A4988_Direction direction);

A4988_Status A4988_Set_Speed(struct A4988 *a4988, int speed_rpm);

A4988_Status A4988_Power_on(struct A4988 *a4988);
A4988_Status A4988_Power_off(struct A4988 *a4988);

/*************************************************************************************************
 *****************					> A4988 make one step <			             *****************
 *************************************************************************************************/
A4988_Status A4988_One_Step(struct A4988 *a4988);

/*************************************************************************************************
 *****************					> A4988 make move <			             	 *****************
 *************************************************************************************************/
A4988_Status A4988_Move(struct A4988 *a4988, double speed);


#endif /* A4988_H_ */
