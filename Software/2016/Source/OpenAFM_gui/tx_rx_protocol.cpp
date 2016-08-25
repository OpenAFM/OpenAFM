#include "TX_RX_PROTOCOL.h"
#include <QDebug>

TX_RX_qt* TX_RX_qt::m_Instance = 0;

void TX_RX_qt::charReceiver(QByteArray dataArray)
{

    quint8 data = 0;

    for(QByteArray::iterator it = dataArray.begin(); it != dataArray.end(); it++) {

        data = (*it);

        receive_frame_buffer[frame_position] = data;
        frame_position++;

        if(data == ';')
        {
            QByteArray valid_frame;
            for(int i=0;i<frame_position;++i){

                valid_frame[i]=receive_frame_buffer[i];

            }
            receive_frame_buffer.remove(0,frame_position);
            emit ValidFrameReceived(valid_frame, frame_position);

            frame_position = 0;
            continue;
        }
    }
}


void TX_RX_qt::frameDecode(QByteArray buffer, quint16 bytes_to_send)
{
    char data;
    QByteArray packet;
    int i = 0;
    while (i < bytes_to_send)
    {
        data = buffer[i];
        packet.append((char)data);
        i++;
    }
    emit TX_TransmitByte(packet);
}



