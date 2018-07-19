#ifndef BUDDIES_H
#define BUDDIES_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <QJsonArray>
#include <groupnotifications.h>
#include "settings.h"

namespace Ui {
class Buddies;
}

class Buddies : public QWidget
{
    Q_OBJECT

public:
    explicit Buddies(QWidget *parent = 0);
    ~Buddies();

    void setWidgetsManager(QWidgetManager *value);
    void setGroupNotifications(GroupNotifications *value);

private slots:
    void on_unbuddy_clicked();

    void on_addBuddy_clicked();

    void on_chat_clicked();

    void on_backB_clicked();

private:
    Ui::Buddies *ui;
    QWidgetManager *widgetsManager;
    GroupNotifications *groupNotifications;

    void disableButtons(bool disable);
    void resetUI();
    void flagErrors(QString message, int state);
    void updateBuddiesList(QString &objectID, QJsonObject &buddiesList, QString &pendingAccepts);
    void updateBuddies();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
};

#endif // BUDDIES_H
