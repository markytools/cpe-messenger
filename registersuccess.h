#ifndef REGISTERSUCCESS_H
#define REGISTERSUCCESS_H

#include <QWidget>
#include <qwidgetmanager.h>

namespace Ui {
class RegisterSuccess;
}

class RegisterSuccess : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterSuccess(QWidget *parent = 0);
    ~RegisterSuccess();

    void setWidgetsManager(QWidgetManager *widgetsManager);
private slots:
    void on_back_clicked();

private:
    Ui::RegisterSuccess *ui;
    QWidgetManager *widgetsManager;
};

#endif // REGISTERSUCCESS_H
