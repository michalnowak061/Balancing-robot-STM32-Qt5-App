#include "bluetooth.h"

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Bluetooth::Bluetooth()
{

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Bluetooth::~Bluetooth()
{

}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Bluetooth::Receive_frame()
{
    //qDebug() << "Odbieram ! \n";

    emit Received_Frame_OK();

    Parse_frame();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Bluetooth::Parse_frame()
{
    //qDebug() << "Parsuję ! \n";

    emit Parsed_Frame_OK();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Bluetooth::Send_frame()
{
    //qDebug() << "Wysyłam ! \n";

    // Sending succes
    if(1) {

        Sended_Frame_OK();
    }
    // Sending fail
    else {

        Sended_Frame_FAIL();
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Bluetooth::Communication()
{
    while(1) {

        // Receiving
        Receive_frame();

        // Sending
        Send_frame();

        msleep(100);
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Bluetooth::Start_communication_thread()
{
    f = &Bluetooth::Communication;
    this->start(Priority::NormalPriority);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------