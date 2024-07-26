#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QDir>
#include <QFile>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir, QString strDstDir);

signals:
    void offline(MyTcpSocket *mysocket);

public slots:
    void recvMsg();
    void clientOffline();
    void sendFileToClient();

private:
    QString m_strName;

    QFile m_file;
    qint64 m_fileTotalSize;     //文件总的大小
    qint64 m_fileRecievedSize;  //已接受文件的大小
    bool m_uploadStatus;      //上传文件的状态：正在上传文件 or 其他状态

    QTimer *m_pTimer;


};

#endif // MYTCPSOCKET_H
