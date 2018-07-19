#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <userinfo.h>
#include <iostream>
#include <QJsonArray>
#include <passwordrecovery.h>
#include "settings.h"

using namespace std;

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

    void setWidgetsManager(QWidgetManager *value);
    void setPasswordRecovery(PasswordRecovery *value);

private slots:
    void on_login_clicked();
    void on_backB_clicked();
    void on_passwordForgetButton_clicked();

private:
    Ui::Login *ui;
    QWidgetManager *widgetsManager;
    QStringList questions;
    PasswordRecovery *passwordRecovery;

    QString currentLoginPassword;
    void flagErrors(QString message);
    void resetUI();
    void disableButtons();
    void enableButtons();
};

#endif // LOGIN_H
