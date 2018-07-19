#ifndef NEWGROUP_H
#define NEWGROUP_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include "settings.h"

namespace Ui {
class NewGroup;
}

class NewGroup : public QWidget
{
    Q_OBJECT

public:
    explicit NewGroup(QWidget *parent = 0);
    ~NewGroup();

    void setWidgetsManager(QWidgetManager *value);

private slots:
    void on_create_clicked();

    void on_backB_clicked();

private:
    Ui::NewGroup *ui;
    QWidgetManager *widgetsManager;

    void disableButtons(bool disable);
    void flagError(QString message, int state);
    void resetUI();
};

#endif // NEWGROUP_H
