#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_tcpIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray bydata = file.readAll();
        QString strdata = bydata.toStdString().c_str();
        // qDebug()<<strdata;
        file.close();

        strdata.replace("\r\n"," ");
        // qDebug()<<strdata;
        QStringList strList = strdata.split(" ");
        // for(int i=0;i<strList.size();i++)
        // {
        //     qDebug() << "--->" << strList[i];
        // }
        m_tcpIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug()<<"ip:"<<m_tcpIP<<" port:"<<m_usPort;
    }else{
        QMessageBox::critical(this,"open config","open config failed!");
    }
}
