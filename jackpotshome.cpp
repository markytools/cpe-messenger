#include "jackpotshome.h"
#include "ui_jackpotshome.h"

JackpotsHome::JackpotsHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JackpotsHome)
{
    ui->setupUi(this);
    subtopic = "jackpots.rooms";
    channelName = "jackpotsrooms";
    ui->response->setWordWrap(true);
    joinGameTimer = new QTimer(this);
    hostSignalTimer = new QTimer(this);
    contManager = new QNetworkAccessManager(this);

    connect(joinGameTimer, SIGNAL(timeout()), SLOT(hostConnectionError()));
    connect(hostSignalTimer, SIGNAL(timeout()), SLOT(checkForHostSignal()));
}

void JackpotsHome::setJackpotsRoom(JackpotsRoom *value)
{
    jackpotsRoom = value;
}


void JackpotsHome::sendMessage(QString message, bool disable)
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
        flagError("Error sending message", 0);
    }
    disableButtons(disable);
    reply->deleteLater();
}

void JackpotsHome::addGameToList(QString gameName, QString publisherID)
{
    bool hasName = false;
    for (int i = 0; i < ui->listOfGames->count(); ++i){
        QListWidgetItem* item = ui->listOfGames->item(i);
        if (ifStringHasSubstring(item->text().toStdString(), publisherID.toStdString())
                || ifStringHasSubstring(gameName.toStdString(), widgetsManager->getUserInfo()->getProfName().toStdString())){
            hasName = true;
            break;
        }
    }
    if (!hasName){
        ui->listOfGames->addItem(gameName);
        hostName[gameName] = publisherID;
    }
}

void JackpotsHome::resetUI()
{
    ui->listOfGames->clear();
}

JackpotsHome::~JackpotsHome()
{
    delete ui;
}
void JackpotsHome::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}
void JackpotsHome::setJackpots(Jackpots *value)
{
    jackpots = value;
}

void JackpotsHome::on_createB_clicked()
{
    if (ui->cashPot->text().toInt() * 10 > totalPot){
        flagError("Not enough cash.", 0);
        return;
    }

    jackpotsRoom->setChannelName(channelName);
    jackpotsRoom->setSubtopic(subtopic);
    jackpotsRoom->setPlayerState("SERVER");
    jackpotsRoom->setCashPerPot(ui->cashPot->text().toInt());
    jackpotsRoom->setSubscriptionID(subscriptionID);
    jackpotsRoom->setTotalPlayers(1);
    jackpotsRoom->setTotalPot(totalPot);
    widgetsManager->showWidget(JACKPOTS_ROOM);
}

void JackpotsHome::flagError(QString message, int state)
{
    ui->response->clear();
    switch (state){
    case 0: ui->response->setText("<font color='red'>" + message + "</font>"); break;
    case 1: ui->response->setText("<font color='green'>" + message + "</font>"); break;
    default: break;
    }
}

void JackpotsHome::disableButtons(bool disable)
{
    ui->backB->setDisabled(disable);
    ui->createB->setDisabled(disable);
    ui->refreshB->setDisabled(disable);
    ui->joinB->setDisabled(disable);
}

void JackpotsHome::refreshData()
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
        flagError("Connection Error", 0);
    }
    reply->deleteLater();
}

/////Call this upon refresh clicked.
void JackpotsHome::checkForRooms()
{
    flagError("", 0);
    disableButtons(true);
    ui->listOfGames->clear();
    hostName.clear();
    if (subscriptionID != NULL){
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/messaging/" + channelName + "/" + subscriptionID));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkReply *reply = manager->get(request);
        connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(listOfRooms(QNetworkReply*)));
    }
}

void JackpotsHome::listOfRooms(QNetworkReply *reply)
{
    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray messagesData = json.value("messages").toArray();
        for (int i = 0; i < messagesData.size(); i++){
            QJsonObject messagesDataObj = messagesData.at(i).toObject();
            QString publisherID = messagesDataObj.value("publisherId").toString();
//            qDebug() << "data: " << messagesDataObj.value("data").toString();
            QString messageStr = messagesDataObj.value("data").toString();
            QJsonObject messageObj = qstringToJSONObject(messageStr);
            QString topic = messageObj.value("topic").toString();
//            qDebug() << "topA: " << topic << "  " << "topB: " << TOPIC::T_GAME_ROOM;
            if (topic == TOPIC::T_GAME_ROOM){
                int totalPlayer = messageObj.value("totalPlayers").toInt();
                addGameToList(publisherID + "(" + QString::number(totalPlayer) + "/8)", publisherID);
            }
        }
    }
    else {
        flagError(reply->errorString(), 0);
    }
    disableButtons(false);
}

void JackpotsHome::showEvent(QShowEvent *)
{
    flagError("", 0);
    disableButtons(false);
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/CashData/" + widgetsManager->getUserInfo()->getCashDataID()));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        totalPot = json.value("cash").toInt();
        ui->totalPot->setText("My Total Pot: $" + QString::number(totalPot));
    }
    else {
        flagError(reply->errorString(), 0);
    }

    refreshData();
    checkForRooms();
}

void JackpotsHome::hideEvent(QHideEvent *)
{
    disableButtons(true);
    resetUI();
}

void JackpotsHome::on_refreshB_clicked()
{
    checkForRooms();
}

void JackpotsHome::on_joinB_clicked()
{
    flagError("", 0);
    if (ui->listOfGames->currentItem() != NULL){
        disableButtons(true);
        string hostNameSelected = ui->listOfGames->currentItem()->text().toStdString();
        removeSubstring(hostNameSelected, "/8)");
        int totalPlayers = QString::fromStdString(hostNameSelected.substr(hostNameSelected.size() - 1, 1)).toInt();
        if (totalPlayers < 8){
            QJsonObject joinGameOBJ;
            joinGameOBJ["topic"] = TOPIC::T_JOIN_ROOM;
            joinGameOBJ["playerState"] = "CLIENT";
            joinGameOBJ["target"] = hostName[ui->listOfGames->currentItem()->text()];
            sendMessage(jsonObjectToQString(joinGameOBJ), true);
            joinGameTimer->start(20000);
            hostSignalTimer->start(1000);
        }
        else {
            flagError("Game is Full.", 0);
        }
    }
}

void JackpotsHome::hostConnectionError()
{
    disableButtons(false);
    flagError("Error connecting to host.", 0);
    joinGameTimer->stop();
    hostSignalTimer->stop();
}

void JackpotsHome::checkForHostSignal()
{
    if (subscriptionID != NULL){
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/messaging/" + channelName + "/" + subscriptionID));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkReply *reply = manager->get(request);
        connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(hostSignalReceived(QNetworkReply*)));
    }
}

void JackpotsHome::hostSignalReceived(QNetworkReply *reply)
{
    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray messagesData = json.value("messages").toArray();
        for (int i = 0; i < messagesData.size(); i++){
            QJsonObject messagesDataObj = messagesData.at(i).toObject();
            QString publisherID = messagesDataObj.value("publisherId").toString();
//            qDebug() << "data: " << messagesDataObj.value("data").toString();
            QString messageStr = messagesDataObj.value("data").toString();
            QJsonObject messageObj = qstringToJSONObject(messageStr);
            QString topic = messageObj.value("topic").toString();
            if (topic == TOPIC::T_ROOM_JOINED){
                QString player = messageObj.value("player").toString();
                int cashPerPot = messageObj.value("cashPerPot").toInt();
                if (player == widgetsManager->getUserInfo()->getProfName()){
                    QString players = messageObj.value("players").toString();
                    vector<string> playersVec = arrayToVector(players.toStdString());

                    qDebug() << "ysea";
                     jackpotsRoom->setPlayerState(false);
                    jackpotsRoom->setChannelName(channelName);
                    jackpotsRoom->setSubtopic(subtopic);
                    jackpotsRoom->setCashPerPot(cashPerPot);
                    jackpotsRoom->setHostName(publisherID);
                    jackpotsRoom->setSubscriptionID(subscriptionID);
                    jackpotsRoom->setPlayers(playersVec);
                    jackpotsRoom->setTotalPot(totalPot);
                    widgetsManager->showWidget(JACKPOTS_ROOM);

                    joinGameTimer->stop();
                    hostSignalTimer->stop();
                }
            }
        }
    }
    else {
        disableButtons(false);
        flagError(reply->errorString(), 0);
    }
}

void JackpotsHome::on_backB_clicked()
{
    widgetsManager->showWidget(MSGR_HOME);
}
