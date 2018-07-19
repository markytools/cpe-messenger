#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <vector>
#include <qwidgetmanager.h>
#include <QVariantMap>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include "settings.h"

using namespace std;

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();

    void setWidgetsManager(QWidgetManager *value);

private slots:
    void on_registerUser_clicked();
    void on_backB_clicked();

private:
    Ui::Register *ui;
    vector<QWidget*> *widgets;
    QWidgetManager *widgetsManager;
    QString currentProfNames;

    void flagErrors(QString message);
    void resetUI();
    void disableButtons(bool disable);
};

#endif // REGISTER_H
