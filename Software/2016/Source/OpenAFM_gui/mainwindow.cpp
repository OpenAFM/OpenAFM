#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tx_rx_protocol.h"
#include "def_commands.h"
#include "scannerwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QLibraryInfo>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    ui->statusBar->showMessage("Qt to Arduino phone  example", 3000);
    serial = new QSerialPort(this);
    this->fillPortsInfo();
    TX_RX_qt* phone = TX_RX_qt::instance();
    phone->receive_frame_buffer.reserve(10000);

    QObject::connect(phone, SIGNAL(TX_TransmitByte(QByteArray)),
                     this, SLOT(putChar(QByteArray)));

    QObject::connect(phone, SIGNAL(TX_TransmitByte(char)),
                     this, SLOT(putChar(char)));

    QObject::connect(phone, SIGNAL(ValidFrameReceived(QByteArray, quint16)),
                     this, SLOT(phone_CommandRouter(QByteArray, quint16)));

    QObject::connect(phone, SIGNAL(ValidFrameReceived(QByteArray, quint16)),
                     this, SLOT(displayIncoming(QByteArray, quint16)));


    QObject::connect(this, SIGNAL(dataReceived(QByteArray)),
                     phone, SLOT(charReceiver(QByteArray)));

    QObject::connect(this, SIGNAL(dataReceived(QByteArray)),
                     this, SLOT(print_hex_value(QByteArray)));

    QObject::connect(this, SIGNAL(sendDataFrame(QByteArray, quint16)),
                     phone, SLOT(frameDecode(QByteArray, quint16)));

    QObject::connect(ui->pushButton_connect, SIGNAL(clicked()),
                     this, SLOT(openSerialPort()));

    QObject::connect(serial, SIGNAL(readyRead()),
                     this, SLOT(readData()));





    QObject::connect(this, SIGNAL(plotDataReceived(QByteArray)), this, SLOT(realtimeDataSlot(QByteArray)));

    ui->scanPB->setEnabled(false);
    ui->calibration_PB->setEnabled(false);
    ui->setup_pushButton->setEnabled(false);

    loadParameters();
    qDebug()<<parameters[0];
    setupStreaming(ui->customPlot);
}


MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::loadParameters(){

    lineLength=ui->line_length_spinBox->value();
    stepSize=ui->step_size_spinBox_3->value();
    sampleSize=ui->sample_size_spinBox_2->value();

    parameters={lineLength, stepSize, sampleSize};

}

void MainWindow::putChar(QByteArray data)
{
    ui->plainTextEdit_debug->appendPlainText(data.data());
    this->writeData(data);
}

void MainWindow::putChar(char data)
{
    QByteArray qdata(1, data);
    ui->plainTextEdit_debug->appendPlainText(qdata.data());
    this->writeData(qdata);
}

void MainWindow::phone_CommandRouter(QByteArray buffer, quint16 bytes_received)
{

    if(buffer==response::READY){
        qDebug()<<"RDY";
        previous_response=response::READY;
    }
    else if(buffer==response::GO){
        qDebug()<<"response::GO";
        previous_response=response::GO;
    }
    else if(buffer==response::DONE){
        qDebug()<<"response::DONE";
        previous_response=response::DONE;
    }
    else if(previous_response==response::READY && buffer!=response::READY){
        buffer.remove(buffer.size()-1,1);
        emit plotDataReceived(buffer);
    }
}


void MainWindow::sendData() {
    QByteArray data;
    data.append(ui->lineEdit_Data->text());;
    ui->lineEdit_Data->clear();
    emit sendDataFrame(data, (quint16)data.length());
}

void MainWindow::fillPortsInfo()
{
    ui->serialPortDropdown->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortDropdown->addItem(list.first(), list);
    }
}

void MainWindow::openSerialPort()
{
    serial->setPortName(ui->serialPortDropdown->currentText());
    serial->setBaudRate(ui->baudDropDown->currentText().toInt());

    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                   .arg(ui->serialPortDropdown->currentText())
                                   .arg(ui->baudDropDown->currentText().toInt())
                                   .arg(QSerialPort::Data8)
                                   .arg(QSerialPort::NoParity)
                                   .arg(QSerialPort::OneStop)
                                   .arg(QSerialPort::NoFlowControl));
        ui->pushButton_connect->setText("Disconnect");

        ui->scanPB->setEnabled(true);
        ui->calibration_PB->setEnabled(true);
        ui->setup_pushButton->setEnabled(true);

        const QSerialPortInfo info= QSerialPortInfo(*serial);
        ui->label_deviceSignature->setText(info.manufacturer());
        disconnect( ui->pushButton_connect, SIGNAL(clicked()),0, 0);
        QObject::connect(ui->pushButton_connect, SIGNAL(clicked()),
                         this, SLOT(closeSerialPort()));

    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        ui->statusBar->showMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    serial->close();
    ui->statusBar->showMessage(tr("Disconnected"));
    disconnect( ui->pushButton_connect, SIGNAL(clicked()),0, 0);
    QObject::connect(ui->pushButton_connect, SIGNAL(clicked()),
                     this, SLOT(openSerialPort()));
    ui->pushButton_connect->setText("Connect");
    ui->scanPB->setEnabled(false);
    ui->calibration_PB->setEnabled(false);
    ui->setup_pushButton->setEnabled(false);

    ui->label_deviceSignature->setText(" ");
}

void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}

void MainWindow::putData_debug(const QByteArray &data)
{
    ui->plainTextEdit_debug->appendPlainText(data.data());

}

void MainWindow::displayIncoming(QByteArray data,quint16 no_of_bytes){

    ui->plainTextEdit_input->appendPlainText(data);
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    data.replace('', "");
    data.replace('\n', "");
    data.replace('\r', "");
    data.simplified();
    int dSize = data.size();
    if (dSize > 0) {
        emit dataReceived(data);}
}


void MainWindow::sendData(QByteArray data) {
    quint16 length = data.length();
    emit sendDataFrame(data, length);
}

void MainWindow::sendReady(){
    qDebug()<<"received sig";
    serial->write(response::READY);

}

void MainWindow::sendGo(){
    qDebug()<<"received sig";
    serial->write(response::GO);
}

void MainWindow::sendDone(){
    qDebug()<<"received sig";
    serial->write(response::DONE);
}

void MainWindow::setStatusbarText(const QString& text) {
    ui->statusBar->showMessage(text);
}

void MainWindow::print_hex_value(QByteArray data) {
    QByteArray hex_console;
    QByteArray character;
    for (int i = 0; i < data.size(); ++i) {
        character.clear();
        character.append(data.at(i));
        //character = data.at(i);
        hex_console.append("0x");
        hex_console.append(character.toHex().toUpper());
        hex_console.append("\n");
    }
    ui->plainTextEdit_input_hex->appendPlainText(QString(hex_console));
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}


/* Button Slot functions */


void MainWindow::on_scanPB_clicked()
{
    scannerwindow* Scanner= new scannerwindow(parameters, this);
    ui->scanPB->setEnabled(false);
    ui->scanPB->setText("Scanning");
    ui->scanPB->setStyleSheet("QPushButton {color:white;}");


    connect(Scanner, &QObject::destroyed, [=](){
        ui->scanPB->setChecked(false);
        ui->scanPB->setEnabled(true);
        ui->scanPB->setStyleSheet("QPushButton {color:black;}");
    });
}

void MainWindow::on_pushButton_Send_clicked()
{
    sendData();
}

void MainWindow::on_pushButton_2_pressed()
{
    fillPortsInfo();
}

void MainWindow::on_lineEdit_Data_returnPressed()
{
    sendData();
}


/*Calibration tab functions*/

void MainWindow::setupStreaming(QCustomPlot *customPlot)
{

    customPlot->addGraph(); // blue line

    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    customPlot->graph(0)->setAntialiasedFill(false);

    customPlot->addGraph(); // blue dot

    customPlot->graph(1)->setPen(QPen(Qt::blue));
    customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);


    customPlot->setInteractions(QCP::iRangeDrag);
    customPlot->setInteractions(QCP::iRangeZoom);


    customPlot->xAxis->setTickLabels(false);
    customPlot->yAxis->setTickLabels(true);


    customPlot->axisRect()->setRangeDrag(Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Vertical);

    customPlot->axisRect()->setupFullAxesBox(true);
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    customPlot->setBackground(this->palette().background().color());

    connect(customPlot->yAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), [=](const QCPRange &  newRange) {
            if(newRange.lower < 0) customPlot->yAxis->setRangeLower(0);
        });
}

void MainWindow::realtimeDataSlot(QByteArray data)
{

    if(ui->calibration_PB->isChecked()){

        int value0 =data.toInt();

        double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

        // add data to line:
        ui->customPlot->graph(0)->addData(key, value0);
        // set data of dot:
        ui->customPlot->graph(1)->clearData();
        ui->customPlot->graph(1)->addData(key, value0);
        // remove data of lines that's outside visible range:
        ui->customPlot->graph(0)->removeDataBefore(key-15);

        // rescale value (vertical) axis to fit the current data:
        ui->customPlot->xAxis->setRange(key+1, 16, Qt::AlignRight);
        //ui->customPlot->graph(1)->rescaleValueAxis();

        ui->customPlot->replot();

        putChar(response::READY);

    }

}

void MainWindow::on_calibration_PB_toggled(bool checked)
{
    if(checked){
        ui->calibration_PB->setText("End");
        sendData(response::STREAM);
        sendData(response::READY);
    }
    else{
        ui->calibration_PB->setText("Calibrate");
        sendData(response::DONE);
    }
}

void MainWindow::on_setup_pushButton_clicked()
{
    loadParameters();

    QByteArray stepSize= QByteArray::number(ui->step_size_spinBox_3->value());
    QByteArray lineLength=QByteArray::number (ui->line_length_spinBox->value());
    QByteArray sampleSize=QByteArray::number (ui->sample_size_spinBox_2->value());

    sendData(response::SETUP);
    sendData(stepSize);
    sendData(response::F_BOUNDARY);
    sendData(lineLength);
    sendData(response::F_BOUNDARY);
    sendData(sampleSize);
    sendData(response::F_BOUNDARY);


}

void MainWindow::on_LoadScan_clicked()
{
    QString filepath = QFileDialog::getOpenFileName();
    QFile file(filepath);
    if(!filepath.isNull()){
        if(file.open(QIODevice::ReadWrite)){

            stream=new QTextStream(&file);
            scannerwindow* Scanner= new scannerwindow(parameters, this ,true,stream);

        }

        else{
            QMessageBox::critical(this, tr("Error"), "File Can't Be Opened");

        }
    }
}
