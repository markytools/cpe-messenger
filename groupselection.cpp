#include "groupselection.h"
#include "ui_groupselection.h"

GroupSelection::GroupSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupSelection)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
    manager = new QNetworkAccessManager(this);
}

GroupSelection::~GroupSelection()
{
    delete ui;
}

void GroupSelection::on_newGroup_clicked()
{
    widgetsManager->showWidget(CREATE_GROUP);
}

void GroupSelection::on_backB_clicked()
{
    flagError("");
    widgetsManager->showWidget(MSGR_HOME);
}

void GroupSelection::showEvent(QShowEvent *e)
{
    refreshList();
}

void GroupSelection::hideEvent(QHideEvent *e)
{

}

void GroupSelection::flagError(QString message)
{
    ui->response->setText("<font color='red'>" + message + "</font>");
}

void GroupSelection::disableButtons(bool disable)
{
    ui->groupNotifs->setDisabled(disable);
    ui->deleteGroup->setDisabled(disable);
    ui->newGroup->setDisabled(disable);
    ui->joinGroup->setDisabled(disable);
    ui->backB->setDisabled(disable);
}

void GroupSelection::refreshList()
{
    ui->groupsList->clear();
    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Groups"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QString profileName = widgetsManager->getUserInfo()->getProfName();
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray groupsDataList = json.value("data").toArray();
        for (int i = 0; i < groupsDataList.size(); i++){
            QJsonObject groupsObj = groupsDataList.at(i).toObject();
            QString groupName = groupsObj.value("groupName").toString();
            QString members = groupsObj.value("member").toString();
            vector<string> memberList = arrayToVector(members.toStdString());
            for (string &str : memberList){
                if (str == profileName.toStdString()) ui->groupsList->addItem(groupName);
            }
        }
    }
    else {
        flagError(reply->errorString());
    }
}

void GroupSelection::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}

void GroupSelection::on_joinGroup_clicked()
{
    widgetsManager->showWidget(JOIN_GROUP);
}

void GroupSelection::on_deleteGroup_clicked()
{
    flagError("");
    disableButtons(true);
    if (ui->groupsList->currentItem() != NULL){
        QString selectedGroup = ui->groupsList->currentItem()->text();

        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/data/Groups"));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        QString profileName = widgetsManager->getUserInfo()->getProfName();
        QNetworkReply* reply = manager->get(request);
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if (reply->error() == 0){
            QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
            QJsonArray groupsDataList = json.value("data").toArray();
            for (int i = 0; i < groupsDataList.size(); i++){
                QJsonObject groupsObj = groupsDataList.at(i).toObject();
                QString groupName = groupsObj.value("groupName").toString();
                if (selectedGroup == groupName){
                    QString owner = groupsObj.value("owner").toString();
                    QString objectID = groupsObj.value("objectId").toString();
                    QString membersStr = groupsObj.value("member").toString();
                    QString passkey = groupsObj.value("passkey").toString();

                    vector<string> memberList = arrayToVector(membersStr.toStdString());
                    for (int i = 0; i < memberList.size(); i++){
                        string memberName = memberList.at(i);
                        if (memberName == profileName.toStdString()){
                            memberList.erase(memberList.begin() + i);
                            break;
                        }
                    }
                    QString realMemberList = QString::fromStdString(arrayToString(memberList));

                    QVariantMap testMapData;
                    testMapData["groupName"] = groupName;
                    testMapData["member"] = realMemberList;
                    testMapData["owner"] = owner;
                    testMapData["objectId"] = objectID;
                    testMapData["passkey"] = passkey;

                    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
                    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

                    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                                 + "/data/Groups/" + objectID));
                    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
                    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

                    QNetworkReply* reply = manager->put(request, testJsonData.toJson());
                    QEventLoop loop;
                    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
                    loop.exec();

                    if (reply->error() == 0){
                        refreshList();
                        disableButtons(false);
                        return;
                    }
                    else {
                        flagError(reply->errorString());
                    }
                }
            }
        }
        else {
            flagError(reply->errorString());
        }
    }
    disableButtons(false);
}

void GroupSelection::on_groupNotifs_clicked()
{
    flagError("");
    if (ui->groupsList->currentItem() != NULL){
        disableButtons(true);
        groupNotifications->setTitleName(ui->groupsList->currentItem()->text());
        groupNotifications->setChannelName(ui->groupsList->currentItem()->text());
        groupNotifications->setSubtopic("group.chat");
        groupNotifications->initSubscription();
        groupNotifications->setGNState(GROUP_CHAT);
        widgetsManager->showWidget(MESSAGING);
        disableButtons(false);
    }
}
void GroupSelection::setGroupNotifications(GroupNotifications *value)
{
    groupNotifications = value;
}

