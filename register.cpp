#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
}

Register::~Register()
{
    delete ui;
}

void Register::on_registerUser_clicked()
{
    QString errorNum;
    ui->response->setText("<font color='green'>Loading...</font>");
    if (ui->email->text().isEmpty() || ui->password->text().isEmpty() ||
            ui->passwordConfirm->text().isEmpty() || ui->profileName->text().isEmpty() || ui->answer->text().isEmpty()){
        flagErrors("Please fill all empty field.");
        return;
    }
    if (!(ifStringHasSubstring(ui->email->text().toStdString(), "@") && ifStringHasSubstring(ui->email->text().toStdString(), "."))){
        flagErrors("Invalid email format.");
        return;
    }
    if (ui->password->text() != ui->passwordConfirm->text()){
        flagErrors("Both passwords are not the same");
        return;
    }
    if (!isLettersAndNumbersOnly(ui->profileName->text().toStdString())){
        flagErrors("You can only use letters and\nnumbers for your profile name.");
        return;
    }
    disableButtons(true);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/ProfileNames/" + Settings::PROFNAMES_OBJ_ID));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->get(request);

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        currentProfNames = json.value("profNames").toString();
        if (!hasStringInArray(currentProfNames.toStdString(), ui->profileName->text().toStdString())){
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
            QVariantMap testMapData;
            testMapData["email"] = ui->email->text();
            testMapData["password"] = ui->password->text();
            testMapData["profileName"] = ui->profileName->text();
            testMapData["question1"] = ui->identityQuestionBox->currentIndex();
            testMapData["answer1"] = ui->answer->text();
            testMapData["hasFilesData"] = false;
            testMapData["hasCashData"] = false;
            testMapData["hasBuddiesData"] = false;

            QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/users/register"));
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            QNetworkReply* reply = manager->post(request, testJsonData.toJson());

            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                currentProfNames.remove(currentProfNames.size() - 1, 1);
                currentProfNames += ui->profileName->text() + ",]";

                QVariantMap testMapData;
                testMapData["objectId"] = Settings::PROFNAMES_OBJ_ID;
                testMapData["profNames"] = currentProfNames;

                QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
                QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                             + "/data/ProfileNames/" + Settings::PROFNAMES_OBJ_ID));
                request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
                request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

                reply = manager->put(request, testJsonData.toJson());

                QEventLoop loop;
                QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
                loop.exec();

                if (reply->error() == 0){
                    resetUI();
                    widgetsManager->showWidget(REG_SUCCESS);
                    reply->deleteLater();
                }
                else {
                    flagErrors(reply->errorString());
                }
            }
            else {
                bool hasConflict = ifStringHasSubstring(reply->errorString().toStdString(), "Conflict");
                if (hasConflict) flagErrors("Email has already been registered.");
                else flagErrors(reply->errorString());
            }
        }
        else {
            flagErrors("Profile name already exist.");
        }
    }
    else {
        flagErrors(reply->errorString());
    }
    disableButtons(false);
}

void Register::on_backB_clicked()
{
    resetUI();
    widgetsManager->showWidget(HOME);
}

void Register::flagErrors(QString message)
{
    ui->response->setText("<font color='red'>" + message + "</font>");
}

void Register::resetUI()
{
    ui->email->clear();
    ui->password->clear();
    ui->passwordConfirm->clear();
    ui->profileName->clear();
    ui->answer->clear();
    ui->identityQuestionBox->setCurrentIndex(0);
    ui->response->clear();
}

void Register::disableButtons(bool disable)
{
    ui->backB->setDisabled(disable);
    ui->registerUser->setDisabled(disable);
}
void Register::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}

