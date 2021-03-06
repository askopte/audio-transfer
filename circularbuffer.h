#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QObject>
#include <QMutex>
#include <QByteArray>
#include <QString>

class CircularBuffer : public QObject
{
    Q_OBJECT
public:
    explicit CircularBuffer(const uint bufferSize,const QString bufferName = "",QObject *parent = 0);
    ~CircularBuffer();
    int getSize();
    bool append(const QByteArray *newData);
    bool append(const QString text);
    bool append(const char* data,size_t size);
    void clear();
    QByteArray getCurrentPosData(int length);
    QByteArray getCurrentPosData();
    QByteArray getData();
    size_t getAvailableBytesCount();
    static void runTest();
    void operator <<(QByteArray newData);
    QByteArray operator >>(const int lenght);
    enum OverflowPolicy {
        Drop = 0,
        Expand = 1,
        Replace = 2
    };
    void setOverflowPolicy(const OverflowPolicy newPolicy);
    void setPacketSize(const size_t size);

private:
    size_t packetSize;
    int bsize;
    int originalSize;
    QByteArray data;
    int positionRead;
    int positionWrite;
    QMutex mutex;
    OverflowPolicy policy;
    void say(const QString message);

signals:
    void readyRead(const int size);
    void overflowPolicyChanged(const OverflowPolicy newPolicy);
    void debug(const QString message);

public slots:

};

#endif // CIRCULARBUFFER_H
