/*
 * mpu9250.c
 *
 *  Created on: 04.07.2019
 *  Author: mnowak5
 */

#include "mpu9250.h"

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Accelerometer_Configuration(I2C_HandleTypeDef *I2Cx,
													   struct MPU9250 *DataStructure,
													   MPU9250_Acce_range Range) {

	uint8_t Byte_temp = 0x00;

	/* Case 1: Set accelerometer sensitivity range */
	Byte_temp = Range << 3;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_ACCEL_CONFIG, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		return MPU9250_Accelerometer_Config_FAIL;
	}

	/* Case 2: Set accelerometer low pass filter cut-off frequency */
	/*
	Byte_temp = 0x0E;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_ACCEL_CONFIG_2, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		return MPU9250_Accelerometer_Config_FAIL;
	}
	*/

	/* Case 3: Save configuration to data structure */
	if(      Range == MPU9250_Acce_2G )     DataStructure->Accelerometer_sensitivity_factor = MPU9250_ACCE_SENSITIVITY_FACTOR_2G;
	else if( Range == MPU9250_Acce_4G )		DataStructure->Accelerometer_sensitivity_factor = MPU9250_ACCE_SENSITIVITY_FACTOR_4G;
	else if( Range == MPU9250_Acce_8G )		DataStructure->Accelerometer_sensitivity_factor = MPU9250_ACCE_SENSITIVITY_FACTOR_8G;
	else if( Range == MPU9250_Acce_16G )	DataStructure->Accelerometer_sensitivity_factor = MPU9250_ACCE_SENSITIVITY_FACTOR_16G;


	return MPU9250_Accelerometer_Config_OK;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Gyroscope_Configuration(I2C_HandleTypeDef *I2Cx,
												   struct MPU9250 *DataStructure,
												   MPU9250_Gyro_range Range) {

	uint8_t Byte_temp = 0x00;


	/* Case 1: Set gyroscope sensitivity range */
	Byte_temp = Range << 3;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_GYRO_CONFIG, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		return MPU9250_Gyroscope_Config_FAIL;
	}

	/* Case 2: Set gyroscope low pass filter cut-off frequency */
	Byte_temp = 0x0E;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_CONFIG, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		return MPU9250_Gyroscope_Config_FAIL;
	}

	/* Case 3: Save configuration to data structure */
	if(      Range == MPU9250_Gyro_250s )   DataStructure->Gyroscope_sensitivity_factor = MPU9250_GYRO_SENSITIVITY_FACTOR_250s;
	else if( Range == MPU9250_Gyro_500s )	DataStructure->Gyroscope_sensitivity_factor = MPU9250_GYRO_SENSITIVITY_FACTOR_500s;
	else if( Range == MPU9250_Gyro_1000s )	DataStructure->Gyroscope_sensitivity_factor = MPU9250_GYRO_SENSITIVITY_FACTOR_1000s;
	else if( Range == MPU9250_Gyro_2000s )	DataStructure->Gyroscope_sensitivity_factor = MPU9250_GYRO_SENSITIVITY_FACTOR_2000s;


	return MPU9250_Gyroscope_Config_OK;

}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Magnetometer_Configuration(I2C_HandleTypeDef *I2Cx,
												      struct MPU9250 *DataStructure) {


	uint8_t Byte_temp = 0x00;
	uint8_t Bytes_temp[3] = {0};

	DataStructure->Magnetometer_addres = 0x0C << 1;
	DataStructure->Magnetometer_sesitivity_factor = 0.1499; /* 4912/32768 */

	// Case 2: Disable the I2C master interface
	Byte_temp = 0x00;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_USER_CTRL, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	// Case 3: Enable the bypass multiplexer
	Byte_temp = 0x02;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_INT_PIN_CFG, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	// Case 1: Is device connected ?
	if( HAL_I2C_IsDeviceReady(I2Cx, DataStructure->Magnetometer_addres, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	// Case 2: Who am i test
	if( HAL_I2C_Mem_Read(I2Cx, DataStructure->Magnetometer_addres, AK9863_WIA, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Init_FAIL;
	}

	if( Byte_temp != 0x48 ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Init_FAIL;
	}

	// Case 4: Setup to fuse ROM access mode and 16-bit output
	Byte_temp = 0x1F;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Magnetometer_addres, AK9863_CNTL1, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	HAL_Delay(100);

	// Case 5: Read from the fuse ROM sensitivity adjustment values
	if( HAL_I2C_Mem_Read(I2Cx, DataStructure->Magnetometer_addres, AK9863_ASAX | 0x80, 1, Bytes_temp, 3, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	DataStructure->Magnetometer_ASAX = ( ( (Bytes_temp[0] - 128) * 0.5 ) / 128 ) + 1;
	DataStructure->Magnetometer_ASAY = ( ( (Bytes_temp[1] - 128) * 0.5 ) / 128 ) + 1;
	DataStructure->Magnetometer_ASAZ = ( ( (Bytes_temp[2] - 128) * 0.5 ) / 128 ) + 1;

	// Case 6: Reset to power down mode
	Byte_temp = 0x00;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Magnetometer_addres, AK9863_CNTL1, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	// Case 7: Enable continuous mode 2 and 16-bit output
	Byte_temp = 0x16; // 0x16

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Magnetometer_addres, AK9863_CNTL1, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return MPU9250_Magnetometer_Config_FAIL;
	}

	HAL_Delay(100);

	return MPU9250_Magnetometer_Config_OK;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Init(I2C_HandleTypeDef *I2Cx,
								struct MPU9250 *DataStructure,
								MPU9250_Device_number Number,
								MPU9250_Acce_range Acce_range,
								MPU9250_Gyro_range Gyro_range) {

	uint8_t Byte_temp = 0x00;

	DataStructure->Device_number = Number;
	DataStructure->Device_addres = (0x68 | DataStructure->Device_number) << 1;

	/* Case 1: Is device connected ? */
	if( HAL_I2C_IsDeviceReady(I2Cx, DataStructure->Device_addres, 1, 1000) != HAL_OK ) {

		return MPU9250_Init_FAIL;
	}

	/* Case 2: Who am i test */
	if( HAL_I2C_Mem_Read(I2Cx, DataStructure->Device_addres, MPU9250_WHO_AM_I, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		return MPU9250_Init_FAIL;
	}

	if( Byte_temp != 0x71 ) {

		return MPU9250_Init_FAIL;
	}

	/* Case 3: Wake up */
	Byte_temp = 0x01;

	if( HAL_I2C_Mem_Write(I2Cx, DataStructure->Device_addres, MPU9250_PWR_MGMT_1, 1, &Byte_temp, 1, 1000) != HAL_OK ) {

		return MPU9250_Init_FAIL;
	}

	/* Case 4: Accelerometer configuration */
	if( MPU9250_Accelerometer_Configuration(I2Cx, DataStructure, Acce_range) != MPU9250_Accelerometer_Config_OK ) {

		return MPU9250_Accelerometer_Config_FAIL;
	}

	/* Case 5: Gyroscope configuration */
	if( MPU9250_Gyroscope_Configuration(I2Cx, DataStructure, Gyro_range) != MPU9250_Gyroscope_Config_OK ) {

		return MPU9250_Gyroscope_Config_FAIL;
	}

	/* Case 6: Magnetometer configuration */
	if( MPU9250_Magnetometer_Configuration(I2Cx, DataStructure) != MPU9250_Magnetometer_Config_OK ) {

		return MPU9250_Magnetometer_Config_FAIL;
	}

	return MPU9250_Init_OK;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Read_Accelerometer(I2C_HandleTypeDef *I2Cx,
											  struct MPU9250 *DataStructure) {

	uint8_t Bytes_temp[6] = {0x00};

	if( HAL_I2C_Mem_Read(I2Cx, DataStructure->Device_addres, MPU9250_ACCEL_XOUT_H, 1, Bytes_temp , 6, 1000) != HAL_OK ) {

		return MPU9250_Read_Accelerometer_FAIL;
	}

	DataStructure->Accelerometer_X = Z_AXIS_ORIENTATION *( ( Bytes_temp[0] << 8 | Bytes_temp[1] ) - DataStructure->Accelerometer_X_offset );
	DataStructure->Accelerometer_Y = Z_AXIS_ORIENTATION *( ( Bytes_temp[2] << 8 | Bytes_temp[3] ) - DataStructure->Accelerometer_Y_offset );
	DataStructure->Accelerometer_Z = Z_AXIS_ORIENTATION *( ( Bytes_temp[4] << 8 | Bytes_temp[5] ) - DataStructure->Accelerometer_Z_offset );

	/* Case x: Calculate g-force values for XYZ axis */
	DataStructure->Accelerometer_X_g = (float)(DataStructure->Accelerometer_X) / DataStructure->Accelerometer_sensitivity_factor;
	DataStructure->Accelerometer_Y_g = (float)(DataStructure->Accelerometer_Y) / DataStructure->Accelerometer_sensitivity_factor;
	DataStructure->Accelerometer_Z_g = (float)(DataStructure->Accelerometer_Z) / DataStructure->Accelerometer_sensitivity_factor;

	return MPU9250_Read_Accelerometer_OK;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Read_Gyroscope(I2C_HandleTypeDef *I2Cx,
										  struct MPU9250 *DataStructure) {

	uint8_t Bytes_temp[6] = { 0x00 };

	if (HAL_I2C_Mem_Read(I2Cx, DataStructure->Device_addres, MPU9250_GYRO_XOUT_H, 1, Bytes_temp, 6, 1000) != HAL_OK) {

		return MPU9250_Read_Gyroscope_FAIL;
	}

	DataStructure->Gyroscope_X = ( Bytes_temp[0] << 8 | Bytes_temp[1] ) - DataStructure->Gyroscope_X_offset;
	DataStructure->Gyroscope_Y = ( Bytes_temp[2] << 8 | Bytes_temp[3] ) - DataStructure->Gyroscope_Y_offset;
	DataStructure->Gyroscope_Z = ( Bytes_temp[4] << 8 | Bytes_temp[5] ) - DataStructure->Gyroscope_Z_offset;

	/* Case x: Calculate dgs/s values for XYZ axis */
	DataStructure->Gyroscope_X_dgs = (float)(DataStructure->Gyroscope_X) / DataStructure->Gyroscope_sensitivity_factor;
	DataStructure->Gyroscope_Y_dgs = (float)(DataStructure->Gyroscope_Y) / DataStructure->Gyroscope_sensitivity_factor;
	DataStructure->Gyroscope_Z_dgs = (float)(DataStructure->Gyroscope_Z) / DataStructure->Gyroscope_sensitivity_factor;

	return MPU9250_Read_Gyroscope_OK;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

MPU9250_Error_code MPU9250_Read_Magnetometer(I2C_HandleTypeDef *I2Cx,
										     struct MPU9250 *DataStructure) {

	uint8_t Bytes_temp[8] = { 0x00 };

	/* Case x: Read measured values from registers */
	if( HAL_I2C_Mem_Read(I2Cx, DataStructure->Magnetometer_addres, AK9863_ST1, 1, Bytes_temp, 8, 1000) != HAL_OK ) {

		return MPU9250_Read_Magnetometer_FAIL;
	}

	if( Bytes_temp[0] & 0x00 ) {

		return MPU9250_Read_Magnetometer_FAIL;
	}

	DataStructure->Magnetometer_X = ( Bytes_temp[2] << 8 | Bytes_temp[1] ) - DataStructure->Magnetometer_X_offset;
	DataStructure->Magnetometer_Y = ( Bytes_temp[4] << 8 | Bytes_temp[3] ) - DataStructure->Magnetometer_Y_offset;
	DataStructure->Magnetometer_Z = ( Bytes_temp[6] << 8 | Bytes_temp[5] ) - DataStructure->Magnetometer_Z_offset;

	/* Case x: Calculate uT (micro Tesla) value for XYZ axis */
	DataStructure->Magnetometer_X_uT = DataStructure->Magnetometer_X * DataStructure->Magnetometer_ASAX * DataStructure->Magnetometer_sesitivity_factor;
	DataStructure->Magnetometer_Y_uT = DataStructure->Magnetometer_Y * DataStructure->Magnetometer_ASAY * DataStructure->Magnetometer_sesitivity_factor;
	DataStructure->Magnetometer_Z_uT = DataStructure->Magnetometer_Z * DataStructure->Magnetometer_ASAZ * DataStructure->Magnetometer_sesitivity_factor;

	return MPU9250_Read_Magnetometer_OK;
}

/* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

void MPU9250_Calibration_Acce(I2C_HandleTypeDef *I2Cx,
	      	  	  	  	  	  	  	        struct MPU9250 *DataStructure) {

	float Acce_X_offset = 0, Acce_Y_offset = 0, Acce_Z_offset = 0;

	for (int i = 0; i < 1000; ++i) {

		MPU9250_Read_Accelerometer(I2Cx, DataStructure);

		Acce_X_offset = Acce_X_offset + DataStructure->Accelerometer_X;
		Acce_Y_offset = Acce_Y_offset + DataStructure->Accelerometer_Y;
		Acce_Z_offset = Acce_Z_offset + DataStructure->Accelerometer_Z;
	}

	DataStructure->Accelerometer_X_offset = Z_AXIS_ORIENTATION
			* (Acce_X_offset / 1000);
	DataStructure->Accelerometer_Y_offset = Z_AXIS_ORIENTATION
			* (Acce_Y_offset / 1000);
	DataStructure->Accelerometer_Z_offset = Z_AXIS_ORIENTATION
			* (Acce_Z_offset / 1000
					- DataStructure->Accelerometer_sensitivity_factor);

}

/* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

void MPU9250_Calibration_Gyro(I2C_HandleTypeDef *I2Cx,
	      	  	  	  	  	  	  	        struct MPU9250 *DataStructure) {

	float Gyro_X_offset = 0, Gyro_Y_offset = 0, Gyro_Z_offset = 0;

	for (int i = 0; i < 1000; ++i) {

		MPU9250_Read_Gyroscope(I2Cx, DataStructure);

		Gyro_X_offset += DataStructure->Gyroscope_X;
		Gyro_Y_offset += DataStructure->Gyroscope_Y;
		Gyro_Z_offset += DataStructure->Gyroscope_Z;
	}

	DataStructure->Gyroscope_X_offset = Gyro_X_offset / 1000;
	DataStructure->Gyroscope_Y_offset = Gyro_Y_offset / 1000;
	DataStructure->Gyroscope_Z_offset = Gyro_Z_offset / 1000;

}

/* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

void MPU9250_Calibration_Mag(I2C_HandleTypeDef *I2Cx,
	      	  	  	  	  	  	  	        struct MPU9250 *DataStructure) {

	float X_max = 0, X_min = 0, Y_max = 0, Y_min = 0, Z_max = 0, Z_min = 0;

	for (int i = 0; i < 1000; ++i) {

		MPU9250_Read_Magnetometer(I2Cx, DataStructure);

		if (DataStructure->Magnetometer_X > X_max)
			X_max = DataStructure->Magnetometer_X;
		if (DataStructure->Magnetometer_Y > Y_max)
			Y_max = DataStructure->Magnetometer_Y;
		if (DataStructure->Magnetometer_Z > Z_max)
			Z_max = DataStructure->Magnetometer_Z;

		if (DataStructure->Magnetometer_X < X_min)
			X_min = DataStructure->Magnetometer_X;
		if (DataStructure->Magnetometer_Y < Y_min)
			Y_min = DataStructure->Magnetometer_Y;
		if (DataStructure->Magnetometer_Z < Z_min)
			Z_min = DataStructure->Magnetometer_Z;

		HAL_Delay(20);
	}

	DataStructure->Magnetometer_X_offset = (X_max + X_min) / 2;
	DataStructure->Magnetometer_Y_offset = (Y_max + Y_min) / 2;
	DataStructure->Magnetometer_Z_offset = (Z_max + Z_min) / 2;
}

/* ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

void MPU9250_Set_Offsets(I2C_HandleTypeDef *I2Cx,
	      	  	  	  	  	  	  	   struct MPU9250 *DataStructure,
									   float Acce_X_offset, float Acce_Y_offset, float Acce_Z_offset,
									   float Gyro_X_offset, float Gyro_Y_offset, float Gyro_Z_offset,
									   float Mag_X_offset, float Mag_Y_offset, float Mag_Z_offset) {

	DataStructure->Accelerometer_X_offset = Acce_X_offset;
	DataStructure->Accelerometer_Y_offset = Acce_Y_offset;
	DataStructure->Accelerometer_Z_offset = Acce_Z_offset;

	DataStructure->Gyroscope_X_offset = Gyro_X_offset;
	DataStructure->Gyroscope_Y_offset = Gyro_Y_offset;
	DataStructure->Gyroscope_Z_offset = Gyro_Z_offset;

	DataStructure->Magnetometer_X_offset = Mag_X_offset;
	DataStructure->Magnetometer_Y_offset = Mag_Y_offset;
	DataStructure->Magnetometer_Z_offset = Mag_Z_offset;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

void MPU9250_Calculate_RPY(I2C_HandleTypeDef *I2Cx,
	      	  	  	  	  	  	  		 struct MPU9250 *DataStructure,
										 float dt) {

	/* Case 1: */
	MPU9250_Read_Accelerometer(I2Cx, DataStructure);
	MPU9250_Read_Gyroscope(I2Cx, DataStructure);
	MPU9250_Read_Magnetometer(I2Cx, DataStructure);

	/* Case 2: Calculate accelerometer Roll and Pitch */
	DataStructure->Accelerometer_Roll  = atan2f(DataStructure->Accelerometer_Y_g, DataStructure->Accelerometer_Z_g) * (180 / M_PI);
	DataStructure->Accelerometer_Pitch = atan2f(DataStructure->Accelerometer_X_g, DataStructure->Accelerometer_Z_g) * (180 / M_PI);

	/* Case 3: Calculate gyroscope Roll, Pitch and Yaw */
	DataStructure->Gyroscope_Roll  += ( 0.5 * dt * (DataStructure->Gyroscope_X_dgs + DataStructure->Gyroscope_X_dgs_past) );
	DataStructure->Gyroscope_Pitch -= ( 0.5 * dt * (DataStructure->Gyroscope_Y_dgs + DataStructure->Gyroscope_Y_dgs_past) );
	DataStructure->Gyroscope_Yaw   += Z_AXIS_ORIENTATION * ( 0.5 * dt * (DataStructure->Gyroscope_Z_dgs + DataStructure->Gyroscope_Z_dgs_past) );

	/* Save actual dgs/s value to data structure */
	DataStructure->Gyroscope_X_dgs_past = DataStructure->Gyroscope_X_dgs;
	DataStructure->Gyroscope_Y_dgs_past = DataStructure->Gyroscope_Y_dgs;
	DataStructure->Gyroscope_Z_dgs_past = DataStructure->Gyroscope_Z_dgs;

	/* Case 4: Calculate magnetometer Yaw */
	int16_t m_y = DataStructure->Magnetometer_X;
	int16_t m_x = DataStructure->Magnetometer_Y;
	int16_t m_z = DataStructure->Magnetometer_Z;

	float Roll  = -DataStructure->Accelerometer_Roll  * (M_PI / 180);
	float Pitch = DataStructure->Accelerometer_Pitch * (M_PI / 180);

	float cosRoll = cos(Roll), cosPitch = cos(Pitch);
	float sinRoll = sin(Roll), sinPitch = sin(Pitch);

	float X_h = m_x * cosPitch + m_y * sinRoll * sinPitch + m_z * cosRoll * sinPitch;
	float Y_h = m_y * cosRoll  - m_z * sinRoll;

	DataStructure->Magnetometer_Yaw = Z_AXIS_ORIENTATION * (atan2f(X_h, Y_h) * (180 / M_PI) /*+ MAGNETIC_DECLINATION*/);

	/* Case 5: Clear gyro drift */
	if( fabs(DataStructure->Gyroscope_Roll  - DataStructure->Accelerometer_Roll)  > 10 ||
		fabs(DataStructure->Gyroscope_Pitch - DataStructure->Accelerometer_Pitch) > 10 ||
		fabs(DataStructure->Gyroscope_Yaw   - DataStructure->Magnetometer_Yaw)    > 10 ) {

		DataStructure->Gyroscope_Roll  = DataStructure->Accelerometer_Roll;
		DataStructure->Gyroscope_Pitch = DataStructure->Accelerometer_Pitch;
		DataStructure->Gyroscope_Yaw   = DataStructure->Magnetometer_Yaw;
	}

	/* Case 6: Clear magnetometer high tilt */
	if( fabs(DataStructure->Accelerometer_Roll) >= 80 || fabs(DataStructure->Accelerometer_Pitch) >= 80 ) {

		DataStructure->Magnetometer_Yaw = DataStructure->Gyroscope_Yaw;
	}

	/* Case 7: Clear accelerometer high tilt */
	if( fabs(DataStructure->Accelerometer_Roll) >= 80 ) {

		DataStructure->Accelerometer_Pitch = DataStructure->Gyroscope_Pitch;
	}

	if( fabs(DataStructure->Accelerometer_Pitch) >= 80 ) {

		DataStructure->Accelerometer_Roll = DataStructure->Gyroscope_Roll;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

void AlphaBeta_filter(struct MPU9250 *DataStructure,
					  float Acce_alpha, float Acce_beta,
					  float Gyro_alpha, float Gyro_beta,
					  float Mag_alpha, float Mag_beta,
					  float dt) {

	/* Case 1: Accelerometer */
	DataStructure->x_Acce_Roll_pri  = DataStructure->x_Acce_Roll_post + dt * DataStructure->v_Acce_Roll_post;
	DataStructure->v_Acce_Roll_pri  = DataStructure->v_Acce_Roll_post;
	DataStructure->x_Acce_Roll_post = DataStructure->x_Acce_Roll_pri + Acce_alpha * (DataStructure->Accelerometer_Roll - DataStructure->x_Acce_Roll_pri);
	DataStructure->v_Acce_Roll_post = DataStructure->v_Acce_Roll_pri + Acce_beta  * (DataStructure->Accelerometer_Roll - DataStructure->x_Acce_Roll_pri) / dt;

	DataStructure->x_Acce_Pitch_pri  = DataStructure->x_Acce_Pitch_post + dt * DataStructure->v_Acce_Pitch_post;
	DataStructure->v_Acce_Pitch_pri  = DataStructure->v_Acce_Pitch_post;
	DataStructure->x_Acce_Pitch_post = DataStructure->x_Acce_Pitch_pri + Acce_alpha * (DataStructure->Accelerometer_Pitch - DataStructure->x_Acce_Pitch_pri);
	DataStructure->v_Acce_Pitch_post = DataStructure->v_Acce_Pitch_pri + Acce_beta  * (DataStructure->Accelerometer_Pitch - DataStructure->x_Acce_Pitch_pri) / dt;

	/* Case 2: Gyroscope */
	DataStructure->x_Gyro_Roll_pri  = DataStructure->x_Gyro_Roll_post + dt * DataStructure->v_Gyro_Roll_post;
	DataStructure->v_Gyro_Roll_pri  = DataStructure->v_Gyro_Roll_post;
	DataStructure->x_Gyro_Roll_post = DataStructure->x_Gyro_Roll_pri + Gyro_alpha * (DataStructure->Gyroscope_Roll - DataStructure->x_Gyro_Roll_pri);
	DataStructure->v_Gyro_Roll_post = DataStructure->v_Gyro_Roll_pri + Gyro_beta  * (DataStructure->Gyroscope_Roll - DataStructure->x_Gyro_Roll_pri) / dt;

	DataStructure->x_Gyro_Pitch_pri  = DataStructure->x_Gyro_Pitch_post + dt * DataStructure->v_Gyro_Pitch_post;
	DataStructure->v_Gyro_Pitch_pri  = DataStructure->v_Gyro_Pitch_post;
	DataStructure->x_Gyro_Pitch_post = DataStructure->x_Gyro_Pitch_pri + Gyro_alpha * (DataStructure->Gyroscope_Pitch - DataStructure->x_Gyro_Pitch_pri);
	DataStructure->v_Gyro_Pitch_post = DataStructure->v_Gyro_Pitch_pri + Gyro_beta  * (DataStructure->Gyroscope_Pitch - DataStructure->x_Gyro_Pitch_pri) / dt;

	DataStructure->x_Gyro_Yaw_pri  = DataStructure->x_Gyro_Yaw_post + dt * DataStructure->v_Gyro_Yaw_post;
	DataStructure->v_Gyro_Yaw_pri  = DataStructure->v_Gyro_Yaw_post;
	DataStructure->x_Gyro_Yaw_post = DataStructure->x_Gyro_Yaw_pri + Gyro_alpha * (DataStructure->Gyroscope_Yaw - DataStructure->x_Gyro_Yaw_pri);
	DataStructure->v_Gyro_Yaw_post = DataStructure->v_Gyro_Yaw_pri + Gyro_beta  * (DataStructure->Gyroscope_Yaw - DataStructure->x_Gyro_Yaw_pri) / dt;

	/* Case 3: Magnetometer */
	DataStructure->x_Mag_Yaw_pri  = DataStructure->x_Mag_Yaw_post + dt * DataStructure->v_Mag_Yaw_post;
	DataStructure->v_Mag_Yaw_pri  = DataStructure->v_Mag_Yaw_post;
	DataStructure->x_Mag_Yaw_post = DataStructure->x_Mag_Yaw_pri + Mag_alpha * (DataStructure->Magnetometer_Yaw - DataStructure->x_Mag_Yaw_pri);
	DataStructure->v_Mag_Yaw_post = DataStructure->v_Mag_Yaw_pri + Mag_beta  * (DataStructure->Magnetometer_Yaw - DataStructure->x_Mag_Yaw_pri) / dt;

	DataStructure->Acce_AlphaBeta_Roll = DataStructure->x_Acce_Roll_post;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

void Complementary_filter(struct MPU9250 *DataStructure,
						  float weight,
						  float dt) {

	DataStructure->Complementary_filter_Roll   = ( (0.99) * (DataStructure->Complementary_filter_Roll  + (DataStructure->Gyroscope_X_dgs * dt) )     + (0.01   * DataStructure->Accelerometer_Roll)  );
	DataStructure->Complementary_filter_Pitch  = ( (1-weight) * (DataStructure->Complementary_filter_Pitch - (DataStructure->Gyroscope_Y_dgs * dt) ) + (weight * DataStructure->Accelerometer_Pitch) );
	DataStructure->Complementary_filter_Yaw    = ( (0.9) * (DataStructure->Complementary_filter_Yaw  + (DataStructure->Gyroscope_Z_dgs * dt) )  + (0.1   * DataStructure->Magnetometer_Yaw) );
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

void Kalman_filter(struct MPU9250 *DataStructure,
				   float Q, float R,
				   float dt) {

	/* Case 1: Update Q and R value */
	if( DataStructure->Kalman_P.kalman_Q != Q || DataStructure->Kalman_P.kalman_R != R ) {

		Kalman_filter_init(&DataStructure->Kalman_P, Q, R);

		Q = 0.0001; R = 10000;
		Kalman_filter_init(&DataStructure->Kalman_Y, Q, R);

		Q = 0.000001; R = 0.001;
		Kalman_filter_init(&DataStructure->Kalman_R, Q, R);


		return;
	}

	/* Case 2: */
	DataStructure->Kalman_filter_Roll  = Kalman_filter_calculate(&DataStructure->Kalman_R, DataStructure->Accelerometer_Roll,   DataStructure->Gyroscope_X_dgs, dt);
	DataStructure->Kalman_filter_Pitch = Kalman_filter_calculate(&DataStructure->Kalman_P, DataStructure->Accelerometer_Pitch, -DataStructure->Gyroscope_Y_dgs, dt);
	DataStructure->Kalman_filter_Yaw   = Kalman_filter_calculate(&DataStructure->Kalman_Y, DataStructure->Magnetometer_Yaw,    -DataStructure->Gyroscope_Z_dgs, dt) /*- MAGNETIC_DECLINATION*/;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

void Madgwick_filter(struct MPU9250 *DataStructure,
					 float beta,
					 float dt) {

	MadgwickAHRSupdateIMU(beta,
			 	 	 	  DataStructure->Gyroscope_X_dgs * (M_PI / 180), DataStructure->Gyroscope_Y_dgs * (M_PI / 180), -DataStructure->Gyroscope_Z_dgs * (M_PI / 180),
					      DataStructure->Accelerometer_X_g, DataStructure->Accelerometer_Y_g, DataStructure->Accelerometer_Z_g,
						  dt);

	float real_part_ag = q0;
	float x_ag = q1;
	float y_ag = q2;
	float z_ag = q3;

	float Roll  = atan2f( 2 * ( (real_part_ag * x_ag) + (y_ag * z_ag) ), 1 - (2 * ( (x_ag * x_ag) + (y_ag * y_ag) ) ) ) * (180 / M_PI);
	float Pitch = -asinf( 2 * ( (real_part_ag * y_ag) - (z_ag * x_ag) ) ) * (180 / M_PI);


	MadgwickAHRSupdate(1,
					   DataStructure->Gyroscope_X_dgs * (M_PI / 180), DataStructure->Gyroscope_Y_dgs * (M_PI / 180), -DataStructure->Gyroscope_Z_dgs * (M_PI / 180),
					   DataStructure->Accelerometer_X_g, DataStructure->Accelerometer_Y_g, DataStructure->Accelerometer_Z_g,
					   DataStructure->Magnetometer_X_uT, DataStructure->Magnetometer_Y_uT, -DataStructure->Magnetometer_Z_uT,
					   dt);

	float real_part = q0_m;
	float x =  q1_m;
	float y =  q2_m;
	float z =  q3_m;

	float Yaw   = atan2f( 2 * ( (real_part * z) + (x * y) ), 1 - (2 * ( (y * y) + (z * z) ) ) ) * (180 / M_PI);

	DataStructure->Madgwick_filter_Roll  = Roll;
	DataStructure->Madgwick_filter_Pitch = Pitch;
	DataStructure->Madgwick_filter_Yaw   = Yaw;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */
