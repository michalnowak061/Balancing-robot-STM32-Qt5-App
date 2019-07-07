#include "communicationwindow.h"
#include "ui_communicationwindow.h"

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::Disconnect_Slot()
{
    this->show();
    on_pushButton_Disconnect_clicked();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CommunicationWindow::CommunicationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommunicationWindow)
{
    ui->setupUi(this);

    CommunicationWindow_set_default();

    // CONNECT:
    connect(this->device, SIGNAL( readyRead() ), this, SLOT( readFromPort() ));
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CommunicationWindow::~CommunicationWindow()
{
    delete ui;
    delete device;
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Search_clicked()
{
    this->CommunicationWindow_addToLogs("Szukam urządzeń...");

    QList<QSerialPortInfo> devices;
    devices = QSerialPortInfo::availablePorts();

    for(int i = 0; i < devices.count(); i++) {

        this->CommunicationWindow_addToLogs("Znalazłem urządzenie: " + devices.at(i).portName() + " " + devices.at(i).description());
        ui->comboBox_Devices->addItem(devices.at(i).portName() + "\t" + devices.at(i).description());
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::CommunicationWindow_addToLogs(QString message)
{
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    ui->textBrowser_Logs->append(currentDateTime + "\t" + message);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::CommunicationWindow_sendMessageToDevice(QString message)
{
    if( this->device->isOpen() && this->device->isWritable() ) {

        this->CommunicationWindow_addToLogs("Wysyłam informacje do urządzenia " + message);
        this->device->write(message.toStdString().c_str());
    }
    else {

        this->CommunicationWindow_addToLogs("Nie mogę wysłać wiadomości. Port nie jest otwarty!");
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox messageBox(QMessageBox::Question,
                           tr("BBot"),
                           tr("Czy na pewno chcesz zakończyć ? \n"),
                           QMessageBox::Yes | QMessageBox::No);

    messageBox.setButtonText(QMessageBox::Yes, tr("Tak"));
    messageBox.setButtonText(QMessageBox::No,  tr("Nie"));

    if(messageBox.exec() != QMessageBox::Yes) {

        event->ignore();
    }
    else {

        exit(0);
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Connect_clicked()
{
    if(ui->comboBox_Devices->count() == 0) {

      this->CommunicationWindow_addToLogs("Nie wykryto żadnych urządzeń!");
      return;
    }

    QString portName = ui->comboBox_Devices->currentText().split("\t").first();
    this->device->setPortName(portName);

    // OTWÓRZ I SKONFIGURUJ PORT:
    if(!device->isOpen()) {

        if( device->open(QSerialPort::ReadWrite) ) {

            this->device->setBaudRate(baud);
            this->device->setDataBits(bits);
            this->device->setParity(parity);
            this->device->setStopBits(stop);
            this->device->setFlowControl(control);

            this->CommunicationWindow_addToLogs("Otwarto port szeregowy.");
        }
        else {

            this->CommunicationWindow_addToLogs("Otwarcie portu szeregowego się nie powiodło!");
        }
    }
    else {

        this->CommunicationWindow_addToLogs("Port już jest otwarty!");
        return;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Disconnect_clicked()
{
    if(this->device->isOpen()) {

        this->device->close();
        this->CommunicationWindow_addToLogs("Zamknięto połączenie.");
    }
    else {

        this->CommunicationWindow_addToLogs("Port nie jest otwarty!");
        return;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::readFromPort()
{
    while(this->device->canReadLine()) {

        QString line = this->device->readLine();
        //qDebug() << line;

        QString terminator = "\r";
        int pos = line.lastIndexOf(terminator);
        //qDebug() << line.left(pos);

        this->CommunicationWindow_addToLogs(line.left(pos));
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Send1_clicked()
{
    CommunicationWindow_sendMessageToDevice("1");
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Send2_clicked()
{
    CommunicationWindow_sendMessageToDevice("0");
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Clear_clicked()
{
    ui->textBrowser_Logs->clear();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::CommunicationWindow_set_default()
{
    on_comboBox_Baud_currentIndexChanged(2);
    on_comboBox_Bits_currentIndexChanged(3);
    on_comboBox_Parity_currentIndexChanged(0);
    on_comboBox_Stop_currentIndexChanged(0);
    on_comboBox_Control_currentIndexChanged(0);

    ui->comboBox_Baud->setCurrentIndex(2);
    ui->comboBox_Bits->setCurrentIndex(3);
    ui->comboBox_Parity->setCurrentIndex(0);
    ui->comboBox_Stop->setCurrentIndex(0);
    ui->comboBox_Control->setCurrentIndex(0);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_comboBox_Baud_currentIndexChanged(int index)
{
    switch(index) {

        case 0:
            baud = QSerialPort::Baud1200;
            break;

        case 1:
            baud = QSerialPort::Baud2400;
            break;

        case 2:
            baud = QSerialPort::Baud4800;
            break;

        case 3:
            baud = QSerialPort::Baud9600;
            break;

        case 4:
            baud = QSerialPort::Baud19200;
            break;

        case 5:
            baud = QSerialPort::Baud38400;
            break;

        case 6:
            baud = QSerialPort::Baud57600;
            break;

        case 7:
            baud = QSerialPort::Baud115200;
            break;

        case 8:
            baud = QSerialPort::UnknownBaud;
            break;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_comboBox_Bits_currentIndexChanged(int index)
{
    switch(index) {

        case 0:
            bits = QSerialPort::Data5;
            break;

        case 1:
            bits = QSerialPort::Data6;
            break;

        case 2:
            bits = QSerialPort::Data7;
            break;

        case 3:
            bits = QSerialPort::Data8;
            break;

        case 4:
            bits = QSerialPort::UnknownDataBits;
            break;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_comboBox_Parity_currentIndexChanged(int index)
{
    switch(index) {

        case 0:
            parity = QSerialPort::NoParity;
            break;

        case 1:
            parity = QSerialPort::EvenParity;
            break;

        case 2:
            parity = QSerialPort::OddParity;
            break;

        case 3:
            parity = QSerialPort::MarkParity;
            break;

        case 4:
            parity = QSerialPort::SpaceParity;
            break;

        case 5:
            parity = QSerialPort::UnknownParity;
            break;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_comboBox_Stop_currentIndexChanged(int index)
{
    switch(index) {

        case 0:
            stop = QSerialPort::OneStop;
            break;

        case 1:
            stop = QSerialPort::TwoStop;
            break;

        case 2:
            stop = QSerialPort::StopBits::OneAndHalfStop;
            break;

        case 3:
            stop = QSerialPort::StopBits::UnknownStopBits;
            break;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_comboBox_Control_currentIndexChanged(int index)
{
    switch(index) {

        case 0:
            control = QSerialPort::FlowControl::NoFlowControl;
            break;

        case 1:
            control = QSerialPort::FlowControl::HardwareControl;
            break;

        case 2:
            control = QSerialPort::FlowControl::SoftwareControl;
            break;

        case 3:
            control = QSerialPort::FlowControl::UnknownFlowControl;
            break;
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Cancel_clicked()
{
    exit(0);
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CommunicationWindow::on_pushButton_Continue_clicked()
{
    if( device->isOpen() ) {

        this->hide();
        emit Connection_OK_Signal();
    }
    else {

        QMessageBox messageBox(QMessageBox::Information,
                               tr("BBot"),
                               tr("Najpierw połącz się z urządzeniem"),
                               QMessageBox::Ok);

        messageBox.exec();
    }
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
