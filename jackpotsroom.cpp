#include "jackpotsroom.h"
#include "ui_jackpotsroom.h"

JackpotsRoom::JackpotsRoom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JackpotsRoom)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
    channelManager = new QNetworkAccessManager(this);
    channelSenderTimer = new QTimer(this);
    channelReceiverTimer = new QTimer(this);
    roomCreatedT = new QTimer(this);
    clientTimeout = new QTimer(this);
    signalMapper = new QSignalMapper(this);
    startGameTimeout = new QTimer(this);
    startGameTimer = new QTimer(this);
}

void JackpotsRoom::disconnectFromServer()
{
    clientTimeout->stop();
    ui->playerList->clear();
    QMessageBox::StandardButton connectionErrorBox;
    connectionErrorBox = QMessageBox::information(this, "Error", "Connection to Server Lost",
                                                  QMessageBox::Ok);
    if (connectionErrorBox == QMessageBox::Ok) {
        resetUI();
        widgetsManager->showWidget(JACKPOTS_HOME);
    }
}

void JackpotsRoom::signalRoomCreated()
{
    QJsonObject hostJSON;
    hostJSON["topic"] = TOPIC::T_GAME_ROOM;
    hostJSON["playerState"] = "SERVER";
    hostJSON["totalPlayers"] = totalPlayers;
    hostJSON["cashPerPot"] = cashPerPot;

    QVariantMap testMapData;
    testMapData["message"] = jsonObjectToQString(hostJSON);
    testMapData["publisherId"] = widgetsManager->getUserInfo()->getProfName();
    testMapData["subtopic"] = subtopic;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/messaging/" + channelName));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    manager->post(request, testJsonData.toJson());
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(roomCreatedSent(QNetworkReply*)));
}

void JackpotsRoom::roomCreatedSent(QNetworkReply* reply)
{
    if (reply->error() == 0){

    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
}

JackpotsRoom::~JackpotsRoom()
{
    delete ui;
}
void JackpotsRoom::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}
void JackpotsRoom::setJackpots(Jackpots *value)
{
    jackpots = value;
}
void JackpotsRoom::setChannelName(const QString &value)
{
    channelName = value;
}
void JackpotsRoom::setSubtopic(const QString &value)
{
    subtopic = value;
}
void JackpotsRoom::setSubscriptionID(const QString &value)
{
    subscriptionID = value;
}

void JackpotsRoom::startRoomCreationSignal()
{
    roomCreatedT->start(3000);
}
void JackpotsRoom::setCashPerPot(int value)
{
    cashPerPot = value;
}
void JackpotsRoom::setTotalPlayers(int value)
{
    totalPlayers = value;
}

void JackpotsRoom::addPlayerToList(QString playerName, QTimer *timer)
{
    bool hasName = false;
    for (int i = 0; i < ui->playerList->count(); ++i){
        QListWidgetItem* item = ui->playerList->item(i);
        if (item->text() == playerName){
            hasName = true;
            break;
        }
    }
    if (!hasName){
        totalPlayers++;
        flagErrors(playerName + " has joined.", 1);
        ui->playerList->addItem(playerName);
        playerTimers[playerName] = timer;
        readyPlayers[playerName] = false;
    }

}

void JackpotsRoom::removePlayerFromList(QString player)
{
    flagErrors(player + " has left.", 0);
    int indToRemove;
    for (int i = 0; i < ui->playerList->count(); ++i){
        QListWidgetItem* item = ui->playerList->item(i);
        if (item->text() == player){
            indToRemove = i;
            break;
        }
    }
    ui->playerList->takeItem(indToRemove);
    delete playerTimers[player];
    playerTimers.remove(player);
    readyPlayers.remove(player);
    totalPlayers--;
}

void JackpotsRoom::flagErrors(QString message, int state)
{
    if (state == 0){
        ui->response->setText("<font color='red'>" + message + "</font>");
    }
    else {
        ui->response->setText("<font color='green'>" + message + "</font>");
    }
}

void JackpotsRoom::disableButtons(bool disable)
{
    ui->back->setDisabled(disable);
    ui->start->setDisabled(disable);
}

void JackpotsRoom::resetUI()
{
    channelSenderTimer->stop();
    roomCreatedT->stop();
    clientTimeout->stop();

    ui->response->clear();
    ui->playerList->clear();
    ui->hostName->clear();
}

void JackpotsRoom::setPlayerState(bool isServer)
{
    if (isServer){
        playerState = "SERVER";
        roomCreatedT->start(2000);
    }
    else {
        playerState = "CLIENT";
    }
}
void JackpotsRoom::setHealthyConnection(bool value)
{
    healthyConnection = value;
}

void JackpotsRoom::setHostName(QString hostName)
{
    this->hostName = hostName;
    ui->hostName->setText("<font color='brown'>HOST: " + hostName + "</font>");
}

void JackpotsRoom::setPlayers(vector<string> players)
{
    ui->playerList->clear();
    for (string &player : players){
        bool hasName = false;
        for (int i = 0; i < ui->playerList->count(); ++i){
            QListWidgetItem* item = ui->playerList->item(i);
            if (item->text() == QString::fromStdString(player)){
                hasName = true;
                break;
            }
        }
        if (!hasName) ui->playerList->addItem(QString::fromStdString(player));
    }
}
void JackpotsRoom::setTotalPot(int value)
{
    totalPot = value;
}

void JackpotsRoom::manageChannelMessages()
{
    if (subscriptionID != NULL){
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/messaging/" + channelName + "/" + subscriptionID));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        channelManager->get(request);
        connect(channelManager, SIGNAL(finished(QNetworkReply*)), SLOT(handleReceivedMsgs(QNetworkReply*)));
    }
}

void JackpotsRoom::handleReceivedMsgs(QNetworkReply *reply)
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

                if (totalPlayers < 8 && playerTimers[publisherID] == NULL && topicValue == TOPIC::T_JOIN_ROOM){
                    QString target = dataOBJ.value("target").toString();
                    if (target == widgetsManager->getUserInfo()->getProfName()){
                        QJsonObject replyOBJ;
                        replyOBJ["topic"] = TOPIC::T_ROOM_JOINED;
                        replyOBJ["playerState"] = playerState;
                        replyOBJ["cashPerPot"] = cashPerPot;
                        replyOBJ["player"] = publisherID;

                        vector<string> playerList;
                        playerList.push_back(widgetsManager->getUserInfo()->getProfName().toStdString());
                        for (QString &player : playerTimers.keys()) playerList.push_back(player.toStdString());
                        playerList.push_back(publisherID.toStdString());
                        replyOBJ["players"] = QString::fromStdString(arrayToString(playerList));

                        sendMessage(jsonObjectToQString(replyOBJ));
                        QTimer *pTimer = new QTimer(this);
                        addPlayerToList(publisherID, pTimer);

                        connect(pTimer, SIGNAL(timeout()), signalMapper, SLOT(map()));
                        signalMapper->setMapping (pTimer, publisherID) ;
                        connect(signalMapper, SIGNAL(mapped(QString)), this,
                                SLOT(removePlayerFromList(QString)));

                        pTimer->start(20000);
                    }
                }
                else if (topicValue == TOPIC::T_PRESENT){
                    QString playerState = dataOBJ.value("playerState").toString();
                    QString target = dataOBJ.value("target").toString();
                    if (playerState == "CLIENT" && target == widgetsManager->getUserInfo()->getProfName()){
                        QTimer *pTimer = playerTimers[publisherID];
                        pTimer->stop();
                        pTimer->start(20000);
                    }
                }
                else if (topicValue == TOPIC::T_START_READY){
                    QString playerState = dataOBJ.value("playerState").toString();
                    QString target = dataOBJ.value("target").toString();
                    if (playerState == "CLIENT" && target == widgetsManager->getUserInfo()->getProfName()){
                        readyPlayers[publisherID] = true;
                    }
                }
            }
            else if (playerState == "CLIENT"){

                if (topicValue == TOPIC::T_PRESENT){
                    QString playerState = dataOBJ.value("playerState").toString();
                    if (playerState == "SERVER" && hostName == publisherID && receivePresentStates){
                        QString players = dataOBJ.value("players").toString();
                        vector<string> playersVec = arrayToVector(players.toStdString());
                        ui->playerList->clear();
                        for (string &name : playersVec){
                            bool hasName = false;
                            for (int i = 0; i < ui->playerList->count(); ++i){
                                QListWidgetItem* item = ui->playerList->item(i);
                                if (item->text() == QString::fromStdString(name)){
                                    hasName = true;
                                    break;
                                }
                            }
                            if (!hasName) ui->playerList->addItem(QString::fromStdString(name));
                        }
                        clientTimeout->stop();
                        clientTimeout->start(20000);
                    }
                }
                else if (topicValue == TOPIC::T_START_GAME){
                    QString playerState = dataOBJ.value("playerState").toString();
                    if (playerState == "SERVER" && hostName == publisherID){
                        clientTimeout->stop();
                        receivePresentStates = false;
                        QJsonObject cStateOBJ;
                        cStateOBJ["topic"] = TOPIC::T_START_READY;
                        cStateOBJ["playerState"] = "CLIENT";
                        cStateOBJ["target"] = hostName;
                        sendMessage(jsonObjectToQString(cStateOBJ));
                        flagErrors("Starting game...", 1);
                    }
                }
                else if (topicValue == TOPIC::T_START){
                    QString playerState = dataOBJ.value("playerState").toString();
                    if (playerState == "SERVER" && hostName == publisherID){

                        clientTimeout->stop();
                        disconnect(this);
                        jackpots->setPlayerState(this->playerState);
                        jackpots->setChannelName("jackpots" + hostName);
                        jackpots->setSubtopic("jackpots.ingame");
                        jackpots->setHostName(hostName);
                        jackpots->setTotalPot(totalPot);
                        jackpots->setCashPerPot(cashPerPot);
                        jackpots->setQuitShown(false);
                        widgetsManager->showWidget(JACKPOTS);
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

void JackpotsRoom::sendMessage(QString message)
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

void JackpotsRoom::initRoom()
{
    connect(clientTimeout, SIGNAL(timeout()), SLOT(disconnectFromServer()));
    connect(channelSenderTimer, SIGNAL(timeout()), SLOT(evaluateConnections()));
    connect(channelReceiverTimer, SIGNAL(timeout()), SLOT(manageChannelMessages()));
    connect(roomCreatedT, SIGNAL(timeout()), SLOT(signalRoomCreated()));
    connect(startGameTimer, SIGNAL(timeout()), SLOT(startGame()));
    connect(startGameTimeout, SIGNAL(timeout()), SLOT(startGameFail()));

    if (playerState == "SERVER"){
        channelSenderTimer->start(3000);
        roomCreatedT->start(3000);
        channelReceiverTimer->start(3000);
        ui->playerList->addItem(widgetsManager->getUserInfo()->getProfName());
        setHostName(widgetsManager->getUserInfo()->getProfName());
        ui->start->setDisabled(false);
    }
    else if (playerState == "CLIENT"){
        channelSenderTimer->start(3000);
        clientTimeout->start(20000);
        channelReceiverTimer->start(3000);
        ui->start->setDisabled(true);
    }
}

void JackpotsRoom::evaluateConnections()
{
    if (playerState == "SERVER"){
        vector<string> playerList;
        for (QString &player : playerTimers.keys()) playerList.push_back(player.toStdString());
        playerList.push_back(widgetsManager->getUserInfo()->getProfName().toStdString());
        QJsonObject cStateOBJ;
        cStateOBJ["topic"] = TOPIC::T_PRESENT;
        cStateOBJ["playerState"] = playerState;
        cStateOBJ["players"] = QString::fromStdString(arrayToString(playerList));
        sendMessage(jsonObjectToQString(cStateOBJ));
    }
    else if (playerState == "CLIENT"){
        QJsonObject cStateOBJ;
        cStateOBJ["topic"] = TOPIC::T_PRESENT;
        cStateOBJ["playerState"] = playerState;
        cStateOBJ["target"] = hostName;
        sendMessage(jsonObjectToQString(cStateOBJ));
    }
}

void JackpotsRoom::showEvent(QShowEvent *)
{
    flagErrors("", 0);
    receivePresentStates = true;
    disableButtons(false);
    initRoom();
    ui->cashPerPotLabel->setText("<font color='brown'>CASH PER POT: $" + QString::number(cashPerPot) + "</font>");
}

void JackpotsRoom::hideEvent(QHideEvent *)
{
    resetUI();
    clientTimeout->stop();
    channelSenderTimer->stop();
    channelReceiverTimer->stop();
    roomCreatedT->stop();
    startGameTimer->stop();
    startGameTimeout->stop();
    for (QString &player : playerTimers.keys()) delete playerTimers[player];
}

void JackpotsRoom::on_back_clicked()
{
    disableButtons(true);
    widgetsManager->showWidget(JACKPOTS_HOME);
    resetUI();
    channelSenderTimer->stop();
    channelReceiverTimer->stop();
    roomCreatedT->stop();
    clientTimeout->stop();
}

void JackpotsRoom::on_start_clicked()
{
    flagErrors("Starting game...", 1);
    if (playerTimers.size() == 0){
        flagErrors("Not enough players.", 0);
        return;
    }
    disableButtons(true);
    QJsonObject cStateOBJ;
    cStateOBJ["topic"] = TOPIC::T_START_GAME;
    cStateOBJ["playerState"] = playerState;
    sendMessage(jsonObjectToQString(cStateOBJ));
    startGameTimer->start(2000);
    startGameTimeout->start(20000);
}

void JackpotsRoom::startGame()
{
    if (readyPlayers.size() == 0){
        flagErrors("Unable to start. Low number of players.", 0);
    }
    bool start = true;
    for (QString &playerName : readyPlayers.keys()){
        if (readyPlayers[playerName] == false){
            start = false;
            break;
        }
    }
    if (start){
        startGameTimer->stop();
        startGameTimeout->stop();
        QJsonObject cStateOBJ;
        cStateOBJ["topic"] = TOPIC::T_START;
        cStateOBJ["playerState"] = playerState;
        sendMessage(jsonObjectToQString(cStateOBJ));
        disconnect(signalMapper);
        disconnect(this);

        clientTimeout->stop();
        jackpots->setPlayerState(playerState);
        jackpots->setChannelName("jackpots" + hostName);
        jackpots->setSubtopic("jackpots.ingame");
        jackpots->setPlayerTimers(playerTimers);
        jackpots->setCashPerPot(cashPerPot);
        jackpots->setHostName(hostName);
        jackpots->setTotalPot(totalPot);
        jackpots->setQuitShown(false);
        widgetsManager->showWidget(JACKPOTS);
    }
}

void JackpotsRoom::startGameFail()
{
    disableButtons(false);
    startGameTimer->stop();
    startGameTimeout->stop();
    flagErrors("Error starting the game.", 0);
}
