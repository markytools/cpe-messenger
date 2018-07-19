#ifndef JACKPOTSHOME_H
#define JACKPOTSHOME_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <jackpots.h>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <QTimer>
#include <QInputDialog>
#include <QJsonArray>
#include <QTimer>
#include <map>
#include <jackpotsroom.h>

namespace Ui {
class JackpotsHome;
}

class JackpotsHome : public QWidget
{
    Q_OBJECT

public:
    explicit JackpotsHome(QWidget *parent = 0);
    ~JackpotsHome();

    void setWidgetsManager(QWidgetManager *value);
    void setJackpots(Jackpots *value);
    void setJackpotsRoom(JackpotsRoom *value);

private slots:
    void on_createB_clicked();
    void listOfRooms(QNetworkReply *reply);

    void on_refreshB_clicked();
    void on_joinB_clicked();
    void hostConnectionError();
    void checkForHostSignal();
    void hostSignalReceived(QNetworkReply *reply);

    void on_backB_clicked();

private:
    Ui::JackpotsHome *ui;
    QWidgetManager *widgetsManager;
    Jackpots *jackpots;
    JackpotsRoom *jackpotsRoom;
    QNetworkAccessManager *contManager;
    QTimer *joinGameTimer;
    QTimer *hostSignalTimer;

    QMap<QString, QString> hostName;
    QString channelName;
    QString subtopic;
    QString subscriptionID;
    int totalPot;

    void flagError(QString message, int state);
    void disableButtons(bool disable);
    void refreshData();
    void checkForRooms();
    void sendMessage(QString message, bool disable);
    void addGameToList(QString gameName, QString publisherID);
    void resetUI();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
};

#endif // JACKPOTSHOME_H
