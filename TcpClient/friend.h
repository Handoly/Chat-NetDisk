#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <online.h>



class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUser(PDU *pdu);
    void updateFriendList(PDU *pdu);
    void updateGroupMsg(PDU *pdu);

    QString m_strSearchName;

    QListWidget *getFriendList();

signals:

public slots:
    void showOnline();
    void searchUser();
    void flushFriend();
    void deleteFriend();
    void privateChat();
    void groupChat();

private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendListWidget;
    QLineEdit *m_pInputMsgLE;

    QPushButton *m_pNamePB;

    QPushButton *m_pDelFriendPB;
    QPushButton *mFlushFriendPB;
    QPushButton *m_pShowOnlineUserPB;
    QPushButton *m_pSearchUserPB;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;

    Online *m_pOnline;

};

#endif // FRIEND_H
