#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QThread>
#include <QDebug>
#include <QSerialPort>
#include <QTimer>
#include <QMessageBox>

#include "math.h"

#define DATA_FRAME_FROM_ROBOT_SIZE      31
#define DATA_FRAME_TO_ROBOT_SIZE        26

#define POLYNOMIAL_9	0x31

typedef int8_t byte;

typedef enum {

    Open_connection_OK      =   1,
    Open_connection_FAIL    =   2,

    Close_connection_OK     =   3,
    Close_connection_FAIL   =   4,

    Port_is_busy            =   5,

    TimeoutError            =   6,

} Status_Codes;

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int16_t Merge_bytes(uint8_t _lower_byte, uint8_t _higher_byte);
int8_t Divide_bytes(int16_t data, uint8_t which_byte);

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Data_from_Robot
{
    double Lipol_voltage;   // <- in volts

    double Filter_roll, Filter_pitch, Filter_yaw;

    int Left_engine_speed, Right_engine_speed;

    double g_x_dgs, g_y_dgs, g_z_dgs;
    double a_x_g, a_y_g, a_z_g;
    double m_x_uT, m_y_uT, m_z_uT;
};

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Data_to_Robot
{
    // Angle PID data
    double Angle_Kp, Angle_Ki, Angle_Kd;

    // Speed PID data
    double Speed_Kp, Speed_Ki, Speed_Kd;

    // Filters data
    double Complementary_filter_weight;
    double Kalman_procces_variance;
    double Madgwick_filter_beta;

    // Engines speed data
    int Left_engine_speed, Right_engine_speed;

    // Additional
    int Emergency_stop;
    int Which_filter;
};

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class Bluetooth : public QThread
{
    Q_OBJECT

private:

    QSerialPort *Device = new QSerialPort;

    int8_t Data_frame_from_robot[DATA_FRAME_FROM_ROBOT_SIZE];

    Data_from_Robot DF_Robot;
    Data_to_Robot   DT_Robot;

    void Parse_data_frame();

    //int Error_frame_counter;
    double Lipol_voltage_past;
    double Lipol_voltage_actual;

    QTimer *TimeoutTimer;
public:

    Bluetooth();
    ~Bluetooth();

    void Open_connection(QString portName);
    void Close_connection();

    void Set_DT_Robot(Data_to_Robot Data);
    Data_from_Robot Get_DF_Robot();

private slots:

public slots:

    void Receive_frame();
    void Send_frame();
    void Timeout_Test();

signals:

    void Serial_Interface_Signal(Status_Codes);
    void Parsed_frame_OK_Signal();
};

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // BLUETOOTH_H
