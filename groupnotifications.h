#ifndef GROUPNOTIFICATIONS_H
#define GROUPNOTIFICATIONS_H

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
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include "settings.h"

enum GroupNotificationsState {
    GROUP_CHAT, BUDDY_CHAT
};

namespace Ui {
class GroupNotifications;
}

class GroupNotifications : public QWidget
{
    Q_OBJECT

public:
    explicit GroupNotifications(QWidget *parent = 0);
    ~GroupNotifications();

    void setWidgetsManager(QWidgetManager *value);

    void setSubtopic(const QString &value);
    void setChannelName(const QString &value);
    void setTitleName(QString title);
    void initSubscription();
    void setGNState(const GroupNotificationsState &value);

private slots:
    void updateNotifications();
    void receiveMSG(QNetworkReply *reply);
    void progressChanged(qint64 a, qint64 b);
    void readingReadyBytesDesktop(QNetworkReply *reply);
    void readingReadyBytes(QNetworkReply *reply);

private:
    Ui::GroupNotifications *ui;

    QWidgetManager *widgetsManager;
    QNetworkAccessManager *manager;
    QNetworkAccessManager *receivedMSGManager;
    QTimer* mTimer;
    map<QString, FileData> filesMap;
    QString currentFileName;
    GroupNotificationsState gNState;

    QString channelName;
    QString subscriptionID;
    QString subtopic;
    string downloadedFileExtension;
    QString downloadPath;

    void resetUI();
    void flagErrors(QString message, int state);
    void disableButtons(bool disable);
    void putMessage(QString publisherID, QString message);  //Automatically filters downloads.
    void sendMessage(QString message);
    bool ifFileExist(QString fileName);
    QString getDownloadDir();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
private slots:
    void on_shareB_clicked();
    void on_sendB_clicked();
    void on_downloadB_clicked();
    void on_backB_clicked();
};

#endif // GROUPNOTIFICATIONS_H
