#include "sharefile.h"
#include "tcpclient.h"
#include "opewidget.h"


ShareFile::ShareFile(QWidget *parent)
    : QWidget{parent}
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCnacelSelectPB = new QPushButton("取消选择");

    m_pOKPB = new QPushButton("确定");
    m_pCnacelPB = new QPushButton("取消");

    m_pSA = new QScrollArea;
    m_pFriendW = new QWidget;
    m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup(m_pFriendW);
    m_pButtonGroup->setExclusive(false);    //多选

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCnacelSelectPB);
    pTopHBL->addStretch();   //弹簧，放在最左边

    QHBoxLayout *pBottomHBL = new QHBoxLayout;
    pBottomHBL->addWidget(m_pOKPB);
    pBottomHBL->addWidget(m_pCnacelPB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pBottomHBL);

    setLayout(pMainVBL);

    connect(m_pCnacelSelectPB,SIGNAL(clicked(bool))
            ,this,SLOT(cancleSelect()));
    connect(m_pSelectAllPB,SIGNAL(clicked(bool))
            ,this,SLOT(selectAll()));
    connect(m_pOKPB,SIGNAL(clicked(bool))
            ,this,SLOT(okShare()));
    connect(m_pCnacelPB,SIGNAL(clicked(bool))
            ,this,SLOT(cancleShare()));
    // test();
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::test()
{
    QVBoxLayout *p = new QVBoxLayout(m_pFriendW);
    QCheckBox *pCB = NULL;
    for(int i=0;i<15;i++)
    {
        pCB = new QCheckBox("handoly");
        p->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{
    if(NULL == pFriendList)
    {
        return;
    }
    QAbstractButton *tmp = NULL;
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();  //获取之前的好友列表
    for(int i=0;i<preFriendList.size();i++)
    {
        tmp = preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }
    QCheckBox *pCB = NULL;
    for(int i=0;i<pFriendList->count();i++)
    {
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::cancleSelect()
{
    QList<QAbstractButton*> cbList =  m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            cbList[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cbList =  m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++)
    {
        if(!cbList[i]->isChecked())
        {
            cbList[i]->setChecked(true);
        }
    }
}

void ShareFile::okShare()
{
    qDebug()<<"已点击确定按钮";
    QString strName = TcpClient::getInstance().getLoginName();
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QString strShareFileName = OpeWidget::getInstance().getBook()->getShareFileName();
    QList<QAbstractButton*> cbList =  m_pButtonGroup->buttons();

    QString strPath = strCurPath+"/"+strShareFileName;

    int num = 0;
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            num++;
        }
    }

    PDU *pdu = mkPDU(32*num+strPath.toUtf8().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toUtf8().toStdString().c_str(),num);
    int j=0;
    for(int i=0;i<cbList.size();i++)
    {

        if(cbList[i]->isChecked())
        {
            memcpy((char*)(pdu->caMsg)+j*32,cbList[i]->text().toUtf8().toStdString().c_str(),cbList[i]->text().toUtf8().size());
            j++;
        }
    }

    memcpy((char*)(pdu->caMsg)+num*32, strPath.toUtf8().toStdString().c_str(),strPath.toUtf8().size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void ShareFile::cancleShare()
{
    hide();
}

