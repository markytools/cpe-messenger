#ifndef QWIDGETMANAGER_H
#define QWIDGETMANAGER_H

#include <QWidget>
#include <vector>
#include <iterator>
#include <userinfo.h>
using namespace std;

enum Form {
    HOME, REG, REG_SUCCESS, LOGIN, MSGR_HOME, MY_FILES, UPLOAD_FILE, GROUP_SELECTION, JOIN_GROUP, CREATE_GROUP, MESSAGING,
    PASS_RECOVERY, BUDDIES, JACKPOTS, JACKPOTS_HOME, JACKPOTS_ROOM
};

class QWidgetManager
{
public:
    QWidgetManager();
    void showWidget(Form form);
    void addWidget(QWidget *widget, Form form);

    void setUserInfo(UserInfo *value);
    UserInfo *getUserInfo() const;

private:
    UserInfo *userInfo;
    vector<QWidget*> *widgets;
    map<QWidget*, Form> widgetMap;
};

#endif // QWIDGETMANAGER_H
