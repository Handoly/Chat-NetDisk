/********************************************************************************
** Form generated from reading UI file 'online.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINE_H
#define UI_ONLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Online
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QListWidget *online_lw;
    QVBoxLayout *verticalLayout;
    QPushButton *addfriend_pb;
    QPushButton *pushButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *Online)
    {
        if (Online->objectName().isEmpty())
            Online->setObjectName("Online");
        Online->resize(325, 363);
        verticalLayout_2 = new QVBoxLayout(Online);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        online_lw = new QListWidget(Online);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        new QListWidgetItem(online_lw);
        online_lw->setObjectName("online_lw");
        QFont font;
        font.setPointSize(16);
        online_lw->setFont(font);

        horizontalLayout->addWidget(online_lw);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        addfriend_pb = new QPushButton(Online);
        addfriend_pb->setObjectName("addfriend_pb");
        QFont font1;
        font1.setPointSize(14);
        addfriend_pb->setFont(font1);

        verticalLayout->addWidget(addfriend_pb);

        pushButton = new QPushButton(Online);
        pushButton->setObjectName("pushButton");
        pushButton->setFont(font1);

        verticalLayout->addWidget(pushButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(Online);

        QMetaObject::connectSlotsByName(Online);
    } // setupUi

    void retranslateUi(QWidget *Online)
    {
        Online->setWindowTitle(QCoreApplication::translate("Online", "Form", nullptr));

        const bool __sortingEnabled = online_lw->isSortingEnabled();
        online_lw->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = online_lw->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("Online", "111", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = online_lw->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("Online", "222", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = online_lw->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("Online", "333", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = online_lw->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("Online", "444", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = online_lw->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("Online", "555", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = online_lw->item(5);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("Online", "66", nullptr));
        QListWidgetItem *___qlistwidgetitem6 = online_lw->item(6);
        ___qlistwidgetitem6->setText(QCoreApplication::translate("Online", "666", nullptr));
        QListWidgetItem *___qlistwidgetitem7 = online_lw->item(7);
        ___qlistwidgetitem7->setText(QCoreApplication::translate("Online", "677", nullptr));
        online_lw->setSortingEnabled(__sortingEnabled);

        addfriend_pb->setText(QCoreApplication::translate("Online", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
        pushButton->setText(QCoreApplication::translate("Online", "\344\270\276\346\212\245\347\224\250\346\210\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Online: public Ui_Online {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINE_H
