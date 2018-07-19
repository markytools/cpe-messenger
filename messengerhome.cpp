#include "messengerhome.h"
#include "ui_messengerhome.h"

MessengerHome::MessengerHome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessengerHome)
{
    ui->setupUi(this);
    setButtonSize(ui->myBuddies, 200, 200);
    ui->myBuddies->setIconSize(QSize(200, 200));
    setButtonSize(ui->myFiles, 200, 200);
    ui->myFiles->setIconSize(QSize(200, 200));
    setButtonSize(ui->myGroups, 200, 200);
    ui->myGroups->setIconSize(QSize(200, 200));
    setButtonSize(ui->jackpots, 200, 200);
    ui->jackpots->setIconSize(QSize(200, 200));

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(checkForFriendRequest()));
}

MessengerHome::~MessengerHome()
{
    delete ui;
}

void MessengerHome::on_myFiles_clicked()
{
    widgetsManager->showWidget(MY_FILES);
}
void MessengerHome::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}


void MessengerHome::on_myGroups_clicked()
{
    widgetsManager->showWidget(GROUP_SELECTION);
}

void MessengerHome::on_logout_clicked()
{
    disableButtons(true);
    QString tokenID = widgetsManager->getUserInfo()->getUserToken();

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/users/logout"));
    request.setRawHeader("user-token", tokenID.toUtf8());
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        widgetsManager->showWidget(HOME);
    }
    else {

    }
    disableButtons(false);
    reply->deleteLater();
}

void MessengerHome::disableButtons(bool disable)
{
    ui->myFiles->setDisabled(disable);
    ui->logout->setDisabled(disable);
    ui->myGroups->setDisabled(disable);
    ui->myBuddies->setDisabled(disable);
    ui->jackpots->setDisabled(disable);
}

void MessengerHome::checkForFriendRequest()
{
    mTimer->stop();
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 +"/data/Buddies/" + widgetsManager->getUserInfo()->getBuddiesID()));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QString myProfileName = widgetsManager->getUserInfo()->getProfName();
    QString mybuddyID = widgetsManager->getUserInfo()->getBuddiesID();
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QString pendingAcceptsStr = json.value("pendingAccepts").toString();
        QJsonObject pendingAcceptsObj = QJsonDocument::fromJson(pendingAcceptsStr.toUtf8()).object();
        foreach (QString key, pendingAcceptsObj.keys()){
            qDebug() << "asdad" << endl;

            QString buddyName = key;
            QString state = pendingAcceptsObj.value(key).toString();
            if (state == "pending"){
                disableButtons(true);
                QMessageBox::StandardButton buddyBox;
                buddyBox = QMessageBox::question(this, "Buddy Request", buddyName + " wants to be your buddy.",
                                                 QMessageBox::Yes|QMessageBox::No);
                if (buddyBox == QMessageBox::Yes) {
                    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                                 + "/data/Buddies?where=profileName%20%3D%20%27" + buddyName + "%27"));
                    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
                    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

                    QNetworkReply *reply2 = manager->get(request);
                    QEventLoop loop;
                    QObject::connect(reply2, SIGNAL(finished()), &loop, SLOT(quit()));
                    loop.exec();

                    if (reply2->error() == 0){
                        QJsonObject replyJSON = QJsonDocument::fromJson(reply2->readAll()).object();
                        QJsonArray buddiesDataList = replyJSON.value("data").toArray();
                        QJsonObject buddiesData = buddiesDataList.at(0).toObject();
                        QString buddiesStr = buddiesData.value("buddies").toString();
                        QString buddiesPendingAcceptsStr = buddiesData.value("pendingAccepts").toString();
                        QString buddyObjectID = buddiesData.value("objectId").toString();
                        QJsonObject buddiesJSONObj = QJsonDocument::fromJson(buddiesStr.toUtf8()).object();
                        buddiesJSONObj[myProfileName] = mybuddyID;
                        updateBuddiesList(buddyObjectID, buddiesJSONObj, buddiesPendingAcceptsStr);

                        pendingAcceptsObj[key] = "accepted";
                        QJsonDocument docA(pendingAcceptsObj);
                        QString strJsonA(docA.toJson(QJsonDocument::Compact));
                        QString mybuddiesStr = json.value("buddies").toString();
                        QJsonObject mybuddiesJSONObj = QJsonDocument::fromJson(mybuddiesStr.toUtf8()).object();
                        mybuddiesJSONObj[buddyName] = buddyObjectID;
                        updateBuddiesList(mybuddyID, mybuddiesJSONObj, strJsonA);
                    }
                    else {

                    }
                }
                else {;
                    pendingAcceptsObj[key] = "denied";
                    QJsonDocument doc(pendingAcceptsObj);
                    QString strJson(doc.toJson(QJsonDocument::Compact));

                    QVariantMap testMapData;
                    testMapData["pendingAccepts"] = strJson;

                    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

                    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                                 + "/data/Buddies/" + mybuddyID));
                    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
                    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

                    QNetworkReply *reply2 = manager->put(request, testJsonData.toJson());
                    QEventLoop loop;
                    connect(reply2, SIGNAL(finished()), &loop, SLOT(quit()));
                    loop.exec();

                    if (reply2->error() == 0){

                    }
                    else {

                    }
                }
                disableButtons(false);
            }
        }
    }
    else {

    }
    mTimer->start(3000);
    reply->deleteLater();
}

void MessengerHome::updateBuddiesList(QString &objectID, QJsonObject &buddiesList, QString &pendingAccepts)
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

    }
    reply->deleteLater();
}

void MessengerHome::setButtonSize(QWidget *a_pVictim, float ac_nWidth, float ac_nHeight)
{
    a_pVictim->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    a_pVictim->setMaximumWidth ( ac_nWidth  );
    a_pVictim->setMinimumWidth ( ac_nWidth  );
    a_pVictim->setMaximumHeight( ac_nHeight );
    a_pVictim->setMinimumHeight( ac_nHeight );

}

void MessengerHome::showEvent(QShowEvent *)
{
    mTimer->start(3000);
}

void MessengerHome::hideEvent(QHideEvent *)
{
    mTimer->stop();
}

void MessengerHome::on_MessengerHome_destroyed()
{

}

void MessengerHome::on_myBuddies_clicked()
{
    widgetsManager->showWidget(BUDDIES);
}

void MessengerHome::on_jackpots_clicked()
{
    widgetsManager->showWidget(JACKPOTS_HOME);
}
