/*
 * a4988.c
 *
 *  Created on: 23.04.2019
 *      Author: Michal Nowak
 */

#include "a4988.h"

/* ************************************************************************************************************** */

void Micros_Init() {

	if(!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {

		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

/* ************************************************************************************************************** */

uint32_t Get_Micros() {

	return DWT->CYCCNT / (SystemCoreClock / 1000000);
}

/* ************************************************************************************************************** */
A4988_Status A4988_Init(struct A4988 *a4988,
						GPIO_TypeDef *SD_EN_Port,	uint16_t SD_EN_Pin,
						GPIO_TypeDef *SD_STEP_Port, uint16_t SD_STEP_Pin,
						GPIO_TypeDef *SD_DIR_Port,  uint16_t SD_DIR_Pin,
						GPIO_TypeDef *SD_MS1_port,  uint16_t SD_MS1_Pin,
						GPIO_TypeDef *SD_MS2_port,  uint16_t SD_MS2_Pin,
						GPIO_TypeDef *SD_MS3_port,  uint16_t SD_MS3_Pin) {

	a4988->SD_EN_Port = SD_EN_Port;
	a4988->SD_EN_Pin  = SD_EN_Pin;

	a4988->SD_STEP_Port = SD_STEP_Port;
	a4988->SD_STEP_Pin  = SD_STEP_Pin;

	a4988->SD_DIR_Port = SD_DIR_Port;
	a4988->SD_DIR_Pin  = SD_DIR_Pin;

	a4988->SD_MS1_Port = SD_MS1_port;
	a4988->SD_MS1_Pin  = SD_MS1_Pin;
	a4988->SD_MS2_Port = SD_MS2_port;
	a4988->SD_MS2_Pin  = SD_MS2_Pin;
	a4988->SD_MS3_Port = SD_MS3_port;
	a4988->SD_MS3_Pin  = SD_MS3_Pin;

	HAL_GPIO_WritePin(a4988->SD_EN_Port, a4988->SD_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(a4988->SD_STEP_Port, a4988->SD_STEP_Pin, GPIO_PIN_RESET);

	a4988->Resolution = 0;
	a4988->Step_angle = 0;
	a4988->Steps_per_round = 0;
	a4988->Speed_RPM = 0;
	a4988->Step_delay = 0;

	a4988->current_micros = 0;
	a4988->previous_micros = 0;

	a4988->actual_speed = 0;

	a4988->soft_start = 1;
	a4988->soft_start_ratio = 0;

	return A4988_Status_Ok;
}

/* ************************************************************************************************************** */
A4988_Status A4988_Set_Resolution(struct A4988 *a4988, A4988_Resolution resolution) {

	if(resolution == A4988_Full_step) {

		a4988->Resolution = A4988_Full_step;

		HAL_GPIO_WritePin(a4988->SD_MS1_Port, a4988->SD_MS1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(a4988->SD_MS2_Port, a4988->SD_MS2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(a4988->SD_MS3_Port, a4988->SD_MS3_Pin, GPIO_PIN_RESET);

		return A4988_Status_Ok;
	}

	if(resolution == A4988_One_2_step) {

		a4988->Resolution = A4988_One_2_step;

		HAL_GPIO_WritePin(a4988->SD_MS1_Port, a4988->SD_MS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(a4988->SD_MS2_Port, a4988->SD_MS2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(a4988->SD_MS3_Port, a4988->SD_MS3_Pin, GPIO_PIN_RESET);

		return A4988_Status_Ok;
	}

	if(resolution == A4988_One_4_step) {

		a4988->Resolution = A4988_One_4_step;

		HAL_GPIO_WritePin(a4988->SD_MS1_Port, a4988->SD_MS1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(a4988->SD_MS2_Port, a4988->SD_MS2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(a4988->SD_MS3_Port, a4988->SD_MS3_Pin, GPIO_PIN_RESET);

		return A4988_Status_Ok;
	}

	if(resolution == A4988_One_8_step) {

		a4988->Resolution = A4988_One_8_step;

		HAL_GPIO_WritePin(a4988->SD_MS1_Port, a4988->SD_MS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(a4988->SD_MS2_Port, a4988->SD_MS2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(a4988->SD_MS3_Port, a4988->SD_MS3_Pin, GPIO_PIN_RESET);

		return A4988_Status_Ok;
	}

	if(resolution == A4988_One_16_step) {

		a4988->Resolution = A4988_One_16_step;

		HAL_GPIO_WritePin(a4988->SD_MS1_Port, a4988->SD_MS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(a4988->SD_MS2_Port, a4988->SD_MS2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(a4988->SD_MS3_Port, a4988->SD_MS3_Pin, GPIO_PIN_SET);

		return A4988_Status_Ok;
	}

	return A4988_Status_Error;
}

/* ************************************************************************************************************** */
A4988_Status A4988_Set_Direction(struct A4988 *a4988, A4988_Direction direction) {

	if(direction == A4988_Left) {

		HAL_GPIO_WritePin(a4988->SD_DIR_Port, a4988->SD_DIR_Pin, GPIO_PIN_SET);
		return A4988_Status_Ok;
	}

	else if(direction == A4988_Right) {

		HAL_GPIO_WritePin(a4988->SD_DIR_Port, a4988->SD_DIR_Pin, GPIO_PIN_RESET);
		return A4988_Status_Ok;
	}

	return A4988_Status_Error;
}

/* ************************************************************************************************************** */
A4988_Status A4988_Set_Speed(struct A4988 *a4988, int speed_rpm) {

	const long int minute = 60000000;

	a4988->Step_angle = 1.8 / a4988->Resolution;
	a4988->Steps_per_round = 360 / a4988->Step_angle;
	a4988->Speed_RPM = speed_rpm * a4988->Steps_per_round;

	if(a4988->Speed_RPM == 0) return A4988_Status_Error;
	else a4988->Step_delay = minute / a4988->Speed_RPM;

	return A4988_Status_Ok;
}

/* ************************************************************************************************************** */
A4988_Status A4988_Power_on(struct A4988 *a4988) {

	HAL_GPIO_WritePin(a4988->SD_EN_Port, a4988->SD_EN_Pin, GPIO_PIN_RESET);
	return A4988_Status_Ok;
}

/* ************************************************************************************************************** */
A4988_Status A4988_Power_off(struct A4988 *a4988) {

	HAL_GPIO_WritePin(a4988->SD_EN_Port, a4988->SD_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(a4988->SD_STEP_Port, a4988->SD_STEP_Pin, GPIO_PIN_RESET);
	return A4988_Status_Ok;
}

/* ************************************************************************************************************** */
A4988_Status A4988_One_Step(struct A4988 *a4988) {


	a4988->current_micros = Get_Micros();

	if( a4988->current_micros - a4988->previous_micros >= a4988->Step_delay) {

		a4988->previous_micros = a4988->current_micros;
		HAL_GPIO_TogglePin(a4988->SD_STEP_Port, a4988->SD_STEP_Pin);

	}

	return A4988_Status_Ok;
}

/* ************************************************************************************************************** */

//int soft_start_test_global = 0;
//double speed_test_global = 0;

A4988_Status A4988_Move(struct A4988 *a4988, double speed) {

	//soft_start_test_global = a4988->soft_start;

	int max_value = 1200;

	if(a4988->actual_speed != speed && speed != 0) {

		// power on
		//A4988_Power_on(a4988);

		// soft start
		/*
		if( a4988->soft_start == 1) {

			speed *= a4988->soft_start_ratio;

			if(getMicros() - a4988->previous_micros > 500) {

				a4988->soft_start_ratio += 0.0005;
				a4988->previous_micros = getMicros();
			}

			if( a4988->soft_start_ratio >= 1 ) {

				a4988->soft_start = 0;
				a4988->soft_start_ratio = 0;
			}
		}
		*/

		// speed calculation
		int real_speed = fabs(speed) * (max_value / 100);

		/*
		if (real_speed >= 450)
			A4988_Set_Resolution(a4988, A4988_Full_step);
		else if (real_speed >= 300 && real_speed < 450)
			A4988_Set_Resolution(a4988, A4988_One_2_step);
		else if (real_speed >= 200 && real_speed < 300)
			A4988_Set_Resolution(a4988, A4988_One_4_step);
		else if (real_speed >= 60 && real_speed < 200)
			A4988_Set_Resolution(a4988, A4988_One_8_step);
		else if (real_speed > 0 && real_speed < 60)
			A4988_Set_Resolution(a4988, A4988_One_16_step);
		*/

		// test
		//real_speed = 25;

		A4988_Set_Resolution(a4988,A4988_One_4_step);

		if (speed > 0) {

			A4988_Set_Direction(a4988, A4988_Right);
		} else {

			A4988_Set_Direction(a4988, A4988_Left);
		}

		// set speed
		if(real_speed < 25) {

			return A4988_Status_Error;
		}

		A4988_Set_Speed(a4988, real_speed);

		// save actual speed
		a4988->actual_speed = speed;
	}

	if(speed != 0) {

		A4988_One_Step(a4988);
	}
	else {

		//A4988_Power_off(a4988);
		//a4988->soft_start = 1;
		//a4988->soft_start_ratio = 0;
	}

	return A4988_Status_Ok;
}
