#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);

    static ShareFile &getInstance();

    void test();

    void updateFriend(QListWidget *pFriendList);

public slots:
    void cancleSelect();
    void selectAll();
    void okShare();
    void cancleShare();

private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCnacelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_pCnacelPB;

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;

    QVBoxLayout *m_pFriendWVBL;

    QButtonGroup *m_pButtonGroup;

signals:
};

#endif // SHAREFILE_H
