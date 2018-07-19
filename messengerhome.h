#ifndef MESSENGERHOME_H
#define MESSENGERHOME_H

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
#include "settings.h"

namespace Ui {
class MessengerHome;
}

class MessengerHome : public QWidget
{
    Q_OBJECT

public:
    explicit MessengerHome(QWidget *parent = 0);
    ~MessengerHome();

    void setWidgetsManager(QWidgetManager *value);

private slots:
    void on_myFiles_clicked();
    void on_myGroups_clicked();
    void on_logout_clicked();

    void checkForFriendRequest();
    void on_MessengerHome_destroyed();
    void on_myBuddies_clicked();

    void on_jackpots_clicked();

private:
    Ui::MessengerHome *ui;
    QWidgetManager *widgetsManager;
    QTimer *mTimer;

    void disableButtons(bool disable);
    void updateBuddiesList(QString &objectID, QJsonObject &buddiesList, QString &pendingAccepts);
    void setButtonSize(QWidget* a_pVictim, float ac_nWidth, float ac_nHeight);

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
};

#endif // MESSENGERHOME_H
