#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
#include "string.h"
#include "privatechat.h"

PrivateChat::PrivateChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setChatName(QString strName)
{
    m_strChatName = strName;
    m_strLoginName = TcpClient::getInstance().getLoginName();
}

void PrivateChat::updateMsg(const PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }
    char friendName[32] = {'\0'};
    memcpy(friendName,pdu->caData,32);
    QString strMsg = QString("%1 : %2").arg(friendName).arg((char*)pdu->caMsg);
    ui->showMsg_te->append(strMsg);
}

void PrivateChat::on_sendMsg_pb_clicked()
{
    QString strMsg = ui->inputMsg_le->text();
    PDU *temp_pdu = mkPDU(strMsg.toUtf8().size()+1);    //这个包不发，仅把自己想要发送的消息显示在聊天框
    memcpy((char*)temp_pdu->caData,m_strLoginName.toUtf8().toStdString().c_str(),m_strLoginName.toUtf8().size()); // 拷贝我的名字
    memcpy((char*)temp_pdu->caMsg,strMsg.toUtf8().toStdString().c_str(),strMsg.toUtf8().size()); // 拷贝我的消息
    PrivateChat::getInstance().updateMsg(temp_pdu); //更新聊天框
    free(temp_pdu);
    temp_pdu = NULL;
    ui->inputMsg_le->clear();//信息发送完后清空发送框
    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.toUtf8().size()+1+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        //拷贝自己的名字
        memcpy(pdu->caData,m_strLoginName.toUtf8().toStdString().c_str(),m_strLoginName.toUtf8().size());
        //拷贝对方的名字
        memcpy(pdu->caData+32,m_strChatName.toUtf8().toStdString().c_str(),m_strChatName.toUtf8().size());
        //拷贝聊天信息
        memcpy((char*)pdu->caMsg,strMsg.toUtf8().toStdString().c_str(),strMsg.toUtf8().size());
        //发送数据
        qDebug()<<"(char*)pdu->caMsg:"<<(char*)pdu->caMsg;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else {
        QMessageBox::warning(this,"私聊","发送信息不能为空！");
    }
}

