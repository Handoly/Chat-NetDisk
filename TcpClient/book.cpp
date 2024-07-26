#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileDialog>
#include <opewidget.h>
#include <sharefile.h>

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_strEnterDir.clear();

    m_bIsDownloading = false;

    m_pTimer = new QTimer;

    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDeleteDirPB = new QPushButton("删除文件夹");
    m_pRenanmePB = new QPushButton("重命名");
    m_pFlushFilePB = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDeleteDirPB);
    pDirVBL->addWidget(m_pRenanmePB);
    pDirVBL->addWidget(m_pFlushFilePB);

    m_pUploadFilePB = new QPushButton("上传文件");
    m_pDownloadFilePB = new QPushButton("下载文件");
    m_pDeleteFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("分享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadFilePB);
    pFileVBL->addWidget(m_pDownloadFilePB);
    pFileVBL->addWidget(m_pDeleteFilePB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(createDir()));
    connect(m_pFlushFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(flushFile()));
    connect(m_pDeleteDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(deleteDir()));
    connect(m_pRenanmePB,SIGNAL(clicked(bool))
            ,this,SLOT(renameFile()));
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex))
            ,this,SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB,SIGNAL(clicked(bool))
            ,this,SLOT(returnPreDir()));
    connect(m_pUploadFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout())
            ,this,SLOT(uploadFileData()));
    connect(m_pDeleteFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(deleteRegFile()));
    connect(m_pDownloadFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(downloadFile()));
    connect(m_pShareFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(shareFile()));
    connect(m_pMoveFilePB,SIGNAL(clicked(bool))
            ,this,SLOT(moveFile()));
    connect(m_pSelectDirPB,SIGNAL(clicked(bool))
            ,this,SLOT(selectDestDir()));
}

void Book::updateFileList(const PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }

    m_pBookListW->clear();  //清空原来的项目，防止多次点击后重复添加

    FileInfo *pFileInfo = NULL;
    int countFile = pdu->uiMsgLen/sizeof(FileInfo); //有多少个文件
    for(int i=0;i<countFile;i++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        qDebug()<<pFileInfo->fileName<<pFileInfo->fileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(0 == pFileInfo->fileType)
        {
            qDebug()<<"文件夹";
            pItem->setIcon(QIcon(QPixmap(":/map/dir.png")));
        }else if(1 == pFileInfo->fileType)
        {
            qDebug()<<"文件";
            pItem->setIcon(QIcon(QPixmap(":/map/reg.png")));
        }
        pItem->setText(pFileInfo->fileName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::getEnterDir()
{
    return m_strEnterDir;
}

void Book::setDownloadStatus(bool status)
{
    m_bIsDownloading = status;
}

bool Book::getDownloadStatus()
{
    return m_bIsDownloading;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32)
        {
            QMessageBox::warning(this,"新建文件夹","新文件夹名不能超过32个字符！");
        }
        else{
            QString strName = TcpClient::getInstance().getLoginName();
            QString strCurPath = TcpClient::getInstance().getCurPath();
            PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toUtf8().toStdString().c_str(),strName.toUtf8().size());
            strncpy(pdu->caData+32,strNewDir.toUtf8().toStdString().c_str(),strNewDir.toUtf8().size());
            memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }
    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","新文件夹名不能为空！");
    }
}

void Book::flushFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Book::deleteDir()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)      //表示没有选择任何文件
    {
        QMessageBox::warning(this,"删除文件","请选择要删除的文件!");
    }else{
        QString strDeleteName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_REQUEST;
        strncpy((char*)pdu->caData,strDeleteName.toUtf8().toStdString().c_str(),strDeleteName.toUtf8().size());
        memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Book::renameFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)      //表示没有选择任何文件
    {
        QMessageBox::warning(this,"重命名文件","请选择要重命名的文件!");
    }else{
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"重命名文件","新文件名");
        if(!strNewName.isEmpty())
        {
            PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy((char*)pdu->caData,strOldName.toUtf8().toStdString().c_str(),strOldName.toUtf8().size());
            strncpy((char*)pdu->caData+32,strNewName.toUtf8().toStdString().c_str(),strNewName.toUtf8().size());
            memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }else{
            QMessageBox::warning(this,"重命名文件","新文件名不能为空!");
        }
    }
}

void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();

    m_strEnterDir = strDirName;

    QString strCurPath = TcpClient::getInstance().getCurPath();
    PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy((char*)pdu->caData,strDirName.toUtf8().toStdString().c_str(),strDirName.toUtf8().size());
    memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Book::returnPreDir()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();         //当前目录
    QString strRootPath = "./"+TcpClient::getInstance().getLoginName(); //根目录
    if(strCurPath == strRootPath)
    {
        QMessageBox::warning(this,"返回","返回失败：已在根目录！");
    }else
    {   //e.g: "./rose/fuck/u" ---> "./rose/fuck"
        int index = strCurPath.lastIndexOf('/');    //返回最后一个斜杠
        //删除斜杠后面的内容
        qDebug()<<"strCurPath:"<<strCurPath;
        strCurPath.remove(index,strCurPath.size()-index);
        qDebug()<<"strCurPath:"<<strCurPath;
        TcpClient::getInstance().setCurPath(strCurPath);//更新当前路径

        clearEnterDir();    //清空进入的文件夹

        flushFile();    //刷新文件
    }

}

void Book::deleteRegFile()
{
    QString strCurPath = TcpClient::getInstance().getCurPath();
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)      //表示没有选择任何文件
    {
        QMessageBox::warning(this,"删除文件","请选择要删除的文件!");
    }else{
        QString strDeleteName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
        strncpy((char*)pdu->caData,strDeleteName.toUtf8().toStdString().c_str(),strDeleteName.toUtf8().size());
        memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Book::uploadFile()
{
    m_strUploadFilePath = QFileDialog::getOpenFileName();//获得上传本地文件的绝对路径
    if(!m_strUploadFilePath.isEmpty())
    {
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index -1);

        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size();

        QString strCurPath = TcpClient::getInstance().getCurPath();
        PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
        sprintf(pdu->caData,"%s %lld",strFileName.toUtf8().toStdString().c_str(),fileSize);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

        m_pTimer->start(1000);

    }else{
        QMessageBox::warning(this,"上传文件","上传文件名不能为空！");
    }

}

void Book::uploadFileData()
{
    m_pTimer->stop();//关掉定时器否则会重新计时
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"上传文件","打开文件失败！");
        return ;
    }
    char *pBuffer = new char[4096];
    qint64 ret = 0; //实际从文件中读取的内容
    while(true)
    {
        ret = file.read(pBuffer,4096);
        qDebug()<<"ret:"<<ret;
        if(ret > 0 && ret <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }else if(0 == ret)//读到文件末尾
        {
            break;
        }else//小于0
        {
            QMessageBox::warning(this,"上传文件","失败：读文件失败！");
            break;
        }
    }

    file.close();
    delete [] pBuffer;
    pBuffer = NULL;
}

void Book::downloadFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)      //表示没有选择任何文件
    {
        QMessageBox::warning(this,"下载文件","请选择要下载的文件!");
    }else{

        QString strSaveFilePath = QFileDialog::getSaveFileName();   //弹出一个窗口让您选择一个保存文件的路径
        if(strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this,"下载文件","请指定要保存的位置");
            m_strSaveFilePath.clear();
        }else{
            m_strSaveFilePath = strSaveFilePath;
        }

        QString strCurPath = TcpClient::getInstance().getCurPath();
        PDU *pdu = mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData,strFileName.toUtf8().toStdString().c_str());
        memcpy(pdu->caMsg,strCurPath.toUtf8().toStdString().c_str(),strCurPath.toUtf8().size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    }
}

void Book::shareFile()
{
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(NULL == pItem)      //表示没有选择任何文件
    {
        QMessageBox::warning(this,"分享文件","请选择要分享的文件!");
        return;
    }else{
        m_strShareFileName = pItem->text();
    }
    Friend *pFriend = OpeWidget::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    ShareFile::getInstance().updateFriend(pFriendList);
    if(ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }
}

void Book::moveFile()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        m_strMoveFileName = pCurItem->text();
        QString strCurPath = TcpClient::getInstance().getCurPath();
        m_strMoveFilePath =  strCurPath + '/' + m_strMoveFileName;

        m_pSelectDirPB->setEnabled(true);

    }else{
        QMessageBox::warning(this,"移动文件","请选择要移动的文件!");
    }
}

void Book::selectDestDir()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        QString strDestDir = pCurItem->text();
        QString strCurPath = TcpClient::getInstance().getCurPath();
        m_strDestDir =  strCurPath + '/' + strDestDir;
        int srcLen = m_strMoveFilePath.toUtf8().size();
        int destLen = m_strDestDir.toUtf8().size();
        PDU *pdu = mkPDU(srcLen+destLen+2);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->caData,"%d %d %s",srcLen,destLen,m_strMoveFileName.toUtf8().toStdString().c_str());
        memcpy(pdu->caMsg,m_strMoveFilePath.toUtf8().toStdString().c_str(),srcLen);
        memcpy((char*)(pdu->caMsg)+(srcLen+1),m_strDestDir.toUtf8().toStdString().c_str(),destLen);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::warning(this,"移动文件","请选择要移动的文件!");
    }
    m_pSelectDirPB->setEnabled(false);
}
