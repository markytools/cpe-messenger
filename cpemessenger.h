#ifndef CPEMESSENGER_H
#define CPEMESSENGER_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <register.h>
#include <registersuccess.h>
#include <login.h>
#include <messengerhome.h>
#include <myfiles.h>
#include <uploadfile.h>
#include <QListWidget>
#include <groupselection.h>
#include <newgroup.h>
#include <joingroup.h>
#include <groupnotifications.h>
#include <passwordrecovery.h>
#include <buddies.h>
#include <jackpots.h>
#include <jackpotshome.h>
#include <jackpotsroom.h>

namespace Ui {
class CPEMessenger;
}

class CPEMessenger : public QWidget
{
    Q_OBJECT

public:
    explicit CPEMessenger(QWidget *parent = 0);
    ~CPEMessenger();

private slots:
    void on_registerB_clicked();
    void on_login_clicked();

private:
    Ui::CPEMessenger *ui;

    QWidgetManager *widgetsManager;
    Register *newAccount;
    RegisterSuccess *regSuccess;
    Login *login;
    MessengerHome *messengerHome;
    MyFiles *myFiles;
    UploadFile *uploadFile;
    GroupSelection *groupSelection;
    NewGroup *newGroup;
    JoinGroup *joinGroup;
    GroupNotifications *groupNotifications;
    PasswordRecovery *passwordRecovery;
    Buddies *buddies;
    Jackpots *jackpots;
    JackpotsHome *jackpotsHome;
    JackpotsRoom *jackpotsRoom;
};

#endif // CPEMESSENGER_H
