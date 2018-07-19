#include "registersuccess.h"
#include "ui_registersuccess.h"

RegisterSuccess::RegisterSuccess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterSuccess)
{
    ui->setupUi(this);
}

RegisterSuccess::~RegisterSuccess()
{
    delete ui;
}

void RegisterSuccess::setWidgetsManager(QWidgetManager *widgetsManager)
{
    this->widgetsManager = widgetsManager;
}

void RegisterSuccess::on_back_clicked()
{
    widgetsManager->showWidget(HOME);
}
