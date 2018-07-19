#ifndef PASSWORDRECOVERY_H
#define PASSWORDRECOVERY_H

#include <QWidget>
#include <qwidgetmanager.h>
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

namespace Ui {
class PasswordRecovery;
}

class PasswordRecovery : public QWidget
{
    Q_OBJECT

public:
    explicit PasswordRecovery(QWidget *parent = 0);
    ~PasswordRecovery();

    void setWidgetsManager(QWidgetManager *value);
    void setIdentityQuestion(const QString &value);
    void setAnswer(const QString &value);
    void setEmail(const QString &value);

private slots:
    void on_backB_clicked();

    void on_confirmB_clicked();

private:
    QString answer;
    Ui::PasswordRecovery *ui;
    QWidgetManager *widgetsManager;
    QString email;

    void resetUI();
    void disableButtons(bool disable);
    void flagErrors(QString message, int state);
};

#endif // PASSWORDRECOVERY_H
