#ifndef TX_RX_PROTOCOL_H
#define TX_RX_PROTOCOL_H

#include <QObject>
#include <QtCore/QtGlobal>
#include <QString>
#include <QMutex>
#include <QQueue>

class TX_RX_qt : public QObject
{
    Q_OBJECT

public:

    QByteArray receive_frame_buffer;
    quint16 frame_position;
    static TX_RX_qt* instance()
    {
        static QMutex mutex;
        if (!m_Instance)
        {
            mutex.lock();
            if (!m_Instance)
                m_Instance = new TX_RX_qt;
            mutex.unlock();
        }
        return m_Instance;
    }

    static void drop()
    {
        static QMutex mutex;
        mutex.lock();
        delete m_Instance;
        m_Instance = 0;
        mutex.unlock();
    }


signals:
    void TX_TransmitByte(QByteArray data);
    void TX_TransmitByte(char data);
    void ValidFrameReceived(QByteArray receive_frame_buffer, quint16 frame_size);

public slots:
    void charReceiver(QByteArray dataArray);
    void frameDecode(QByteArray buffer, quint16 bytes_to_send);

private:
    bool streaming=false;
    bool data_incoming=false;


    const QByteArray FRAME_BOUNDARY=QByteArray(';',1);

    TX_RX_qt(){}
    TX_RX_qt(const TX_RX_qt &);
    TX_RX_qt& operator=(const TX_RX_qt &);

    static TX_RX_qt* m_Instance;

};

#endif // TX_RX_PROTOCOL_H
