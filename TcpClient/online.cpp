#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUser(PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLen/32;
    char caTemp[32];

    QString myName = TcpClient::getInstance().getLoginName().toUtf8();

    //先清空再添加，防止多次点击导致重复添加
    ui->online_lw->clear();

    for(uint i=0;i<uiSize;i++)
    {
        qDebug()<<"TcpClient::getInstance().getLoginName():"<<TcpClient::getInstance().getLoginName();
        qDebug()<<"(char *)(pdu->caMsg):"<<(char *)(pdu->caMsg)+i*32;
        if(0 != strcmp((char *)(pdu->caMsg)+i*32,myName.toUtf8().toStdString().c_str()))//不添加自己
        {
            memcpy(caTemp,(char *)(pdu->caMsg)+i*32,32);
            ui->online_lw->addItem(caTemp);
        }
    }

}

void Online::on_addfriend_pb_clicked()
{
    QListWidgetItem *pItem =  ui->online_lw->currentItem();
    QString strPerUserName = pItem->text().toUtf8();
    QString strLoginName = TcpClient::getInstance().getLoginName().toUtf8();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerUserName.toUtf8().toStdString().c_str(),strPerUserName.toUtf8().size());
    memcpy(pdu->caData+32,strLoginName.toUtf8().toStdString().c_str(),strLoginName.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

