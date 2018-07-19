#ifndef JACKPOTS_H
#define JACKPOTS_H

#include <QWidget>
#include <qwidgetmanager.h>
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
#include <iostream>
#include <QStringList>
#include <QSignalMapper>
#include <QMessageBox>
#include <QSound>
#include "settings.h"

using namespace std;

namespace TOPIC {
    const QString GAME_CREATION = "12312";
    const QString PRE_GAME = "asdsa";
    const QString IN_GAME = "rgf";
    const QString POST_GAME = "4rdf";

    const QString SERVER = "6hfr";
    const QString CLIENT = "6hfghf";

    const QString T_GAME_ROOM = "gfdgf";
    const QString T_JOIN_ROOM = "gt";
    const QString T_ROOM_JOINED = "5gd";
    const QString T_PRESENT = "jtkr";
    const QString T_START_GAME = "thg";
    const QString T_START_READY = "dse";
    const QString T_START = "start";



    const QString T_GAME_OVERVIEW = "overview";
    const QString T_BET = "bet";
    const QString T_BET_REPLY = "betrep";
    const QString T_GAME_RESULT = "gameresult";
}

struct PlayerPotBets {
    int potABets = 0;
    int potBBets = 0;
    int potCBets = 0;
    int potDBets = 0;
};

enum POT {
    POT_A, POT_B, POT_C, POT_D
};

namespace Ui {
class Jackpots;
}

class Jackpots : public QWidget
{
    Q_OBJECT

public:
    explicit Jackpots(QWidget *parent = 0);
    ~Jackpots();

    void setWidgetsManager(QWidgetManager *value);
    void setChannelName(const QString &value);
    void setPlayerTimers(const QMap<QString, QTimer *> &value);
    void setPlayerState(const QString &value);
    void setHostName(const QString &value);
    void setCashPerPot(int value);
    void setTotalPot(int value);
    void setSubtopic(const QString &value);

    void setQuitShown(bool value);

private:
    Ui::Jackpots *ui;
    QWidgetManager *widgetsManager;
    QSound *coinSound;
    QMap<QString, QTimer*> playerTimers;
    QString hostName;
    QString playerState;
    QTimer *channelSenderTimer;
    QTimer *channelReceiverTimer;
    QTimer *clientTimeout;
    QTimer *timedMSGTimer;
    QTimer *gameResultsTimer;
    QSignalMapper *signalMapper;
    QNetworkAccessManager *channelManager;

    QString subtopic;
    QString channelName;
    QString subscriptionID;
    int totalPot;
    int cashPerPot;
    int maxBetPerPot;

    int originalPlayerSize;
    int lastBetTurn;
    int currentTurn;    //Decreasing order,
    int lastPotBet; //1,2,3,4
    QStringList turnsList;
    int potACash, potBCash, potCCash, potDCash;
    int potALeft, potBLeft, potCLeft, potDLeft;
    int myPotABets, myPotBBets, myPotCBets, myPotDBets;
    QMap<QString, PlayerPotBets> playerPotBets;
    QString timedMSGStr;
    bool myTurn;
    bool quitShown;

    bool resultsSent;
    bool gameResultsFinish;
    int currentPotJackpotResult;
    QMap<QString, int> potAResults, potBResults, potCResults, potDResults;
    QMap<QString, int> potAWinners, potBWinners, potCWinners, potDWinners;

    void putTimedMessage(QString message);
    void setMyTurn();
    void setButtonSize(QWidget* a_pVictim, float ac_nWidth, float ac_nHeight);
    void sendMessage(QString message);
    void resetUI();
    void flagErrors(QString message, int state);
    void disableButtons(bool disable);
    void initGame();
    void subscribeToChannel();

    void sendClientBetMessage(int potNumber, int turnNum);
    void placePotBet(QString playerName, POT pot);
    void updatePotLabels();
    void disableAllPots();
    void setHighLabelText(QString str);
    void setLowLabelText(QString str);
    void decreaseTotalPot(int decreaseAmount);
    void startTimers();
    void stopGame();
    void flushNewCash(int increaseAmount);
    void changeCashData(int amount);

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private slots:
    void on_quit_clicked();
    void manageChannelMessages();
    void handleReceivedMsgs(QNetworkReply *reply);
    void removePlayerFromList(QString player);
    void on_potA_clicked();
    void on_potB_clicked();
    void on_potC_clicked();
    void on_potD_clicked();
    void resetTimedMSG();
    void evaluateConnections();
    void disconnectFromServer();
    void showGameResults();
};

#endif // JACKPOTS_H
