#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString getEnterDir();
    void setDownloadStatus(bool status);
    bool getDownloadStatus();
    QString getSaveFilePath();
    QString getShareFileName();

    qint64 m_iFileTotalSize;        //总的文件大小
    qint64 m_iFileRecievedSize;     //已经收到的文件大小

public slots:
    void createDir();
    void flushFile();
    void deleteDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void returnPreDir();
    void deleteRegFile();
    void uploadFile();  //上传文件

    void uploadFileData();  //上传文件内容

    void downloadFile();

    void shareFile();

    void moveFile();
    void selectDestDir();

private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDeleteDirPB;
    QPushButton *m_pRenanmePB;
    QPushButton *m_pFlushFilePB;
    QPushButton *m_pUploadFilePB;
    QPushButton *m_pDownloadFilePB;
    QPushButton *m_pDeleteFilePB;
    QPushButton *m_pShareFilePB;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;

    QString m_strEnterDir;

    QString m_strUploadFilePath;

    QTimer *m_pTimer;

    QString m_strSaveFilePath;
    bool m_bIsDownloading;

    QString m_strShareFileName;

    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strDestDir;

signals:
};

#endif // BOOK_H
