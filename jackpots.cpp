#include "jackpots.h"
#include "ui_jackpots.h"

#ifdef Q_OS_WIN

#else
#include <QtAndroidExtras/QAndroidJniObject>
#endif

Jackpots::Jackpots(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Jackpots)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
    setButtonSize(ui->potA, 150, 150);
    setButtonSize(ui->potB, 150, 150);
    setButtonSize(ui->potC, 150, 150);
    setButtonSize(ui->potD, 150, 150);
    ui->potA->setIconSize(QSize(100, 100));
    ui->potB->setIconSize(QSize(100, 100));
    ui->potC->setIconSize(QSize(100, 100));
    ui->potD->setIconSize(QSize(100, 100));

    coinSound = new QSound(":/game/jackpots/jackpots/coin.mp3");
    channelManager = new QNetworkAccessManager(this);
    signalMapper = new QSignalMapper(this);
    channelSenderTimer = new QTimer(this);
    channelReceiverTimer = new QTimer(this);
    timedMSGTimer = new QTimer(this);
    gameResultsTimer = new QTimer(this);
    clientTimeout = new QTimer(this);
}

void Jackpots::startTimers()
{
    channelSenderTimer->start(2000);
    channelReceiverTimer->start(2000);
}

void Jackpots::stopGame()
{
    setHighLabelText("");
    clientTimeout->stop();
    channelSenderTimer->stop();
    channelReceiverTimer->stop();
    timedMSGTimer->stop();


    if (playerState == "SERVER"){
        for (QString &playerName : playerTimers.keys()){
            playerTimers[playerName]->stop();
        }
    }
}

void Jackpots::flushNewCash(int increaseAmount)
{
    totalPot += increaseAmount;

    changeCashData(totalPot);
}

void Jackpots::changeCashData(int amount)
{
    QVariantMap testMapData;
    testMapData["cash"] = amount;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/CashData/" + widgetsManager->getUserInfo()->getCashDataID()));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->put(request, testJsonData.toJson());
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){

    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
}

Jackpots::~Jackpots()
{
    delete ui;
}
void Jackpots::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}
void Jackpots::setChannelName(const QString &value)
{
    channelName = value;
}

void Jackpots::setPlayerTimers(const QMap<QString, QTimer *> &value)
{
    playerTimers = value;
}
void Jackpots::setPlayerState(const QString &value)
{
    playerState = value;
}
void Jackpots::setHostName(const QString &value)
{
    hostName = value;
}

void Jackpots::setCashPerPot(int value)
{
    cashPerPot = value;
    ui->cashPerPot->setText("<font color='red'>CASH PER POT: $" + QString::number(cashPerPot) + "</font>");
}
void Jackpots::setTotalPot(int value)
{
    totalPot = value;
}
void Jackpots::setSubtopic(const QString &value)
{
    subtopic = value;
}
void Jackpots::setQuitShown(bool value)
{
    quitShown = value;
}


void Jackpots::putTimedMessage(QString message)
{
    timedMSGStr = message;
    timedMSGTimer->start(5000);
}

void Jackpots::setMyTurn()
{
    if (myTurn){
        if (potALeft != 0) ui->potA->setDisabled(false);
        if (potBLeft != 0) ui->potB->setDisabled(false);
        if (potCLeft != 0) ui->potC->setDisabled(false);
        if (potDLeft != 0) ui->potD->setDisabled(false);
        setLowLabelText("YOUR TURN. PLACE A BET!");
    }

#ifdef Q_OS_WIN

#else
    //    QAndroidJniObject javaNotification = QAndroidJniObject::fromString("Downloading the file...");
    //    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
    //                                       "toast",
    //                                       "(Ljava/lang/String;)V",
    //                                       javaNotification.object<jstring>());

    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient", "vibrate", "(I)V", 300);
#endif
}

void Jackpots::setButtonSize(QWidget* a_pVictim, float ac_nWidth, float ac_nHeight)
{
    a_pVictim->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    a_pVictim->setMaximumWidth ( ac_nWidth  );
    a_pVictim->setMinimumWidth ( ac_nWidth  );
    a_pVictim->setMaximumHeight( ac_nHeight );
    a_pVictim->setMinimumHeight( ac_nHeight );
}

void Jackpots::sendMessage(QString message)
{
    QVariantMap testMapData;
    testMapData["message"] = message;
    testMapData["publisherId"] = widgetsManager->getUserInfo()->getProfName();
    testMapData["subtopic"] = subtopic;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/messaging/" + channelName));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, testJsonData.toJson());

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){

    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
}

void Jackpots::resetUI()
{
    ui->playerList->clear();
    ui->response->clear();
}

void Jackpots::flagErrors(QString message, int state)
{
    if (state == 0){
        ui->response->setText("<font color='red'>" + message + "</font>");
    }
    else {
        ui->response->setText("<font color='green'>" + message + "</font>");
    }
}

void Jackpots::disableButtons(bool disable)
{
    ui->quit->setDisabled(disable);
}

void Jackpots::initGame()
{
    connect(gameResultsTimer, SIGNAL(timeout()), SLOT(showGameResults()));
    connect(clientTimeout, SIGNAL(timeout()), SLOT(disconnectFromServer()));
    connect(channelSenderTimer, SIGNAL(timeout()), SLOT(evaluateConnections()));
    connect(channelReceiverTimer, SIGNAL(timeout()), SLOT(manageChannelMessages()));
    connect(timedMSGTimer, SIGNAL(timeout()), SLOT(resetTimedMSG()));

    if (playerState == "SERVER"){
        ui->playerList->addItem(hostName);
        while (playerTimers.remove(widgetsManager->getUserInfo()->getProfName()));
        for (QString &playerName : playerTimers.keys()){
            ui->playerList->addItem(playerName);
            QTimer *pTimer = new QTimer();
            playerTimers[playerName] = pTimer;
            connect(pTimer, SIGNAL(timeout()), signalMapper, SLOT(map()));
            signalMapper->setMapping (pTimer, playerName);
            connect(signalMapper, SIGNAL(mapped(QString)), this,
                    SLOT(removePlayerFromList(QString)));

            pTimer->stop();
            pTimer->start(20000);
        }

        maxBetPerPot = (int)(((playerTimers.size() + 1) * 7) / 3.0);
        if (maxBetPerPot % 2 == 0) maxBetPerPot++;
        potALeft = maxBetPerPot;
        potBLeft = maxBetPerPot;
        potCLeft = maxBetPerPot;
        potDLeft = maxBetPerPot;
        potACash = 0;
        potBCash = 0;
        potCCash = 0;
        potDCash = 0;

        for (QString &playerName : playerTimers.keys()){
            for (int i = 0; i < 7; i++){
                turnsList.push_back(playerName);
            }
            PlayerPotBets pPBets;
            playerPotBets[playerName] = pPBets;
        }
        for (int i = 0; i < 7; i++){
            turnsList.push_back(hostName);
        }
        PlayerPotBets pPBets;
        playerPotBets[hostName] = pPBets;
        shuffleList(turnsList);
        currentTurn = turnsList.size();
    }
    else if (playerState == "CLIENT"){
        clientTimeout->start(20000);
    }
    ui->potA->setDisabled(true);
    ui->potB->setDisabled(true);
    ui->potC->setDisabled(true);
    ui->potD->setDisabled(true);
    ui->potACash->setText("");
    ui->potBCash->setText("");
    ui->potCCash->setText("");
    ui->potDCash->setText("");
    ui->potABets->setText("");
    ui->potBBets->setText("");
    ui->potCBets->setText("");
    ui->potDBets->setText("");

    myPotABets = 0;
    myPotBBets = 0;
    myPotCBets = 0;
    myPotDBets = 0;

    potAResults.clear();
    potBResults.clear();
    potCResults.clear();
    potDResults.clear();

    gameResultsFinish = false;
    currentPotJackpotResult = 0;
    resultsSent = false;
    myTurn = true;
    lastBetTurn = -1;
    lastPotBet = -1;
    setHighLabelText("---Jackpots---");
    setLowLabelText("Game about to start.");
    decreaseTotalPot(0);
}

void Jackpots::setHighLabelText(QString str)
{
    ui->topHighLabel->setText("<font color='brown'>" + str + "</font>");
}

void Jackpots::setLowLabelText(QString str)
{
    ui->topLowLabel->setText("<font color='green'>" + str + "</font>");
}

void Jackpots::decreaseTotalPot(int decreaseAmount)
{

    totalPot -= decreaseAmount;

    QVariantMap testMapData;
    testMapData["cash"] = totalPot;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/CashData/" + widgetsManager->getUserInfo()->getCashDataID()));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->put(request, testJsonData.toJson());
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){

    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();

    ui->totalCash->setText("<font color='purple'>My Total Pot: $" + QString::number(totalPot) + "</font>");
}

void Jackpots::evaluateConnections()
{
    if (playerState == "SERVER"){
        if (currentTurn > 0){
            vector<string> playerList;
            for (QString &player : playerTimers.keys()) playerList.push_back(player.toStdString());
            playerList.push_back(widgetsManager->getUserInfo()->getProfName().toStdString());
            QJsonObject cStateOBJ;
            cStateOBJ["topic"] = TOPIC::T_GAME_OVERVIEW;
            cStateOBJ["playerState"] = playerState;
            cStateOBJ["potACash"] = potACash;
            cStateOBJ["potBCash"] = potBCash;
            cStateOBJ["potCCash"] = potCCash;
            cStateOBJ["potDCash"] = potDCash;
            cStateOBJ["potALeft"] = potALeft;
            cStateOBJ["potBLeft"] = potBLeft;
            cStateOBJ["potCLeft"] = potCLeft;
            cStateOBJ["potDLeft"] = potDLeft;

            cStateOBJ["maxBetPerPot"] = maxBetPerPot;
            cStateOBJ["players"] = QString::fromStdString(arrayToString(playerList));
            cStateOBJ["currentTurn"] = currentTurn;
            cStateOBJ["currentPlayer"] = turnsList.at(currentTurn - 1);
            cStateOBJ["timedMSGStr"] = timedMSGStr;

            sendMessage(jsonObjectToQString(cStateOBJ));
        }
        else {
            if (!resultsSent){
                resultsSent = true;

                QJsonObject resultsOBJ;
                resultsOBJ["topic"] = TOPIC::T_GAME_RESULT;
                resultsOBJ["playerState"] = playerState;

                resultsOBJ["potACash"] = potACash;
                resultsOBJ["potBCash"] = potBCash;
                resultsOBJ["potCCash"] = potCCash;
                resultsOBJ["potDCash"] = potDCash;
                resultsOBJ["potALeft"] = potALeft;
                resultsOBJ["potBLeft"] = potBLeft;
                resultsOBJ["potCLeft"] = potCLeft;
                resultsOBJ["potDLeft"] = potDLeft;
                resultsOBJ["maxBetPerPot"] = maxBetPerPot;

                int totalPotAJackpot = 0;
                int totalPotBJackpot = 0;
                int totalPotCJackpot = 0;
                int totalPotDJackpot = 0;
                for (QString &playerName : playerPotBets.keys()){
                    PlayerPotBets pPBets = playerPotBets[playerName];
                    potAResults[playerName] = pPBets.potABets;
                    potBResults[playerName] = pPBets.potBBets;
                    potCResults[playerName] = pPBets.potCBets;
                    potDResults[playerName] = pPBets.potDBets;

                    totalPotAJackpot += pPBets.potABets * cashPerPot;
                    totalPotBJackpot += pPBets.potBBets * cashPerPot;
                    totalPotCJackpot += pPBets.potCBets * cashPerPot;
                    totalPotDJackpot += pPBets.potDBets * cashPerPot;
                }

                int highestBet = 0;
                for (QString &playerName : potAResults.keys()){
                    if (highestBet < potAResults[playerName]) highestBet = potAResults[playerName];
                }
                for (QString &playerName : potAResults.keys()){
                    if (highestBet == potAResults[playerName] && highestBet != 0) potAWinners[playerName] = highestBet;
                }
                for (QString &playerName : potAWinners.keys()){
                    potAWinners[playerName] = totalPotAJackpot / potAWinners.size();

                }
                highestBet = 0;
                for (QString &playerName : potBResults.keys()){
                    if (highestBet < potBResults[playerName]) highestBet = potBResults[playerName];
                }
                for (QString &playerName : potBResults.keys()){
                    if (highestBet == potBResults[playerName] && highestBet != 0) potBWinners[playerName] = highestBet;
                }
                for (QString &playerName : potBWinners.keys()){
                    potBWinners[playerName] = totalPotBJackpot / potBWinners.size();
                }

                highestBet = 0;
                for (QString &playerName : potCResults.keys()){
                    if (highestBet < potCResults[playerName]) highestBet = potCResults[playerName];
                }
                for (QString &playerName : potCResults.keys()){
                    if (highestBet == potCResults[playerName] && highestBet != 0) potCWinners[playerName] = highestBet;
                }
                for (QString &playerName : potCWinners.keys()){
                    potCWinners[playerName] = totalPotCJackpot / potCWinners.size();
                }

                highestBet = 0;
                for (QString &playerName : potDResults.keys()){
                    if (highestBet < potDResults[playerName]) highestBet = potDResults[playerName];
                }
                for (QString &playerName : potDResults.keys()){
                    if (highestBet == potDResults[playerName] && highestBet != 0) potDWinners[playerName] = highestBet;
                }
                for (QString &playerName : potDWinners.keys()){
                    potDWinners[playerName] = totalPotDJackpot / potDWinners.size();
                }

                QJsonObject potAResultsOBJ, potBResultsOBJ, potCResultsOBJ, potDResultsOBJ;
                for (QString &playerName : potAResults.keys()) potAResultsOBJ[playerName] = potAResults[playerName];
                for (QString &playerName : potBResults.keys()) potBResultsOBJ[playerName] = potBResults[playerName];
                for (QString &playerName : potCResults.keys()) potCResultsOBJ[playerName] = potCResults[playerName];
                for (QString &playerName : potDResults.keys()) potDResultsOBJ[playerName] = potDResults[playerName];

                QJsonObject potAWinnersOBJ, potBWinnersOBJ, potCWinnersOBJ, potDWinnersOBJ;
                for (QString &playerName : potAWinners.keys()) potAWinnersOBJ[playerName] = potAWinners[playerName];
                for (QString &playerName : potBWinners.keys()) potBWinnersOBJ[playerName] = potBWinners[playerName];
                for (QString &playerName : potCWinners.keys()) potCWinnersOBJ[playerName] = potCWinners[playerName];
                for (QString &playerName : potDWinners.keys()) potDWinnersOBJ[playerName] = potDWinners[playerName];

                resultsOBJ["potAResults"] = jsonObjectToQString(potAResultsOBJ);
                resultsOBJ["potBResults"] = jsonObjectToQString(potBResultsOBJ);
                resultsOBJ["potCResults"] = jsonObjectToQString(potCResultsOBJ);
                resultsOBJ["potDResults"] = jsonObjectToQString(potDResultsOBJ);

                resultsOBJ["potAWinners"] = jsonObjectToQString(potAWinnersOBJ);
                resultsOBJ["potBWinners"] = jsonObjectToQString(potBWinnersOBJ);
                resultsOBJ["potCWinners"] = jsonObjectToQString(potCWinnersOBJ);
                resultsOBJ["potDWinners"] = jsonObjectToQString(potDWinnersOBJ);

                sendMessage(jsonObjectToQString(resultsOBJ));
            }

            updatePotLabels();
            stopGame();
            setLowLabelText("Betting has ended.");
            gameResultsTimer->start(4000);
        }
    }
    else if (playerState == "CLIENT"){
        QJsonObject cStateOBJ;
        cStateOBJ["topic"] = TOPIC::T_PRESENT;
        cStateOBJ["playerState"] = playerState;
        sendMessage(jsonObjectToQString(cStateOBJ));
    }
}

void Jackpots::disconnectFromServer()
{
    quitShown = true;
    ui->playerList->clear();
//    QMessageBox::StandardButton connectionErrorBox;
//    connectionErrorBox = QMessageBox::information(this, "Error", "Connection to Server Lost",
//                                                  QMessageBox::Ok);
//    if (connectionErrorBox == QMessageBox::Ok) {
        resetUI();
        disconnect(this);
        widgetsManager->showWidget(JACKPOTS_HOME);
//    }
}

void Jackpots::showGameResults()
{
    currentPotJackpotResult++;
    int totalIncrease = 0;
    switch (currentPotJackpotResult){
    case 1: {
        setLowLabelText("Pot A Jackpot");
        ui->playerList->clear();
        for (QString &playerName : potAResults.keys()){
            ui->playerList->addItem("A: " + playerName + " - " + QString::number(potAResults[playerName]) + " bets");
        }
        int reward = 0;
        QString winnersStr = "Winner/s: ";
        int count = 0;
        for (QString &playerName : potAWinners.keys()){
            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                reward = potAWinners[playerName];
            }
            if (count == 0) winnersStr += playerName;
            else winnersStr += ", " + playerName;
            count++;
        }
        ui->cashPerPot->setText("<font color='brown'>My Pot A Jackpot: $" + QString::number(reward) + "</font>");
        flagErrors(winnersStr, 1);

        totalIncrease += reward;
        totalPot += reward;
        ui->totalCash->setText("<font color='purple'>My Total Pot: $" + QString::number(totalPot) + "</font>");

        if (reward != 0){
        #ifdef Q_OS_WIN

        #else
            QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient", "vibrate", "(I)V", 300);
        #endif
        }
    }; break;
    case 2: {
        setLowLabelText("Pot B Jackpot");
        for (QString &playerName : potBResults.keys()){
            ui->playerList->addItem("B: " + playerName + " - " + QString::number(potBResults[playerName]) + " bets");
        }
        int reward = 0;
        QString winnersStr = "Winner/s: ";
        int count = 0;
        for (QString &playerName : potBWinners.keys()){
            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                reward = potBWinners[playerName];
            }
            if (count == 0) winnersStr += playerName;
            else winnersStr += ", " + playerName;
            count++;
        }
        ui->cashPerPot->setText("<font color='brown'>My Pot B Jackpot: $" + QString::number(reward) + "</font>");
        flagErrors(winnersStr, 1);

        totalIncrease += reward;
        totalPot += reward;
        ui->totalCash->setText("<font color='purple'>My Total Pot: $" + QString::number(totalPot) + "</font>");

        if (reward != 0){
        #ifdef Q_OS_WIN

        #else
            QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient", "vibrate", "(I)V", 300);
        #endif
        }
    }; break;
    case 3: {
        setLowLabelText("Pot C Jackpot");
        for (QString &playerName : potCResults.keys()){
            ui->playerList->addItem("C: " + playerName + " - " + QString::number(potCResults[playerName]) + " bets");
        }
        int reward = 0;
        QString winnersStr = "Winner/s: ";
        int count = 0;
        for (QString &playerName : potCWinners.keys()){
            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                reward = potCWinners[playerName];
            }
            if (count == 0) winnersStr += playerName;
            else winnersStr += ", " + playerName;
            count++;
        }
        ui->cashPerPot->setText("<font color='brown'>My Pot C Jackpot: $" + QString::number(reward) + "</font>");
        flagErrors(winnersStr, 1);

        totalIncrease += reward;
        totalPot += reward;
        ui->totalCash->setText("<font color='purple'>My Total Pot: $" + QString::number(totalPot) + "</font>");

        if (reward != 0){
        #ifdef Q_OS_WIN

        #else
            QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient", "vibrate", "(I)V", 300);
        #endif
        }
    }; break;
    case 4: {
        setLowLabelText("Pot D Jackpot");
        for (QString &playerName : potDResults.keys()){
            ui->playerList->addItem("D: " + playerName + " - " + QString::number(potDResults[playerName]) + " bets");
        }
        int reward = 0;
        QString winnersStr = "Winner/s: ";
        int count = 0;
        for (QString &playerName : potDWinners.keys()){
            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                reward = potDWinners[playerName];
            }
            if (count == 0) winnersStr += playerName;
            else winnersStr += ", " + playerName;
            count++;
        }
        ui->cashPerPot->setText("<font color='brown'>My Pot D Jackpot: $" + QString::number(reward) + "</font>");
        flagErrors(winnersStr, 1);

        totalIncrease += reward;
        totalPot += reward;
        ui->totalCash->setText("<font color='purple'>My Total Pot: $" + QString::number(totalPot) + "</font>");

        if (reward != 0){
        #ifdef Q_OS_WIN

        #else
            QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient", "vibrate", "(I)V", 300);
        #endif
        }
    }; break;
    case 5: {
        flushNewCash(totalIncrease);
        gameResultsTimer->stop();
        gameResultsFinish = true;
        flagErrors("You can now leave the game.", 1);
    }; break;
    default: break;
}

}

void Jackpots::subscribeToChannel()
{
    QVariantMap testMapData;
    testMapData["subtopic"] = subtopic;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/messaging/" + channelName + "/subscribe"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->post(request, testJsonData.toJson());
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        subscriptionID = json.value("subscriptionId").toString();
    }
    else {
        flagErrors("Connection Error", 0);
    }
    reply->deleteLater();
}

void Jackpots::placePotBet(QString playerName, POT pot)
{
    switch (pot){
    case POT_A: {
        potACash += cashPerPot;
        potALeft--;
        playerPotBets[playerName].potABets++;
        currentTurn--;
    }; break;
    case POT_B: {
        potBCash += cashPerPot;
        potBLeft--;
        playerPotBets[playerName].potBBets++;
        currentTurn--;
    }; break;
    case POT_C: {
        potCCash += cashPerPot;
        potCLeft--;
        playerPotBets[playerName].potCBets++;
        currentTurn--;
    }; break;
    case POT_D: {
        potDCash += cashPerPot;
        potDLeft--;
        playerPotBets[playerName].potDBets++;
        currentTurn--;
    }; break;
    default: break;
    }
}

void Jackpots::updatePotLabels()
{
    ui->potABets->setText("<font color='brown'>" + QString::number(myPotABets) + "/" + QString::number(maxBetPerPot - potALeft)
                          + "/" + QString::number(maxBetPerPot) + "</font>");
    ui->potBBets->setText("<font color='brown'>" + QString::number(myPotBBets) + "/" + QString::number(maxBetPerPot - potBLeft)
                          + "/" + QString::number(maxBetPerPot) + "</font>");
    ui->potCBets->setText("<font color='brown'>" + QString::number(myPotCBets) + "/" + QString::number(maxBetPerPot - potCLeft)
                          + "/" + QString::number(maxBetPerPot) + "</font>");
    ui->potDBets->setText("<font color='brown'>" + QString::number(myPotDBets) + "/" + QString::number(maxBetPerPot - potDLeft)
                          + "/" + QString::number(maxBetPerPot) + "</font>");

    ui->potACash->setText("<font color='orange'>$" + QString::number(potACash) + "</font>");
    ui->potBCash->setText("<font color='orange'>$" + QString::number(potBCash) + "</font>");
    ui->potCCash->setText("<font color='orange'>$" + QString::number(potCCash) + "</font>");
    ui->potDCash->setText("<font color='orange'>$" + QString::number(potDCash) + "</font>");

    if (potACash != 0){
        if (potALeft == 0){
            QPixmap pix(":/game/jackpots/jackpots/potA3.png");
            QIcon icon(pix);
            ui->potA->setIcon(icon);
            ui->potA->setIconSize(QSize(100, 100));
        }
        else {
            QPixmap pix(":/game/jackpots/jackpots/potA2.png");
            QIcon icon(pix);
            ui->potA->setIcon(icon);
            ui->potA->setIconSize(QSize(100, 100));
        }
    }
    else {
        QPixmap pix(":/game/jackpots/jackpots/potA1.png");
        QIcon icon(pix);
        ui->potA->setIcon(icon);
        ui->potA->setIconSize(QSize(100, 100));
    }

    if (potBCash != 0){
        if (potBLeft == 0){
            QPixmap pix(":/game/jackpots/jackpots/potB3.png");
            QIcon icon(pix);
            ui->potB->setIcon(icon);
            ui->potB->setIconSize(QSize(100, 100));
        }
        else {
            QPixmap pix(":/game/jackpots/jackpots/potB2.png");
            QIcon icon(pix);
            ui->potB->setIcon(icon);
            ui->potB->setIconSize(QSize(100, 100));
        }
    }
    else {
        QPixmap pix(":/game/jackpots/jackpots/potB1.png");
        QIcon icon(pix);
        ui->potB->setIcon(icon);
        ui->potB->setIconSize(QSize(100, 100));
    }

    if (potCCash != 0){
        if (potCLeft == 0){
            QPixmap pix(":/game/jackpots/jackpots/potC3.png");
            QIcon icon(pix);
            ui->potC->setIcon(icon);
            ui->potC->setIconSize(QSize(100, 100));
        }
        else {
            QPixmap pix(":/game/jackpots/jackpots/potC2.png");
            QIcon icon(pix);
            ui->potC->setIcon(icon);
            ui->potC->setIconSize(QSize(100, 100));
        }
    }
    else {
        QPixmap pix(":/game/jackpots/jackpots/potC1.png");
        QIcon icon(pix);
        ui->potC->setIcon(icon);
        ui->potC->setIconSize(QSize(100, 100));
    }

    if (potDCash != 0){
        if (potDLeft == 0){
            QPixmap pix(":/game/jackpots/jackpots/potD3.png");
            QIcon icon(pix);
            ui->potD->setIcon(icon);
            ui->potD->setIconSize(QSize(100, 100));
        }
        else {
            QPixmap pix(":/game/jackpots/jackpots/potD2.png");
            QIcon icon(pix);
            ui->potD->setIcon(icon);
            ui->potD->setIconSize(QSize(100, 100));
        }
    }
    else {
        QPixmap pix(":/game/jackpots/jackpots/potD1.png");
        QIcon icon(pix);
        ui->potD->setIcon(icon);
        ui->potD->setIconSize(QSize(100, 100));
    }
}

void Jackpots::disableAllPots()
{
    ui->potA->setDisabled(true);
    ui->potB->setDisabled(true);
    ui->potC->setDisabled(true);
    ui->potD->setDisabled(true);
}

void Jackpots::showEvent(QShowEvent *)
{
    if (!quitShown){
        disableButtons(false);
        subscribeToChannel();
        initGame();
        startTimers();
    }
    else quitShown = false;
}

void Jackpots::manageChannelMessages()
{
    if (subscriptionID != NULL){
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/messaging/" + channelName + "/" + subscriptionID));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        channelManager->get(request);
        connect(channelManager, SIGNAL(finished(QNetworkReply*)), SLOT(handleReceivedMsgs(QNetworkReply*)));
    }
}

void Jackpots::handleReceivedMsgs(QNetworkReply *reply)
{
    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray messagesData = json.value("messages").toArray();
        for (int i = 0; i < messagesData.size(); i++){
            QJsonObject messagesDataObj = messagesData.at(i).toObject();
            const QString publisherID = messagesDataObj.value("publisherId").toString();
            QJsonObject dataOBJ = qstringToJSONObject(messagesDataObj.value("data").toString());
            QString topicValue = dataOBJ.value("topic").toString();
            if (playerState == "SERVER"){

                if (topicValue == TOPIC::T_GAME_OVERVIEW){
                    QString msgStr = dataOBJ.value("timedMSGStr").toString();
                    flagErrors(msgStr, 0);
                    QString currentPlayer = dataOBJ.value("currentPlayer").toString();
                    int currentTurn = dataOBJ.value("currentTurn").toInt();
                    if (currentTurn > 0){
                        if (currentPlayer == widgetsManager->getUserInfo()->getProfName()){
                            if (lastBetTurn != currentTurn){
                                setMyTurn();
                            }
                        }
                        else {
                            setLowLabelText("TURNS LEFT: " + QString::number(currentTurn));
                        }
                        updatePotLabels();
                    }
                }
                else if (topicValue == TOPIC::T_BET){
                    QString playerState = dataOBJ.value("playerState").toString();
                    if (playerState == "CLIENT"){
                        int potNumber = dataOBJ.value("potNumber").toInt();
                        POT betPOT;
                        switch (potNumber){
                        case 1: betPOT = POT_A; break;
                        case 2: betPOT = POT_B; break;
                        case 3: betPOT = POT_C; break;
                        case 4: betPOT = POT_D; break;
                        default: break;
                        }
                        placePotBet(publisherID, betPOT);

                        QJsonObject betReplyOBJ;
                        betReplyOBJ["topic"] = TOPIC::T_BET_REPLY;
                        betReplyOBJ["target"] = publisherID;
                        betReplyOBJ["playerState"] = this->playerState;
                        sendMessage(jsonObjectToQString(betReplyOBJ));
                    }
                }
                else if (topicValue == TOPIC::T_PRESENT){
                    QString playerState = dataOBJ.value("playerState").toString();
                    if (playerState == "CLIENT"){
                        QTimer *pTimer = playerTimers[publisherID];
                        pTimer->stop();
                        pTimer->start(20000);
                    }
                }
            }
            else if (playerState == "CLIENT"){
                if (topicValue == TOPIC::T_GAME_OVERVIEW){
                    int currentTurn = dataOBJ.value("currentTurn").toInt();

                    this->potACash = dataOBJ.value("potACash").toInt();
                    this->potBCash = dataOBJ.value("potBCash").toInt();
                    this->potCCash = dataOBJ.value("potCCash").toInt();
                    this->potDCash = dataOBJ.value("potDCash").toInt();
                    this->potALeft = dataOBJ.value("potALeft").toInt();
                    this->potBLeft = dataOBJ.value("potBLeft").toInt();
                    this->potCLeft = dataOBJ.value("potCLeft").toInt();
                    this->potDLeft = dataOBJ.value("potDLeft").toInt();
                    this->maxBetPerPot = dataOBJ.value("maxBetPerPot").toInt();

                    QString playersStr = dataOBJ.value("players").toString();
                    vector<string> playersVec = arrayToVector(playersStr.toStdString());

                    ui->playerList->clear();
                    for (string &pName : playersVec){
                        ui->playerList->addItem(QString::fromStdString(pName));
                    }

                    QString msgStr = dataOBJ.value("timedMSGStr").toString();
                    flagErrors(msgStr, 0);
                    QString currentPlayer = dataOBJ.value("currentPlayer").toString();
                    this->currentTurn = currentTurn;
                    if (currentPlayer == widgetsManager->getUserInfo()->getProfName()){
                        if (lastBetTurn != currentTurn){
                            setMyTurn();
                        }
                    }
                    else {
                        setLowLabelText("TURNS LEFT: " + QString::number(currentTurn));
                    }
                    updatePotLabels();

                    clientTimeout->stop();
                    clientTimeout->start(20000);
                }
                else if (topicValue == TOPIC::T_BET_REPLY){
                    QString playerState = dataOBJ.value("playerState").toString();
                    QString target = dataOBJ.value("target").toString();
                    if (playerState == "SERVER" && target == widgetsManager->getUserInfo()->getProfName()){
                        decreaseTotalPot(cashPerPot);
                        myTurn = true;
                    }
                }
                else if (topicValue == TOPIC::T_GAME_RESULT){
                    QString playerState = dataOBJ.value("playerState").toString();
                    if (playerState == "SERVER"){
                        this->potACash = dataOBJ.value("potACash").toInt();
                        this->potBCash = dataOBJ.value("potBCash").toInt();
                        this->potCCash = dataOBJ.value("potCCash").toInt();
                        this->potDCash = dataOBJ.value("potDCash").toInt();
                        this->potALeft = dataOBJ.value("potALeft").toInt();
                        this->potBLeft = dataOBJ.value("potBLeft").toInt();
                        this->potCLeft = dataOBJ.value("potCLeft").toInt();
                        this->potDLeft = dataOBJ.value("potDLeft").toInt();
                        this->maxBetPerPot = dataOBJ.value("maxBetPerPot").toInt();
                        updatePotLabels();

                        stopGame();
                        setLowLabelText("Betting has ended.");

                        QJsonObject potAResultsOBJ = qstringToJSONObject(dataOBJ.value("potAResults").toString());
                        QJsonObject potBResultsOBJ = qstringToJSONObject(dataOBJ.value("potBResults").toString());
                        QJsonObject potCResultsOBJ = qstringToJSONObject(dataOBJ.value("potCResults").toString());
                        QJsonObject potDResultsOBJ = qstringToJSONObject(dataOBJ.value("potDResults").toString());

                        QJsonObject potAWinnersOBJ = qstringToJSONObject(dataOBJ.value("potAWinners").toString());
                        QJsonObject potBWinnersOBJ = qstringToJSONObject(dataOBJ.value("potBWinners").toString());
                        QJsonObject potCWinnersOBJ = qstringToJSONObject(dataOBJ.value("potCWinners").toString());
                        QJsonObject potDWinnersOBJ = qstringToJSONObject(dataOBJ.value("potDWinners").toString());

                        for (QString &playerName : potAResultsOBJ.keys()){
                            potAResults[playerName] = potAResultsOBJ.value(playerName).toInt();
                        }
                        for (QString &playerName : potBResultsOBJ.keys()){
                            potBResults[playerName] = potBResultsOBJ.value(playerName).toInt();
                        }
                        for (QString &playerName : potCResultsOBJ.keys()){
                            potCResults[playerName] = potCResultsOBJ.value(playerName).toInt();
                        }
                        for (QString &playerName : potDResultsOBJ.keys()){
                            potDResults[playerName] = potDResultsOBJ.value(playerName).toInt();
                        }

                        for (QString &playerName : potAWinnersOBJ.keys()){
                            potAWinners[playerName] = potAWinnersOBJ.value(playerName).toInt();
                        }
                        for (QString &playerName : potBWinnersOBJ.keys()){
                            potBWinners[playerName] = potBWinnersOBJ.value(playerName).toInt();
                        }
                        for (QString &playerName : potCWinnersOBJ.keys()){
                            potCWinners[playerName] = potCWinnersOBJ.value(playerName).toInt();
                        }
                        for (QString &playerName : potDWinnersOBJ.keys()){
                            potDWinners[playerName] = potDWinnersOBJ.value(playerName).toInt();
                        }

                        int totalIncrease = 0;
                        for (QString &playerName : potAWinners.keys()){
                            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                                totalIncrease += potAWinners[playerName];
                            }
                        }
                        for (QString &playerName : potBWinners.keys()){
                            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                                totalIncrease += potBWinners[playerName];
                            }
                        }
                        for (QString &playerName : potCWinners.keys()){
                            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                                totalIncrease += potCWinners[playerName];
                            }
                        }
                        for (QString &playerName : potDWinners.keys()){
                            if (playerName == widgetsManager->getUserInfo()->getProfName()){
                                totalIncrease += potDWinners[playerName];
                            }
                        }

                        gameResultsTimer->start(4000);
                    }
                }
            }
        }
    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
}

void Jackpots::removePlayerFromList(QString player)
{
    int indToRemove = -1;
    for (int i = 0; i < ui->playerList->count(); ++i){
        QListWidgetItem* item = ui->playerList->item(i);
        if (item->text() == player){
            indToRemove = i;
            break;
        }
    }
    if (indToRemove != -1){
        while (turnsList.removeOne(player)){
            currentTurn--;
        }
        timedMSGTimer->start(7500);
        flagErrors(timedMSGStr, 0);
        ui->playerList->takeItem(indToRemove);
        delete playerTimers[player];
        playerTimers.remove(player);
    }
}

void Jackpots::hideEvent(QHideEvent *)
{

}

void Jackpots::on_quit_clicked()
{
    disableButtons(true);
    stopGame();
    if (!gameResultsFinish){
        QMessageBox::StandardButton buddyBox;
        buddyBox = QMessageBox::question(this, "QUIT", "Are you sure you want to quit?",
                                         QMessageBox::Yes|QMessageBox::No);

        if (buddyBox == QMessageBox::Yes) {
            resetUI();
            disconnect(this);
            widgetsManager->showWidget(JACKPOTS_HOME);
        }
        else {

        }
    }
    else {
        resetUI();
        disconnect(this);
        widgetsManager->showWidget(JACKPOTS_HOME);
    }
}

void Jackpots::on_potA_clicked()
{
    disableAllPots();
    sendClientBetMessage(1, currentTurn);
    myPotABets++;

    coinSound->play();
}

void Jackpots::on_potB_clicked()
{
    disableAllPots();
    sendClientBetMessage(2, currentTurn);
    myPotBBets++;

    coinSound->play();
}

void Jackpots::on_potC_clicked()
{
    disableAllPots();
    sendClientBetMessage(3, currentTurn);
    myPotCBets++;

    coinSound->play();
}

void Jackpots::on_potD_clicked()
{
    disableAllPots();
    sendClientBetMessage(4, currentTurn);
    myPotDBets++;

    coinSound->play();
}

void Jackpots::resetTimedMSG()
{
    timedMSGStr = "";
    timedMSGTimer->stop();
}

void Jackpots::sendClientBetMessage(int potNumber, int turnNum)
{
    if (playerState == "SERVER"){
        myTurn = false;
        lastBetTurn = currentTurn;
        lastPotBet = potNumber;

        POT betPOT;
        switch (potNumber){
        case 1: betPOT = POT_A; break;
        case 2: betPOT = POT_B; break;
        case 3: betPOT = POT_C; break;
        case 4: betPOT = POT_D; break;
        default: break;
        }
        placePotBet(hostName, betPOT);
        decreaseTotalPot(cashPerPot);
        myTurn = true;
    }
    else if (playerState == "CLIENT"){
        myTurn = false;
        lastBetTurn = currentTurn;
        lastPotBet = potNumber;
        QJsonObject cStateOBJ;
        cStateOBJ["topic"] = TOPIC::T_BET;
        cStateOBJ["playerState"] = playerState;
        cStateOBJ["potNumber"] = potNumber;
        cStateOBJ["turnNum"] = turnNum;
        sendMessage(jsonObjectToQString(cStateOBJ));
    }
}
