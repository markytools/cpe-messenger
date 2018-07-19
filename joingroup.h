#ifndef JOINGROUP_H
#define JOINGROUP_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <map>
#include "settings.h"

struct GroupData {
    QString owner;
    QString objectID;
    QString passkey;
    vector<string> memberList;
};

namespace Ui {
class JoinGroup;
}

class JoinGroup : public QWidget
{
    Q_OBJECT

public:
    explicit JoinGroup(QWidget *parent = 0);
    ~JoinGroup();

    void setWidgetsManager(QWidgetManager *value);

private slots:
    void on_join_clicked();
    void on_search_clicked();

    void on_backB_clicked();

private:
    Ui::JoinGroup *ui;
    QWidgetManager *widgetsManager;
    map<QString, GroupData> groupKeyMap;

    void disableButtons(bool disable);
    void flagErrors(QString message, int state);
    void resetUI();
};

#endif // JOINGROUP_H
