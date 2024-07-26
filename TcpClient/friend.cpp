#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include "QInputDialog"
#include <QDebug>
#include "string.h"
#include "privatechat.h"
#include <QMessageBox>


Friend::    Friend(QWidget *parent)
    : QWidget{parent}
{
    //几个选项卡纵向布局，称中间框
    m_pDelFriendPB = new QPushButton("删除好友");
    mFlushFriendPB = new QPushButton("刷新好友列表");
    m_pShowOnlineUserPB = new QPushButton("显示在线用户");
    m_pSearchUserPB = new QPushButton("查找用户");
    m_pPrivateChatPB = new QPushButton("私聊");
    QVBoxLayout *pMiddlePBVBL = new QVBoxLayout;    //纵向布局
    pMiddlePBVBL->addWidget(m_pDelFriendPB);
    pMiddlePBVBL->addWidget(mFlushFriendPB);
    pMiddlePBVBL->addWidget(m_pShowOnlineUserPB);
    pMiddlePBVBL->addWidget(m_pSearchUserPB);
    pMiddlePBVBL->addWidget(m_pPrivateChatPB);

    //消息显示框 好友列表框 中间选项框 横向布局，称顶部框
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pMiddlePBVBL);

    //消息发送按钮和消息输入框横向布局，称为信息框
    m_pMsgSendPB = new QPushButton("信息发送");
    m_pInputMsgLE = new QLineEdit;
    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    //用户名称框与顶部框与信息框纵向布局，称为左边框
    m_pNamePB = new QPushButton(QString("用\t\t\t户: %1")\
                                    .arg(TcpClient::getInstance()\
                                             .getLoginName().toUtf8())); //显示用户名称
    QVBoxLayout *pLeftHBL = new QVBoxLayout;
    pLeftHBL->addWidget(m_pNamePB); //用户名称放在最上面
    pLeftHBL->addLayout(pTopHBL);   //顶部框放中间
    pLeftHBL->addLayout(pMsgHBL);   //信息框放最下面

    //左边框 与 显示在线用户框横向布局,称为主框

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addLayout(pLeftHBL);
    m_pOnline = new Online;
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUserPB,SIGNAL(clicked(bool))
            ,this,SLOT(showOnline()));
    connect(m_pSearchUserPB,SIGNAL(clicked(bool))
            ,this,SLOT(searchUser()));
    connect(mFlushFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool))
            ,this,SLOT(deleteFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool))
            ,this,SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked(bool))
            ,this,SLOT(groupChat()));

}

void Friend::showAllOnlineUser(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    m_pOnline->showUser(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLen/32;
    char caName[32] = {'\0'};

    //清空在线好友列表，防止多次点击重复添加
    m_pFriendListWidget->clear();

    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg = QString("%1 : %2").arg(pdu->caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        m_pOnline->hide();
    }
}

void Friend::searchUser()
{
    m_strSearchName = QInputDialog::getText(this,"搜索","用户名");
    if(!m_strSearchName.isEmpty())
    {
        qDebug()<<m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy((char *)(pdu->caData),m_strSearchName.toUtf8().toStdString().c_str(),m_strSearchName.toUtf8().size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()
{
    QString strName = TcpClient::getInstance().getLoginName().toUtf8();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strName.toUtf8().toStdString().c_str(),strName.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::deleteFriend()
{
    if(NULL != m_pFriendListWidget->currentItem())
    {
        QString friendName = m_pFriendListWidget->currentItem()->text();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString myName = TcpClient::getInstance().getLoginName();
        memcpy(pdu->caData,myName.toUtf8().toStdString().c_str(),myName.toUtf8().size());
        memcpy(pdu->caData+32,friendName.toUtf8().toStdString().c_str(),friendName.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::privateChat()
{
    if(NULL != m_pFriendListWidget->currentItem())
    {
        QString chatName = m_pFriendListWidget->currentItem()->text();  //私聊时对方名字
        PrivateChat::getInstance().setChatName(chatName);
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }else{
        QMessageBox::warning(this,"私聊","请选择私聊对象！");
    }
}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();

    PDU *temp_pdu = mkPDU(strMsg.toUtf8().size()+1);    //这个包不发，仅把自己想要发送的消息显示在聊天框
    QString myName = TcpClient::getInstance().getLoginName();
    memcpy((char*)temp_pdu->caData,myName.toUtf8().toStdString().c_str(),myName.toUtf8().size()); // 拷贝我的名字
    memcpy((char*)temp_pdu->caMsg,strMsg.toUtf8().toStdString().c_str(),strMsg.toUtf8().size()); // 拷贝我的消息
    PrivateChat::getInstance().updateMsg(temp_pdu); //更新聊天框
    free(temp_pdu);
    temp_pdu = NULL;

    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.toUtf8().size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        QString strName = TcpClient::getInstance().getLoginName();
        strncpy(pdu->caData,strName.toUtf8().toStdString().c_str(),strName.toUtf8().size());
        strncpy((char*)pdu->caMsg,strMsg.toUtf8().toStdString().c_str(),strMsg.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::warning(this,"群聊","信息不能为空!");
    }
}
