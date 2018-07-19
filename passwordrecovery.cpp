#include "passwordrecovery.h"
#include "ui_passwordrecovery.h"

PasswordRecovery::PasswordRecovery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PasswordRecovery)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
    ui->question->setWordWrap(true);
}

PasswordRecovery::~PasswordRecovery()
{
    delete ui;
}

void PasswordRecovery::on_backB_clicked()
{
    widgetsManager->showWidget(LOGIN);
    resetUI();
}

void PasswordRecovery::resetUI()
{
    ui->response->clear();
    ui->answer->setDisabled(false);
    ui->confirmB->setDisabled(false);
    ui->answer->clear();
}

void PasswordRecovery::disableButtons(bool disable)
{
    ui->backB->setDisabled(disable);
    ui->confirmB->setDisabled(disable);
}

void PasswordRecovery::flagErrors(QString message, int state)
{
    if (state == 0) ui->response->setText("<font color='red'>" + message + "</font>");
    else ui->response->setText("<font color='green'>" + message + "</font>");
}

void PasswordRecovery::setAnswer(const QString &value)
{
    answer = value;
}

void PasswordRecovery::setIdentityQuestion(const QString &value)
{
    ui->question->setText(value);
}

void PasswordRecovery::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}


void PasswordRecovery::on_confirmB_clicked()
{
    flagErrors("", 0);
    if (ui->answer->text().toLower() != answer.toLower()){
        flagErrors("Wrong answer!", 0);
        return;
    }

    disableButtons(true);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString emailURL = QString::fromStdString(url_encode(email.toStdString()));
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/users/restorepassword/" + emailURL));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QNetworkReply *reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        flagErrors("An email to change your password has been sent.", 1);
        ui->answer->setDisabled(true);
        ui->confirmB->setDisabled(true);
        ui->backB->setDisabled(false);
    }
    else {
        disableButtons(false);
        flagErrors(reply->errorString(), 1);
    }
    reply->deleteLater();
}
void PasswordRecovery::setEmail(const QString &value)
{
    email = value;
}

