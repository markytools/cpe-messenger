#include "joingroup.h"
#include "ui_joingroup.h"

JoinGroup::JoinGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinGroup)
{
    ui->setupUi(this);
    ui->response->setWordWrap(true);
}

JoinGroup::~JoinGroup()
{
    delete ui;
}
void JoinGroup::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}


void JoinGroup::on_join_clicked()
{
    disableButtons(true);
    flagErrors("", 0);
    if (ui->listOfGroups->currentItem() != NULL){
        QString selectedGroup = ui->listOfGroups->currentItem()->text();

        if (!ifStringHasSubstring(selectedGroup.toStdString(), "(joined)")){
            GroupData groupData = groupKeyMap[selectedGroup];
            if (ui->passkey->text() == groupData.passkey){
                groupData.memberList.push_back(widgetsManager->getUserInfo()->getProfName().toStdString());
                QString memberList = QString::fromStdString(arrayToString(groupData.memberList));

                QVariantMap testMapData;
                testMapData["groupName"] = selectedGroup;
                testMapData["member"] = memberList;
                testMapData["owner"] = groupData.owner;
                testMapData["passkey"] = groupData.passkey;
                testMapData["objectId"] = groupData.objectID;

                QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
                QNetworkAccessManager *manager = new QNetworkAccessManager(this);

                QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                             + "/data/Groups/" + groupData.objectID));
                request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
                request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

                QNetworkReply* reply = manager->put(request, testJsonData.toJson());
                QEventLoop loop;
                connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
                loop.exec();

                if (reply->error() == 0){
                    resetUI();
                    flagErrors("Group Joined: " + selectedGroup, 1);
                }
                else {
                    flagErrors(reply->errorString(), 0);
                }
                reply->deleteLater();
            }
            else {
                flagErrors("Invalid Passkey!", 0);
            }
        }
        else {
            flagErrors("Already a member of the group.", 0);
        }
    }
    disableButtons(false);
}

void JoinGroup::disableButtons(bool disable)
{
    ui->join->setDisabled(disable);
    ui->search->setDisabled(disable);
    ui->backB->setDisabled(disable);
}

void JoinGroup::flagErrors(QString message, int state)
{
    if (state == 0) ui->response->setText("<font color='red'>" + message + "</font>");
    else ui->response->setText("<font color='green'>" + message + "</font>");
}

void JoinGroup::resetUI()
{
    ui->passkey->clear();
    ui->listOfGroups->clear();
    ui->groupName->clear();
}

void JoinGroup::on_search_clicked()
{
    flagErrors("", 0);
    groupKeyMap.clear();
    ui->listOfGroups->clear();
    ui->passkey->clear();
    disableButtons(true);
    QString searchedGroup = ui->groupName->text();
    QString profileName = widgetsManager->getUserInfo()->getProfName();

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/data/Groups?where=groupName%20LIKE%20%27" + searchedGroup + "%25%27"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(request);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray groupsDataList = json.value("data").toArray();
        if (groupsDataList.size() == 0) flagErrors("No groups found", 0);
        for (int i = 0; i < groupsDataList.size(); i++){
            QJsonObject groupsObj = groupsDataList.at(i).toObject();
            QString groupName = groupsObj.value("groupName").toString();
            QString passkey = groupsObj.value("passkey").toString();
            QString objectId = groupsObj.value("objectId").toString();
            QString members = groupsObj.value("member").toString();
            QString owner = groupsObj.value("owner").toString();
            vector<string> memberList = arrayToVector(members.toStdString());
            bool isMember = false;
            for (string &str : memberList){
                if (str == profileName.toStdString()) isMember = true;
            }
            if (isMember) ui->listOfGroups->addItem(groupName + " (joined)");
            else ui->listOfGroups->addItem(groupName);

            GroupData groupData;
            groupData.objectID = objectId;
            groupData.passkey = passkey;
            groupData.owner = owner;
            groupData.memberList = memberList;
            groupKeyMap[groupName] = groupData;
        }
    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
    disableButtons(false);
}

void JoinGroup::on_backB_clicked()
{
    resetUI();
    widgetsManager->showWidget(GROUP_SELECTION);
}
