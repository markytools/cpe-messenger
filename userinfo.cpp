#include "userinfo.h"


UserInfo::UserInfo(QString profName, QString email, QString password, QString userToken, QString userID, bool hasFilesData, bool hasCashData,
                   bool hasBuddiesData, int maxBet, int cash, vector<FileData> myFiles)
{
    this->profName = profName;
    this->email = email;
    this->password = password;
    this->userToken = userToken;
    this->userID = userID;
    this->profName = profName;
    this->hasFilesData = hasFilesData;
    this->hasCashData = hasCashData;
    this->hasBuddiesData = hasBuddiesData;
    this->maxBet = maxBet;
    this->cash = cash;
    this->myFiles = myFiles;
}


QString UserInfo::getEmail() const
{
    return email;
}

void UserInfo::setEmail(const QString &value)
{
    email = value;
}

void UserInfo::setPassword(const QString &value)
{
    password = value;
}

QString UserInfo::getUserToken() const
{
    return userToken;
}

void UserInfo::setUserToken(const QString &value)
{
    userToken = value;
}

QString UserInfo::getUserID() const
{
    return userID;
}

void UserInfo::setUserID(const QString &value)
{
    userID = value;
}

bool UserInfo::getHasFilesData() const
{
    return hasFilesData;
}

void UserInfo::setHasFilesData(bool value)
{
    hasFilesData = value;
}

bool UserInfo::getHasCashData() const
{
    return hasCashData;
}

void UserInfo::setHasCashData(bool value)
{
    hasCashData = value;
}

vector<FileData> UserInfo::getMyFiles() const
{
    return myFiles;
}

void UserInfo::setMyFiles(const vector<FileData> &value)
{
    myFiles = value;
}

int UserInfo::getMaxBet() const
{
    return maxBet;
}

void UserInfo::setMaxBet(int value)
{
    maxBet = value;
}

int UserInfo::getCash() const
{
    return cash;
}

void UserInfo::setCash(int value)
{
    cash = value;
}

void UserInfo::addAdditionalFiles(FileData fData)
{
    myFiles.push_back(fData);
}

QString UserInfo::getFilesDataID() const
{
    return filesDataID;
}

void UserInfo::setFilesDataID(const QString &value)
{
    filesDataID = value;
}

QString UserInfo::getCashDataID() const
{
    return cashDataID;
}

void UserInfo::setCashDataID(const QString &value)
{
    cashDataID = value;
}

void UserInfo::setBuddiesID(const QString &value)
{
    buddiesID = value;
}

QString UserInfo::getBuddiesID() const
{
    return buddiesID;
}
void UserInfo::updateProfileRequest(QNetworkReply *reply)
{
    if (reply->error() > 0) {
        qDebug() << reply->errorString();
    }
    else {
        qDebug() << reply->readAll();
    }
}

bool UserInfo::updateUserProfile(QObject *parent)
{
    QVariantMap testMapData;
    testMapData["email"] = email;
    testMapData["password"] = password;
    testMapData["profileName"] = profName;
    testMapData["hasCashData"] = hasCashData;
    testMapData["hasFilesData"] = hasFilesData;
    testMapData["hasBuddiesData"] = hasBuddiesData;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
    QNetworkAccessManager *manager = new QNetworkAccessManager(parent);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/users/" + userID));
    request.setRawHeader("user-token", userToken.toUtf8());
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->put(request, testJsonData.toJson());
    QEventLoop loop;
    parent->connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0) return true;
    else return false;
}

bool UserInfo::updateUserFiles(QObject *parent)
{
    QJsonObject jsonObj;
    for (int i = 0; i < myFiles.size(); i++){
        FileData data = myFiles.at(i);
        jsonObj[data.fileName] = data.fileURL;
    }
    QJsonDocument *jsonDoc = new QJsonDocument(jsonObj);

    QVariantMap testMapData;
    testMapData["objectId"] = filesDataID;
    testMapData["profileName"] = profName;
    testMapData["files"] = jsonDoc->toJson();

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
    QNetworkAccessManager *manager = new QNetworkAccessManager(parent);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Files/" + filesDataID));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->put(request, testJsonData.toJson());
    QEventLoop loop;
    parent->connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0) return true;
    else return false;
}

QString UserInfo::getProfName() const
{
    return profName;
}

void UserInfo::setProfName(const QString &value)
{
    profName = value;
}
UserInfo::UserInfo()
{

}

