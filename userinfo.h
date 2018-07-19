#ifndef USERINFO_H
#define USERINFO_H

#include <iostream>
#include <vector>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <QEventLoop>
#include "settings.h"

using namespace std;

struct FileData {
    QString fileName;
    QString fileURL;
};

class UserInfo
{
private:
    QString profName;
    QString email;
    QString password;
    QString userToken;
    QString userID;

    bool hasFilesData;
    bool hasCashData;
    bool hasBuddiesData;

    vector<FileData> myFiles;

    int maxBet;
    int cash;

    QString filesDataID;
    QString cashDataID;
    QString buddiesID;

private slots:
    void updateProfileRequest(QNetworkReply *reply);

public:
    UserInfo();
    UserInfo(QString profName, QString email, QString password, QString userToken, QString userID, bool hasFilesData, bool hasCashData,
             bool hasBuddiesData, int maxBet, int cash, vector<FileData> myFiles);

    QString getProfName() const;
    void setProfName(const QString &value);
    QString getEmail() const;
    void setEmail(const QString &value);
    void setPassword(const QString &value);
    QString getUserToken() const;
    void setUserToken(const QString &value);
    QString getUserID() const;
    void setUserID(const QString &value);
    bool getHasFilesData() const;
    void setHasFilesData(bool value);
    bool getHasCashData() const;
    void setHasCashData(bool value);
    vector<FileData> getMyFiles() const;
    void setMyFiles(const vector<FileData> &value);
    int getMaxBet() const;
    void setMaxBet(int value);
    int getCash() const;
    void setCash(int value);

    void addAdditionalFiles(FileData fData);
    bool updateUserProfile(QObject *parent);
    bool updateUserFiles(QObject *parent);

    QString getFilesDataID() const;
    void setFilesDataID(const QString &value);
    QString getCashDataID() const;
    void setCashDataID(const QString &value);
    void setBuddiesID(const QString &value);
    QString getBuddiesID() const;
};

#endif // USERINFO_H
