#include "newgroup.h"
#include "ui_newgroup.h"

NewGroup::NewGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewGroup)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
}

NewGroup::~NewGroup()
{
    delete ui;
}
void NewGroup::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}


void NewGroup::on_create_clicked()
{
    flagError("", 0);
    if (ui->groupName->text().isEmpty() || ui->passkey->text().isEmpty()){
        flagError("Please fill all empty fields.", 0);
        return;
    }

    if (!isLettersAndNumbersOnly(ui->groupName->text().toStdString())){
        flagError("Invalid group name.", 0);
        return;
    }

    disableButtons(true);
    QString groupName = ui->groupName->text();
    QString passkey = ui->passkey->text();
    QString owner = widgetsManager->getUserInfo()->getProfName();
    QString userToken = widgetsManager->getUserInfo()->getUserToken();

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Groups?where=groupName%20%3D%20%27" + groupName + "%27"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        int totalObjects = json.value("totalObjects").toInt();
        if (totalObjects == 0){
            vector<string> members;
            members.push_back(owner.toStdString());
            QString membersStr = QString::fromStdString(arrayToString(members));

            QVariantMap testMapData;
            testMapData["groupName"] = groupName;
            testMapData["owner"] = owner;
            testMapData["member"] = membersStr;
            testMapData["passkey"] = passkey;

            QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
            QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                         + "/data/Groups"));
            request.setRawHeader("user-token", userToken.toUtf8());
            request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

            reply = manager->post(request, testJsonData.toJson());

            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            if (reply->error() == 0){
                resetUI();
                flagError("Group created: " + groupName + ", Passkey: " + passkey, 1);
            }
            else {
                disableButtons(false);
                flagError(reply->errorString(), 0);
                return;
            }
        }
        else {
            disableButtons(false);
            flagError("Group name already exists.", 0);
            return;
        }
    }
    else {
        disableButtons(false);
        flagError(reply->errorString(), 0);
        return;
    }
    disableButtons(false);
}

void NewGroup::disableButtons(bool disable)
{
    ui->backB->setDisabled(disable);
    ui->create->setDisabled(disable);
}

void NewGroup::flagError(QString message, int state)
{
    if (state == 0) ui->response->setText("<font color='red'>" + message + "</font>");
    else ui->response->setText("<font color='green'>" + message + "</font>");
}

void NewGroup::resetUI()
{
    ui->groupName->clear();
    ui->passkey->clear();
}

void NewGroup::on_backB_clicked()
{
    disableButtons(true);
    resetUI();
    widgetsManager->showWidget(GROUP_SELECTION);
    disableButtons(false);
}
