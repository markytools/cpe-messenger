#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    ui->response->setWordWrap(true);
    questions.push_back("What was the last name of your third grade teacher?");
    questions.push_back("What was the name of the boy/girl you had your second kiss with?");
    questions.push_back("Where were you when you had your first kiss?");
    questions.push_back("When you were young, what did you want to be when you grew up?");
    questions.push_back("Who was your childhood hero?");
    questions.push_back("What is your oldest cousin's first/last name?");
    questions.push_back("What was the name of your elementary/primary school?");
    questions.push_back("What are the last 5 digits of your driver's license number?");
}

Login::~Login()
{
    delete ui;
}
void Login::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}


void Login::on_login_clicked()
{
    if (ui->email->text().isEmpty() || ui->password->text().isEmpty()){
        flagErrors("Please fill all empty fields.");
        return;
    }

    ui->response->setText("<font color='green'>Loading...</font>");
    currentLoginPassword = ui->password->text();

    disableButtons();
    QVariantMap testMapData;
    testMapData["login"] = ui->email->text();
    testMapData["password"] = currentLoginPassword;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY + "/users/login"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->post(request, testJsonData.toJson());
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QString profileName = json.value("profileName").toString();
        QString email = json.value("email").toString();
        QString userID = json.value("objectId").toString();
        QString userToken = json.value("user-token").toString();
        bool hasFilesData = json.value("hasFilesData").toBool();
        bool hasCashData = json.value("hasCashData").toBool();
        bool hasBuddiesData = json.value("hasBuddiesData").toBool();
        int maxBet = 500;
        int cash = 20000;
        QString filesDataID;
        QString cashDataID;
        QString buddiesDataID;
        vector<FileData> files;

        UserInfo *userInfo;
        if (!hasFilesData){
            QJsonObject jsonObj;
            QJsonDocument *jsonDoc = new QJsonDocument(jsonObj);

            QVariantMap testMapData;
            testMapData["profileName"] = profileName;
            testMapData["files"] = jsonDoc->toJson();

            QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/Files"));
            request.setRawHeader("user-token", userToken.toUtf8());
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->post(request, testJsonData.toJson());
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
                filesDataID = json.value("objectId").toString();
            }
            else {
                enableButtons();
                flagErrors(reply->errorString());
                return;
            }
        }
        else {
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/Files?where=profileName%20%3D%20%27" + profileName + "%27"));
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->get(request);
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
                QJsonArray filesDataList = json.value("data").toArray();
                QJsonObject filesOBJ = filesDataList.at(0).toObject();
                filesDataID = filesOBJ.value("objectId").toString();
                QString filesData = filesOBJ.value("files").toString();
                QJsonDocument doc = QJsonDocument::fromJson(filesData.toUtf8());
                QJsonObject filesDataJSON = doc.object();
                QStringList filesDataKeys = filesDataJSON.keys();
                foreach (QString key, filesDataKeys){
                    FileData data;
                    data.fileName = key;
                    data.fileURL = filesDataJSON.value(key).toString();
                    files.push_back(data);
                }
            }
            else {
                enableButtons();
                flagErrors(reply->errorString());
                return;
            }
        }

        if (!hasCashData){
            QVariantMap testMapData;
            testMapData["profileName"] = profileName;
            testMapData["maxBet"] = maxBet;
            testMapData["cash"] = cash;

            QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/CashData"));
            request.setRawHeader("user-token", userToken.toUtf8());
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->post(request, testJsonData.toJson());
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
                cashDataID = json.value("objectId").toString();
            }
            else {
                enableButtons();
                flagErrors(reply->errorString());
                return;
            }
        }
        else {
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/CashData?where=profileName%20%3D%20%27" + profileName + "%27"));
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->get(request);
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
                QJsonArray cashData = json.value("data").toArray();
                QJsonObject cashDataJSON = cashData.at(0).toObject();
                cashDataID = cashDataJSON.value("objectId").toString();
                maxBet = cashDataJSON.value("maxBet").toInt();
                cash = cashDataJSON.value("cash").toInt();
                qDebug() << "Success: " << cash;
                qDebug() << "Success: " << maxBet;
            }
            else {
                enableButtons();
                flagErrors(reply->errorString());
                return;
            }
        }

        if (!hasBuddiesData){
            QVariantMap testMapData;
            testMapData["profileName"] = profileName;
            testMapData["pendingAccepts"] = "{}";
            testMapData["buddies"] = "{}";

            QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/Buddies"));
            request.setRawHeader("user-token", userToken.toUtf8());
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->post(request, testJsonData.toJson());
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
                buddiesDataID = json.value("objectId").toString();
            }
            else {
                enableButtons();
                flagErrors(reply->errorString());
                return;
            }
        }
        else {
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/Buddies?where=profileName%20%3D%20%27" + profileName + "%27"));
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->get(request);
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
                QJsonArray buddiesData = json.value("data").toArray();
                QJsonObject buddiesDataJSON = buddiesData.at(0).toObject();
                buddiesDataID = buddiesDataJSON.value("objectId").toString();
            }
            else {
                enableButtons();
                flagErrors(reply->errorString());
                return;
            }
        }
        qDebug() << buddiesDataID;

        userInfo = new UserInfo(profileName, email, currentLoginPassword, userToken, userID, true, true, true, maxBet, cash, files);
        if (!hasBuddiesData){
            userInfo->updateUserProfile(this);
        }
        userInfo->setFilesDataID(filesDataID);
        userInfo->setCashDataID(cashDataID);
        userInfo->setBuddiesID(buddiesDataID);
        widgetsManager->setUserInfo(userInfo);
        resetUI();
        widgetsManager->showWidget(MSGR_HOME);
    }
    else {
        enableButtons();
        flagErrors("There was an error logging in.");
        return;
    }
    enableButtons();
}

void Login::flagErrors(QString message)
{
    ui->response->setText("<font color='red'>" + message + "</font>");
}

void Login::resetUI()
{
    ui->email->clear();
    ui->password->clear();
    ui->response->clear();
}

void Login::on_passwordForgetButton_clicked()
{
    flagErrors("");
    if (ui->email->text().isEmpty()){
        flagErrors("Please enter your email address.");
        return;
    }
    if (ui->password->text() != "forgotten"){
        flagErrors("Please type the word 'forgotten' in the password field.");
        return;
    }

    disableButtons();
    string email = ui->email->text().toStdString();
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Users?where=email%20%3D%20%27" + QString::fromStdString(url_encode(email)) + "%27"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply *reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray buddiesData = json.value("data").toArray();
        if (buddiesData.size() == 0){
            enableButtons();
            flagErrors("Invalid email.");
            return;
        }
        else {
            QJsonObject buddiesDataJSON = buddiesData.at(0).toObject();
            passwordRecovery->setIdentityQuestion(questions.at(buddiesDataJSON.value("question1").toInt()));
            passwordRecovery->setAnswer(buddiesDataJSON.value("answer1").toString());
            passwordRecovery->setEmail(QString::fromStdString(email));
            widgetsManager->showWidget(PASS_RECOVERY);
            resetUI();
            enableButtons();
        }
    }
    else {
        enableButtons();
        flagErrors(reply->errorString());
        return;
    }

    enableButtons();
    reply->deleteLater();
}
void Login::setPasswordRecovery(PasswordRecovery *value)
{
    passwordRecovery = value;
}


void Login::disableButtons()
{
    ui->login->setDisabled(true);
    ui->backB->setDisabled(true);
    ui->passwordForgetButton->setDisabled(true);
}

void Login::enableButtons()
{
    ui->login->setDisabled(false);
    ui->backB->setDisabled(false);
    ui->passwordForgetButton->setDisabled(false);
}

void Login::on_backB_clicked()
{
    resetUI();
    widgetsManager->showWidget(HOME);
}
