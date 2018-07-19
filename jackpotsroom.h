#ifndef JACKPOTSROOM_H
#define JACKPOTSROOM_H

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
#include <map>
#include <QMap>
#include <QMessageBox>
#include <QSignalMapper>

namespace Ui {
class JackpotsRoom;
}

class JackpotsRoom : public QWidget
{
    Q_OBJECT

public:
    explicit JackpotsRoom(QWidget *parent = 0);
    ~JackpotsRoom();

    void setWidgetsManager(QWidgetManager *value);
    void setJackpots(Jackpots *value);

    void setChannelName(const QString &value);
    void setSubtopic(const QString &value);
    void setSubscriptionID(const QString &value);
    void startRoomCreationSignal();
    void setCashPerPot(int value);
    void setTotalPlayers(int value);
    void addPlayerToList(QString playerName, QTimer *timer);
    void setPlayerState(bool isServer);
    void setHealthyConnection(bool value);
    void setHostName(QString hostName);
    void setPlayers(vector<string> players);
    void setTotalPot(int value);

private:
    Ui::JackpotsRoom *ui;
    QWidgetManager *widgetsManager;
    Jackpots *jackpots;
    QTimer *roomCreatedT;
    QMap<QString, QTimer*> playerTimers;
    QMap<QString, bool> readyPlayers;
    QTimer *channelSenderTimer;
    QTimer *channelReceiverTimer;
    QTimer *clientTimeout;
    QNetworkAccessManager *channelManager;
    bool healthyConnection;
    bool receivePresentStates;
    QString hostName;
    QSignalMapper *signalMapper;
    QTimer *startGameTimeout;
    QTimer *startGameTimer;

    QString playerState;
    QString channelName;
    QString subtopic;
    QString subscriptionID;
    int totalPlayers;
    int cashPerPot;
    int totalPot;

    void flagErrors(QString message, int state);
    void disableButtons(bool disable);
    void resetUI();
    void initRoom();
    void sendMessage(QString message);

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private slots:
    void on_back_clicked();
    void signalRoomCreated();
    void roomCreatedSent(QNetworkReply* reply);
    void disconnectFromServer();
    void evaluateConnections();
    void manageChannelMessages();
    void handleReceivedMsgs(QNetworkReply* reply);
    void removePlayerFromList(QString player);
    void on_start_clicked();

    void startGame();
    void startGameFail();
};

#endif // JACKPOTSROOM_H
