#include "buddies.h"
#include "ui_buddies.h"

Buddies::Buddies(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Buddies)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
}

Buddies::~Buddies()
{
    delete ui;
}

void Buddies::on_unbuddy_clicked()
{
    flagErrors("", 0);
    if (ui->buddyList->count() != 0){
        disableButtons(true);
        QString myProfName = widgetsManager->getUserInfo()->getProfName();
        QString myBuddyID = widgetsManager->getUserInfo()->getBuddiesID();
        QString buddyName = ui->buddyList->currentItem()->text();

        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/data/Buddies/" + myBuddyID));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkReply* reply = manager->get(request);
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if (reply->error() == 0){
            QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
            QString myBuddiesStr = json.value("buddies").toString();
            QJsonObject myBuddiesObj = QJsonDocument::fromJson(myBuddiesStr.toUtf8()).object();
            QString buddyID = myBuddiesObj.value(buddyName).toString();

            myBuddiesObj.remove(buddyName);
            QString myPendingAcceptsStr = json.value("pendingAccepts").toString();
            QJsonObject myPendingAcceptsObj = QJsonDocument::fromJson(myPendingAcceptsStr.toUtf8()).object();
            myPendingAcceptsObj.remove(buddyName);
            QJsonDocument docA(myPendingAcceptsObj);
            QString strJsonA(docA.toJson(QJsonDocument::Compact));
            updateBuddiesList(myBuddyID, myBuddiesObj, strJsonA);

            //////////////////////////
            //////////////////////////
            //////////////////////////

            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/Buddies/" + buddyID));
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

            QNetworkReply* reply2 = manager->get(request);
            QEventLoop loop;
            QObject::connect(reply2, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply2->error() == 0){
                QJsonObject json2 = QJsonDocument::fromJson(reply2->readAll()).object();
                QString buddiesStr = json2.value("buddies").toString();
                QJsonObject buddiesObj = QJsonDocument::fromJson(buddiesStr.toUtf8()).object();
                buddiesObj.remove(myProfName);

                QString pendingAcceptsStr = json2.value("pendingAccepts").toString();
                QJsonObject pendingAcceptsObj = QJsonDocument::fromJson(pendingAcceptsStr.toUtf8()).object();
                pendingAcceptsObj.remove(myProfName);
                QJsonDocument docB(pendingAcceptsObj);
                QString strJsonB(docB.toJson(QJsonDocument::Compact));

                updateBuddiesList(buddyID, buddiesObj, strJsonB);
                updateBuddies();
            }
            else {
                flagErrors(reply2->errorString(), 0);
            }
            reply2->deleteLater();
        }
        else {
            flagErrors(reply->errorString(), 0);
        }
        disableButtons(false);
        reply->deleteLater();
    }
}

void Buddies::disableButtons(bool disable)
{
    ui->backB->setDisabled(disable);
    ui->chat->setDisabled(disable);
    ui->addBuddy->setDisabled(disable);
    ui->unbuddy->setDisabled(disable);
    ui->buddyName->setDisabled(disable);
}

void Buddies::resetUI()
{
    ui->buddyList->clear();
    ui->buddyName->clear();
    ui->response->clear();
}

void Buddies::flagErrors(QString message, int state)
{
    switch (state){
    case 0: ui->response->setText("<font color='red'>" + message + "</font>"); break;
    case 1: ui->response->setText("<font color='green'>" + message + "</font>"); break;
    default: break;
    }
}

void Buddies::updateBuddiesList(QString &objectID, QJsonObject &buddiesList, QString &pendingAccepts)
{
    QJsonDocument doc(buddiesList);
    QString strJson(doc.toJson(QJsonDocument::Compact));

    QVariantMap testMapData;
    testMapData["buddies"] = strJson;
    testMapData["pendingAccepts"] = pendingAccepts;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Buddies/" + objectID));
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

void Buddies::updateBuddies()
{
    ui->buddyList->clear();
    QString myBuddyID = widgetsManager->getUserInfo()->getBuddiesID();

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Buddies/" + myBuddyID));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QString myBuddiesStr = json.value("buddies").toString();
        QJsonObject myBuddiesObj = QJsonDocument::fromJson(myBuddiesStr.toUtf8()).object();

        foreach (QString key, myBuddiesObj.keys()){
            ui->buddyList->addItem(key);
        }
    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
}

void Buddies::on_addBuddy_clicked()
{
    flagErrors("", 0);
    if (!ui->buddyName->text().isEmpty()){
        QString myProfName = widgetsManager->getUserInfo()->getProfName();
        QString buddyToAdd = ui->buddyName->text();
        if (buddyToAdd == myProfName){
            flagErrors("Invalid buddy name.", 0);
            return;
        }
        for (int i = 0; i < ui->buddyList->count(); i++){
            if (ui->buddyName->text() == ui->buddyList->item(i)->text()){
                flagErrors("Already buddies.", 0);
                return;
            }
        }

        disableButtons(true);
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/data/Buddies?where=profileName%20%3D%20%27" + buddyToAdd + "%27"));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkReply* reply = manager->get(request);
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if (reply->error() == 0){
            QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
            QJsonArray buddiesDataList = json.value("data").toArray();
            if (buddiesDataList.count() != 0){
                QJsonObject buddiesDataObj = buddiesDataList.at(0).toObject();
                QString objectID = buddiesDataObj.value("objectId").toString();
                QString pendingAcceptsStr = buddiesDataObj.value("pendingAccepts").toString();
                QJsonObject pendingAcceptsObj = QJsonDocument::fromJson(pendingAcceptsStr.toUtf8()).object();
                if (pendingAcceptsObj.value(myProfName) == QJsonValue::Undefined){
                    pendingAcceptsObj[myProfName] = "pending";
                }
                else {
                    if (pendingAcceptsObj.value(myProfName).toString() != "accepted"){
                        pendingAcceptsObj[myProfName] = "pending";
                    }
                }
                QJsonDocument doc(pendingAcceptsObj);
                QString strJson(doc.toJson(QJsonDocument::Compact));

                QVariantMap testMapData;
                testMapData["pendingAccepts"] = strJson;

                QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

                QNetworkAccessManager *manager = new QNetworkAccessManager(this);
                QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                             + "/data/Buddies/" + objectID));
                request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
                request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

                QNetworkReply* reply = manager->put(request, testJsonData.toJson());
                QEventLoop loop;
                connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
                loop.exec();

                if (reply->error() == 0){
                    flagErrors("Buddy request sent.", 1);
                }
                else {
                    flagErrors(reply->errorString(), 0);
                }
                reply->deleteLater();
            }
            else {
                flagErrors("User does not exist.", 0);
            }
        }
        else {
            flagErrors(reply->errorString(), 0);
        }
        disableButtons(false);
        reply->deleteLater();
    }
}

void Buddies::showEvent(QShowEvent *)
{
    updateBuddies();
}

void Buddies::hideEvent(QHideEvent *)
{

}

void Buddies::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}

void Buddies::on_chat_clicked()
{
    flagErrors("", 0);
    if (ui->buddyList->currentItem() != NULL){
        disableButtons(true);
        QString myProfname = widgetsManager->getUserInfo()->getProfName();
        QString buddyName = ui->buddyList->currentItem()->text();
        QStringList profNames;
        profNames.push_back(myProfname);
        profNames.push_back(buddyName);
        profNames.sort();

        groupNotifications->setTitleName("Chatting w/ " + buddyName);
        groupNotifications->setChannelName(profNames.at(0) + profNames.at(1));
        groupNotifications->setSubtopic(profNames.at(1) + "." + profNames.at(0) + ".buddychat");
        groupNotifications->initSubscription();
        groupNotifications->setGNState(BUDDY_CHAT);
        widgetsManager->showWidget(MESSAGING);
        disableButtons(false);
    }
}

void Buddies::on_backB_clicked()
{
    widgetsManager->showWidget(MSGR_HOME);
    resetUI();
}
void Buddies::setGroupNotifications(GroupNotifications *value)
{
    groupNotifications = value;
}

