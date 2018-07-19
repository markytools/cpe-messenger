#ifndef GROUPSELECTION_H
#define GROUPSELECTION_H

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
#include <groupnotifications.h>

namespace Ui {
class GroupSelection;
}

class GroupSelection : public QWidget
{
    Q_OBJECT

public:
    explicit GroupSelection(QWidget *parent = 0);
    ~GroupSelection();

    void setWidgetsManager(QWidgetManager *value);
    void setGroupNotifications(GroupNotifications *value);

private slots:
    void on_newGroup_clicked();
    void on_backB_clicked();
    void on_joinGroup_clicked();
    void on_deleteGroup_clicked();
    void on_groupNotifs_clicked();

private:
    Ui::GroupSelection *ui;
    QWidgetManager *widgetsManager;
    QNetworkAccessManager *manager;
    GroupNotifications *groupNotifications;

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void flagError(QString message);
    void disableButtons(bool disable);
    void refreshList();
};

#endif // GROUPSELECTION_H
