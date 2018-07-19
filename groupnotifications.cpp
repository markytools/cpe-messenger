#include "groupnotifications.h"
#include "ui_groupnotifications.h"

#ifdef Q_OS_WIN

#else
#include <QtAndroidExtras/QAndroidJniObject>
#endif

GroupNotifications::GroupNotifications(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupNotifications)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    receivedMSGManager = new QNetworkAccessManager(this);
    ui->response->setWordWrap(true);
    ui->messagesList->setWordWrap(true);
    ui->messagesList->setTextElideMode(Qt::ElideNone);
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(updateNotifications()));
}

GroupNotifications::~GroupNotifications()
{
    delete ui;
}
void GroupNotifications::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}

void GroupNotifications::updateNotifications()
{
    if (subscriptionID != NULL){
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/messaging/" + channelName + "/" + subscriptionID));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);

        receivedMSGManager->get(request);
        connect(receivedMSGManager, SIGNAL(finished(QNetworkReply*)), SLOT(receiveMSG(QNetworkReply*)));

        //        if (reply->error() == 0){
        //            QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        //            QJsonArray messagesData = json.value("messages").toArray();
        //            for (int i = 0; i < messagesData.size(); i++){
        //                QJsonObject messagesDataObj = messagesData.at(i).toObject();
        //                QString publisherID = messagesDataObj.value("publisherId").toString();
        //                QString message = messagesDataObj.value("data").toString();
        //                putMessage(publisherID, message);
        //            }
        //        }
        //        else {
        //            flagErrors(reply->errorString(), 0);
        //        }
    }
}

void GroupNotifications::receiveMSG(QNetworkReply *reply)
{
    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray messagesData = json.value("messages").toArray();
        for (int i = 0; i < messagesData.size(); i++){
            QJsonObject messagesDataObj = messagesData.at(i).toObject();
            QString publisherID = messagesDataObj.value("publisherId").toString();
            QString message = messagesDataObj.value("data").toString();
            putMessage(publisherID, message);
        }
    }
    else {
        flagErrors(reply->errorString(), 0);
    }
    reply->deleteLater();
}

void GroupNotifications::resetUI()
{
    flagErrors("", 0);
    ui->messagesList->clear();
    ui->messageToSend->clear();
}
void GroupNotifications::setChannelName(const QString &value)
{
    channelName = value;
}

void GroupNotifications::setTitleName(QString title)
{
    ui->groupNameLabel->setText(title);
}

void GroupNotifications::initSubscription()
{
    QVariantMap testMapData;
    testMapData["subtopic"] = subtopic;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY + "/messaging/" + channelName + "/subscribe"));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->post(request, testJsonData.toJson());
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        subscriptionID = json.value("subscriptionId").toString();

        mTimer->start(2000);
    }
    else {
        flagErrors("Connection Error", 0);
    }
    reply->deleteLater();
}

void GroupNotifications::setSubtopic(const QString &value)
{
    subtopic = value;
}


void GroupNotifications::flagErrors(QString message, int state)
{
    switch (state){
    case 0: ui->response->setText("<font color='red'>" + message + "</font>"); break;
    case 1: ui->response->setText("<font color='blue'>" + message + "</font>"); break;
    case 2: ui->response->setText("<font color='green'>" + message + "</font>"); break;
    default: break;
    }
}

void GroupNotifications::disableButtons(bool disable)
{
    ui->backB->setDisabled(disable);
    ui->downloadB->setDisabled(disable);
    ui->shareB->setDisabled(disable);
    ui->sendB->setDisabled(disable);
}

//Automatically puts a message at the topmost list of the widget.
void GroupNotifications::putMessage(QString publisherID, QString message)
{
    string msgStr = message.toStdString();
    bool isDownloadLink = false;
    if (msgStr.size() > 10){
        if (msgStr.substr(0, 4) == "qazx" && msgStr.substr(msgStr.size() - 4, 4) == "plkm"){
            isDownloadLink = true;
        }
    }

    if (isDownloadLink){
        msgStr.erase(0, 4);
        msgStr.erase(msgStr.size() - 4, 4);
        QJsonDocument json = QJsonDocument::fromJson(QString::fromStdString(msgStr).toUtf8());
        QJsonObject messageJSONObj = json.object();
        QString fileName, fileURL;
        foreach (QString key, messageJSONObj.keys()){
            fileName = key;
            fileURL = messageJSONObj.value(key).toString();
        }
        FileData fData;
        fData.fileName = fileName;
        fData.fileURL = fileURL;
        filesMap[publisherID + "(FILE): " + fileName] = fData;   //TODO
        ui->messagesList->insertItem(0, publisherID + "(FILE): " + fileName);
        ui->messagesList->item(0)->setForeground(Qt::blue);
    }
    else {
        ui->messagesList->insertItem(0, publisherID + ": " + message);
        ui->messagesList->item(0)->setFlags(Qt::ItemIsEnabled);
        ui->messagesList->item(0)->setBackground(Qt::transparent);
    }
}

void GroupNotifications::sendMessage(QString message)
{
    disableButtons(true);
    QVariantMap testMapData;
    testMapData["message"] = message;
    testMapData["publisherId"] = widgetsManager->getUserInfo()->getProfName();
    testMapData["subtopic"] = subtopic;

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);

    QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                 + "/messaging/" + channelName));
    request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

    QNetworkReply* reply = manager->post(request, testJsonData.toJson());
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == 0){

    }
    else {
        flagErrors("Error sending message", 0);
    }
    disableButtons(false);
    reply->deleteLater();
}

bool GroupNotifications::ifFileExist(QString fileName)
{
    QFileInfo checkFile(fileName);
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}

QString GroupNotifications::getDownloadDir()
{
    QDir mypath(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    downloadPath = mypath.path();
    return downloadPath;


//    QDir mypath(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

//    if (!mypath.cd("CPEMSGR")){
//        if (mypath.mkpath("CPEMSGR"))
//            qDebug() << "path created";
//        else
//            qDebug() << "path not created";
//    }
//    else {
//        qDebug() << "inside dir";
//    }
//    downloadPath = mypath.path() + "/CPEMSGR";
//    return downloadPath;
}

void GroupNotifications::showEvent(QShowEvent *)
{
    ////////SUBSCRIBE
    //    filesMap.clear();
    //    for (FileData &fData : widgetsManager->getUserInfo()->getMyFiles()){
    //        filesMap[fData.fileName] = fData.fileURL;
    //    }
    flagErrors("", 0);
    mTimer->start();
}

void GroupNotifications::hideEvent(QHideEvent *)
{
    mTimer->stop();
    ui->messagesList->clear();
    filesMap.clear();
}


void GroupNotifications::on_shareB_clicked()
{
    flagErrors("", 0);
    QStringList items;
    for (FileData &fData : widgetsManager->getUserInfo()->getMyFiles()){
        items << fData.fileName;
    }
    if (items.size() != 0){
        bool ok;
        QInputDialog fileDialog;
        fileDialog.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        QString item = fileDialog.getItem(this, tr("QInputDialog::getItem()"),
                                             tr("Select File:"), items, 0, false, &ok);
        if (ok && !item.isEmpty()){
            QString fileURL;
            for (FileData &fData : widgetsManager->getUserInfo()->getMyFiles()){
                if (fData.fileName == item) fileURL = fData.fileURL;
            }
            QJsonObject itemObj;
            itemObj[item] = fileURL;
            QJsonDocument doc(itemObj);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            sendMessage("qazx" + strJson + "plkm");
        }
    }
    else {
        flagErrors("No files to share.", 0);
    }
}

void GroupNotifications::on_sendB_clicked()
{
    flagErrors("", 0);
    if (!ui->messageToSend->text().isEmpty()) sendMessage(ui->messageToSend->text());
    ui->messageToSend->clear();
}

void GroupNotifications::on_downloadB_clicked()
{
    flagErrors("", 0);
    if (ui->messagesList->currentItem() != NULL){
        disableButtons(true);

        QString fileURL;
#ifdef Q_OS_WIN
        QString file = ui->messagesList->currentItem()->text();
        FileData fData = filesMap[file];
        currentFileName = fData.fileName;
        fileURL = fData.fileURL;
        string path = fileURL.toStdString();
        downloadedFileExtension = "." + path.substr(path.find_last_of(".") + 1);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                SLOT(readingReadyBytesDesktop(QNetworkReply*)));
        QUrl url(fileURL);
        QNetworkRequest request(url);
        QNetworkReply *currentDownloadRequest = manager->get(request);
        connect(currentDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)),
                SLOT(progressChanged(qint64,qint64)));
        flagErrors("Downloading...", 1);
#else
        QString file = ui->messagesList->currentItem()->text();
        FileData fData = filesMap[file];
        currentFileName = fData.fileName;
        fileURL = fData.fileURL;

        string path = fileURL.toStdString();
        downloadedFileExtension = "." + path.substr(path.find_last_of(".") + 1);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                SLOT(readingReadyBytes(QNetworkReply*)));
        QUrl url(fileURL);
        QNetworkRequest request(url);
        QNetworkReply *currentDownloadRequest = manager->get(request);
        connect(currentDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)),
                SLOT(progressChanged(qint64,qint64)));
        flagErrors("Downloading...", 1);

        QAndroidJniObject javaNotification = QAndroidJniObject::fromString("Downloading the file...");
        QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                                  "toast",
                                                  "(Ljava/lang/String;)V",
                                                  javaNotification.object<jstring>());
#endif
    }
}

void GroupNotifications::progressChanged(qint64 a, qint64 b)
{
    if (b > 0){
        string fAbbrevA;
        float downloadedA;
        if (a >= 1024 && a < 1048576){
            fAbbrevA = " KB";
            downloadedA = (float)a / 1024;
        }
        else if (a >= 1048576 && a < 1073741824){
            fAbbrevA = " MB";
            downloadedA = (float)a / 1048576;
        }
        else if (a >= 1073741824){
            fAbbrevA = " GB";
            downloadedA = (float)a / 1073741824;
        }
        else {
            fAbbrevA = " B";
            downloadedA = (float)a;
        }

        string fAbbrevB;
        float downloadedB;
        if (b >= 1024 && b < 1048576){
            fAbbrevB = " KB";
            downloadedB = (float)b / 1024;
        }
        else if (b >= 1048576 && b < 1073741824){
            fAbbrevB = " MB";
            downloadedB = (float)b / 1048576;
        }
        else if (b >= 1073741824){
            fAbbrevB = " GB";
            downloadedB = (float)b / 1073741824;
        }
        else {
            fAbbrevB = " B";
            downloadedB = (float)b;
        }
        if (a != b) flagErrors(QString::fromStdString(numberToString(downloadedA, 2) + fAbbrevA + " / " +
                                                      numberToString(downloadedB, 2) + fAbbrevB), 1);
    }
}

void GroupNotifications::readingReadyBytesDesktop(QNetworkReply *reply)
{
    if (reply->error() == 0){
        string args = string("File Type (*") + downloadedFileExtension + string(") ;; All Files (*)");
        QString filePath;
        QFileDialog dlg(this, tr("Save As"), currentFileName,
                        tr(args.c_str()));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
        if (dlg.exec()){
            filePath = dlg.selectedFiles().at(0);
            QFile *downloadedFile = new QFile(filePath);
            downloadedFile->open(QIODevice::WriteOnly);
            downloadedFile->write(reply->readAll());
            downloadedFile->close();
            flagErrors("Download Complete.", 2);
        }
        else{
            flagErrors("Download Canceled.", 0);
        }
    }
    else {
        flagErrors("Download Failed.", 0);
    }
    reply->deleteLater();
    disableButtons(false);
}

void GroupNotifications::readingReadyBytes(QNetworkReply *reply)

{
    if (reply->error() == 0){
        int count = 1;
        QString filePath = getDownloadDir() + '/' + currentFileName;
        while (ifFileExist(filePath)){
            count++;
            string path = filePath.toStdString();
            removeSubstring(path, downloadedFileExtension);
            filePath = QString::fromStdString(path + "(" + numberToString(count, 0) + ")" + downloadedFileExtension);
        }

        QFile *downloadedFile = new QFile(filePath);
        downloadedFile->open(QIODevice::WriteOnly);
        downloadedFile->write(reply->readAll());
        downloadedFile->close();
        flagErrors(currentFileName, 2);
    }
    else {
        flagErrors("Download Failed.", 0);
    }
    reply->deleteLater();
    disableButtons(false);
}
void GroupNotifications::setGNState(const GroupNotificationsState &value)
{
    gNState = value;
}


void GroupNotifications::on_backB_clicked()
{
    resetUI();
    switch (gNState){
    case GROUP_CHAT: widgetsManager->showWidget(GROUP_SELECTION); break;
    case BUDDY_CHAT: widgetsManager->showWidget(BUDDIES); break;
    default: break;
    }
}
