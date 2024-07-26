#include "mytcpsocket.h"
#include <QDebug>
#include "stdio.h"
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>
#include <QTimer>

MyTcpSocket::MyTcpSocket()
{
    connect(this,SIGNAL(readyRead())
            ,this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected())
            ,this,SLOT(clientOffline()));

    m_uploadStatus = false; //初始化为不是上传文件的状态
    m_pTimer = new QTimer;

    connect(m_pTimer,SIGNAL(timeout())
            ,this,SLOT(sendFileToClient()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDstDir)
{
    qDebug()<<"正在拷贝目录";
    QDir dir;
    dir.mkdir(strDstDir);

    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();

    QString srcTmp;
    QString destTmp;
    for(int i=0;i<fileInfoList.size();i++)
    {
        qDebug()<<"filename"<<fileInfoList[i].fileName();
        if(fileInfoList[i].isFile())
        {
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDstDir + '/' + fileInfoList[i].fileName();
            QFile::copy(srcTmp,destTmp);
        }else if(fileInfoList[i].isDir())
        {
            if(QString(".")==fileInfoList[i].fileName() || QString("..")==fileInfoList[i].fileName())
            {
                continue;
            }
            srcTmp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTmp = strDstDir + '/' + fileInfoList[i].fileName();
            copyDir(srcTmp, destTmp);   //递归调用
        }
    }
}

void MyTcpSocket::recvMsg()
{
    if(!m_uploadStatus)
    {
        qDebug()<<this->bytesAvailable();
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        switch(pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleRegist(caName,caPwd);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,REGIST_OK);
                QDir dir;
                qDebug()<<"create dir:"<<dir.mkdir(QString("./%1").arg(caName)); //创建目录
            }else{
                strcpy(respdu->caData,REGIST_FAILED);
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            bool ret = OpeDB::getInstance().handleLogin(caName,caPwd);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,LOGIN_OK);
                m_strName = caName;
            }else{
                strcpy(respdu->caData,LOGIN_FAILED);
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret = OpeDB::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for(int i=0;i<ret.size();i++)
            {
                memcpy((char*)(respdu->caMsg)+i*32
                       ,ret.at(i).toUtf8().toStdString().c_str()
                       ,ret.at(i).toUtf8().size());
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
        {
            int ret = OpeDB::getInstance().handleSearchUser(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
            if(-1 == ret)
            {
                strcpy(respdu->caData,SEARCH_USER_NO);
            }else if(1==ret)
            {
                strcpy(respdu->caData,SEARCH_USER_ONLINE);
            }else if(0==ret)
            {
                strcpy(respdu->caData,SEARCH_USER_OFFLINE);
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caperName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caperName,pdu->caData,32);
            strncpy(caName,pdu->caData+32,32);
            int ret = OpeDB::getInstance().handleAddFriend(caperName,caName);
            PDU *respdu = NULL;
            if(-1 == ret)           //未知错误
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,UNKNOW_ERROR);
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }else if(0 == ret){     //已经是好友
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,EXISTED_FRIEND);
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }else if(1 == ret){     //用户存在且在线
                MyTcpServer::getInstance().resend(caperName,pdu);
            }else if(2 == ret){     //用户存在但不在线
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }else if(3 == ret){     //用户不存在
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData,ADD_FRIEND_NO_EXIST);
                write((char *)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
        {
            //修改数据库
            //向发送方发送反馈消息
            char caName[32] = {'\0'};//用户名或好友的用户名，自行安排
            char caPwd[32] = {'\0'};//用户名或好友的用户名，自行安排
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            OpeDB::getInstance().handleAddRelationShip(caName,caPwd);
            pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
            MyTcpServer::getInstance().resend(caPwd,pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            //直接向发送方反馈消息
            char caName[32] = {'\0'};//用户名或好友的用户名，自行安排
            char caPwd[32] = {'\0'};//用户名或好友的用户名，自行安排
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            MyTcpServer::getInstance().resend(caPwd,pdu);//转发给我
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};//用户名或好友的用户名，自行安排
            strncpy(caName,pdu->caData,32);
            QStringList ret = OpeDB::getInstance().handleFlushFriend(caName);
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType  = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i=0;i<ret.size();i++)
            {
                memcpy((char*)(respdu->caMsg)+i*32
                       ,ret.at(i).toUtf8().toStdString().c_str()
                       ,ret.at(i).toUtf8().size());
            }
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char myName[32] = {'\0'};//用户名或好友的用户名，自行安排
            char friendName[32] = {'\0'};//用户名或好友的用户名，自行安排
            strncpy(myName,pdu->caData,32);
            strncpy(friendName,pdu->caData+32,32);
            OpeDB::getInstance().handleDeleteFriend(myName,friendName);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            strcpy((respdu->caData),DELETE_FRIEND_OK);
            write((char *)respdu,respdu->uiPDULen); // 给发送删除请求的人的回复
            free(respdu);
            respdu = NULL;

            MyTcpServer::getInstance().resend(friendName,pdu);  //给被删除的人的提示

            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            char chatfriendName[32] = {'\0'};           //私聊好友名字
            memcpy(chatfriendName,pdu->caData+32,32);   //拷贝私聊好友名字
            qDebug()<<"chatfriendName:"<<chatfriendName;
            qDebug()<<"(char*)pdu->caMsg:"<<(char*)pdu->caMsg;
            MyTcpServer::getInstance().resend(chatfriendName,pdu);  //转发给私聊好友
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            char caName[32] = {'\0'};//用户名或好友的用户名，自行安排
            strncpy(caName,pdu->caData,32);
            QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);
            QString tmp;
            for(int i=0;i<onlineFriend.size();i++)
            {
                tmp = onlineFriend.at(i);
                MyTcpServer::getInstance().resend(tmp.toUtf8().toStdString().c_str(),pdu);  //转发给私聊好友
            }
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {
            QDir dir;
            QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));    //提前目录
            qDebug()<<"strCurPath:"<<strCurPath;
            bool ret = dir.exists(strCurPath);  //判断目录是否存在
            qDebug()<<"ret:"<<ret;
            PDU *respdu = NULL;
            if(ret)     //当前目录存在
            {
                char caNewDir[32] = {'\0'};
                memcpy(caNewDir,pdu->caData+32,32);     //提取文件名，拼接新路径
                QString strNewPath = strCurPath+"/"+caNewDir;
                qDebug()<<strNewPath;
                ret = dir.exists(strNewPath);
                qDebug()<<"--->"<<ret;
                if(ret)     //创建的文件名已经存在，
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,FILE_NAME_EXIST);
                }else{      //创建的文件名不存在，则创建
                    dir.mkdir(strNewPath);
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,CREATE_DIR_OK);
                }
            }
            else        //当前目录不存在
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,DIR_NO_EXIST);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        {
            char *pCurPath = new char[pdu->uiMsgLen];
            memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
            QDir dir(pCurPath);
            QFileInfoList fileInfoList = dir.entryInfoList();

            //清除".."和"."
            fileInfoList.removeFirst();
            fileInfoList.removeFirst();
            //清除".."和"."

            int fileCount = fileInfoList.size() ;        //文件个数
            PDU *respdu = mkPDU(sizeof(FileInfo)*(fileCount));
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo *pFileInfo = NULL;
            QString strFileName;
            for(int i=0;i<fileCount;i++)
            {
                pFileInfo= (FileInfo*)(respdu->caMsg)+i;
                strFileName = fileInfoList[i].fileName();
                memcpy(pFileInfo->fileName,strFileName.toUtf8().toStdString().c_str(),strFileName.toUtf8().size());
                if(fileInfoList[i].isDir())
                {
                    pFileInfo->fileType = 0;        //0表示是文件夹
                }else if(fileInfoList[i].isFile())
                {
                    pFileInfo->fileType = 1;   //1表示是常规文件
                }
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_DIR_REQUEST:
        {
            char fileName[32] = {'\0'};
            strcpy(fileName,pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(fileName);
            qDebug()<<strPath;

            //delete []pPath;
            pPath = NULL;

            QFileInfo fileInfo(strPath);
            bool ret = false;
            if(fileInfo.isDir())
            {
                QDir dir;
                dir.setPath(strPath);
                ret = dir.removeRecursively();

            }else if(fileInfo.isFile()){    //常规文件
                ret = false;
            }
            PDU *respdu = NULL;
            if(ret)
            {
                respdu = mkPDU(0);
                memcpy(respdu->caData,DELETE_DIR_OK,strlen(DELETE_DIR_OK));
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_RESPOND;
            }else
            {
                respdu = mkPDU(0);
                memcpy(respdu->caData,DELETE_DIR_FIALED,strlen(DELETE_DIR_FIALED));
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_RESPOND;
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        {
            char oldName[32] = {'\0'};
            char newName[32] = {'\0'};
            strncpy(oldName,pdu->caData,32);
            strncpy(newName,pdu->caData+32,32);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strOldPath = QString("%1/%2").arg(pPath).arg(oldName);
            QString strNewPath = QString("%1/%2").arg(pPath).arg(newName);

            qDebug()<<strOldPath;
            qDebug()<<strNewPath;


            QDir dir;
            bool ret = dir.rename(strOldPath,strNewPath);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            if(ret)
            {
                strcpy(respdu->caData,RENAME_FILE_OK);
            }else{
                strcpy(respdu->caData,RENAME_FILE_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
        {
            char enterDirName[32] = {'\0'};
            strncpy(enterDirName,pdu->caData,32);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(enterDirName);
            qDebug()<<"strPath:"<<strPath;

            PDU *respdu = NULL;
            QFileInfo fileInfo(strPath);
            if(fileInfo.isDir())
            {
                QDir dir(strPath);
                QFileInfoList fileInfoList = dir.entryInfoList();

                //清除".."和"."
                fileInfoList.removeFirst();
                fileInfoList.removeFirst();
                //清除".."和"."

                int fileCount = fileInfoList.size() ;        //文件个数
                PDU *respdu = mkPDU(sizeof(FileInfo)*(fileCount));
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                FileInfo *pFileInfo = NULL;
                QString strFileName;
                for(int i=0;i<fileCount;i++)
                {
                    pFileInfo= (FileInfo*)(respdu->caMsg)+i;
                    strFileName = fileInfoList[i].fileName();

                    memcpy(pFileInfo->fileName,strFileName.toUtf8().toStdString().c_str(),strFileName.toUtf8().size());
                    if(fileInfoList[i].isDir())
                    {
                        pFileInfo->fileType = 0;        //0表示是文件夹
                    }else if(fileInfoList[i].isFile())
                    {
                        pFileInfo->fileType = 1;   //1表示是常规文件
                    }
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }else if(fileInfo.isFile())
            {
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                strcpy(respdu->caData,ENTER_DIR_FAILED);

                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        {
            char fileName[32] = {'\0'};
            qint64 fileSize = 0;
            sscanf(pdu->caData,"%s %lld",fileName,&fileSize);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPth = QString("%1/%2").arg(pPath).arg(fileName);
            delete []pPath;
            pPath = NULL;

            m_file.setFileName(strPth);
            if(m_file.open(QIODevice::WriteOnly))
            {
                m_uploadStatus = true;
                m_fileTotalSize = fileSize;
                m_fileRecievedSize = 0;
            }

            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:
        {
            char fileName[32] = {'\0'};
            strcpy(fileName,pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(fileName);
            qDebug()<<strPath;
            delete []pPath;
            pPath = NULL;

            QFileInfo fileInfo(strPath);
            bool ret = false;
            if(fileInfo.isDir())
            {
                ret = false;
            }else if(fileInfo.isFile()){    //常规文件
                QDir dir;
                ret = dir.remove(strPath);
            }
            PDU *respdu = NULL;
            if(ret)
            {
                respdu = mkPDU(0);
                memcpy(respdu->caData,DELETE_FILE_OK,strlen(DELETE_FILE_OK));
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
            }else
            {
                respdu = mkPDU(0);
                memcpy(respdu->caData,DELETE_FILE_FIALED,strlen(DELETE_FILE_FIALED));
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        {
            char caFileName[32] = {'\0'};
            strcpy(caFileName,pdu->caData);
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
            qDebug()<<strPath;
            delete []pPath;
            pPath = NULL;

            QFileInfo fileInfo(strPath);
            qint64 fileSize = fileInfo.size();
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            sprintf(respdu->caData,"%s %lld",caFileName,fileSize);

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_pTimer->start(1000);

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        {
            qDebug()<<"服务器已收到分享文件请求";
            char caSendName[32] = {'\0'};
            int num = 0;
            sscanf(pdu->caData,"%s %d",caSendName,&num);
            int size = num*32;
            PDU *respdu = mkPDU(pdu->uiMsgLen-size);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTICE;
            strcpy(respdu->caData,caSendName);
            memcpy(respdu->caMsg,(char*)(pdu->caMsg)+size,pdu->uiMsgLen-size);

            char caRevName[32] = {'\0'};
            for(int i=0;i<num;i++)
            {
                memcpy(caRevName,(char*)(pdu->caMsg)+i*32,32);
                MyTcpServer::getInstance().resend(caRevName,respdu);
            }
            free(respdu);
            respdu =NULL;

            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData,"share file successfully!");
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu =NULL;

            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTICE_RESPOND:
        {
            QString strRecvPath = QString("./%1").arg(pdu->caData);
            QString strShareFilePath = QString("%1").arg((char*)(pdu->caMsg));

            int index = strShareFilePath.lastIndexOf('/');
            QString strFileName = strShareFilePath.right(strShareFilePath.size() - index -1);
            strRecvPath = strRecvPath + '/' + strFileName;

            QFileInfo fileInfo(strShareFilePath);
            if(fileInfo.isFile())
            {
                QFile::copy(strShareFilePath,strRecvPath);
            }else if(fileInfo.isDir()){
                copyDir(strShareFilePath,strRecvPath);
            }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        {
            char cafileName[32] = {'\0'};
            int srcLen = 0;
            int destLen = 0;
            sscanf(pdu->caData,"%d %d %s", &srcLen, &destLen, cafileName);

            char *pSrcPth = new char[srcLen+1];
            char *pDestPath = new char[destLen+1+32];
            memset(pSrcPth,'\0',srcLen+1);
            memset(pDestPath,'\0',destLen+1+32);

            memcpy(pSrcPth,pdu->caMsg,srcLen);
            memcpy(pDestPath,(char*)(pdu->caMsg)+(srcLen+1),destLen);

            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;

            QFileInfo fileInfo(pDestPath);
            if(fileInfo.isDir())
            {
                strcat(pDestPath,"/");
                strcat(pDestPath,cafileName);

                bool ret = QFile::rename(pSrcPth,pDestPath);
                if(ret)
                {
                    strcpy(respdu->caData, MOVE_FILE_OK);
                }else{
                    strcpy(respdu->caData, COMMON_ERR);
                }
            }else if(fileInfo.isFile())
            {
                strcpy(respdu->caData, MOVE_FILE_FIALED);
            }

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
        // qDebug()<<caName<<caPwd<<pdu->uiMsgType;
    }else
    {

        PDU *respdu  = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        QByteArray buff = this->readAll();    //接受读取到的数据
        m_file.write(buff);             //将收到的数据内容写到文件里面
        qDebug()<<"buff.size()"<<buff.size();
        m_fileRecievedSize += buff.size();  //收到的数据大小更新

        if(m_fileTotalSize == m_fileRecievedSize)
        {
            m_file.close();
            m_uploadStatus = false;
            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        else if(m_fileTotalSize < m_fileRecievedSize)
        {
            m_file.close();
            m_uploadStatus = false;
            strcpy(respdu->caData,UPLOAD_FILE_FAILED);

            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
    }
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

void MyTcpSocket::sendFileToClient()
{
    // m_pTimer->stop();
    char *pData = new char[4096];
    qint64 ret = 0;
    while(true)
    {
        ret = m_file.read(pData,4096);
        if(ret > 0 && ret <= 4096)
        {
            write(pData,ret);
        }else if(0 == ret)
        {
            m_file.close();
            break;
        }else if(ret < 0)
        {
            qDebug()<<"发送文件内容给客户端过程中失败";
            m_file.close();
            break;
        }
    }
    delete []pData;
    pData = NULL;
}
