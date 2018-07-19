#include "myfiles.h"
#include "ui_myfiles.h"

#ifdef Q_OS_WIN

#else
#include <QtAndroidExtras/QAndroidJniObject>
#endif

MyFiles::MyFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFiles)
{
    ui->setupUi(this);
    ui->fileNameToDownload->setWordWrap(true);
    ui->downloadProgress->setWordWrap(true);
}

MyFiles::~MyFiles()
{
    delete ui;
}

void MyFiles::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}

void MyFiles::on_back_clicked()
{
    widgetsManager->showWidget(MSGR_HOME);
}

void MyFiles::on_upload_clicked()
{
    if (widgetsManager->getUserInfo()->getMyFiles().size() <= 3){
        widgetsManager->showWidget(UPLOAD_FILE);
    }
    else {
        writeFileNameToDownload("You can only upload up to 3 files.", 0);
        writeFileDownloadProgress("", 0);
    }
}

void MyFiles::showEvent(QShowEvent *)
{
    refreshList();
}

void MyFiles::on_download_clicked()
{
    if (ui->listOfFiles->count() != 0 && ui->listOfFiles->currentItem() != NULL){
#ifdef Q_OS_WIN
        currentFileName = ui->listOfFiles->currentItem()->text();
        QString fileURL = returnFileURL(currentFileName);

        string path = fileURL.toStdString();
        downloadedFileExtension = "." + path.substr(path.find_last_of(".") + 1);

        disableButtons();
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                SLOT(readingReadyBytesDesktop(QNetworkReply*)));
        QUrl url(fileURL);
        QNetworkRequest request(url);
        QNetworkReply *currentDownloadRequest = manager->get(request);
        connect(currentDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)),
                SLOT(progressChanged(qint64,qint64)));
        writeFileNameToDownload("Downloading...", 1);
#else
        currentFileName = ui->listOfFiles->currentItem()->text();
        QString fileURL = returnFileURL(currentFileName);

        string path = fileURL.toStdString();
        downloadedFileExtension = "." + path.substr(path.find_last_of(".") + 1);

        disableButtons();
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                SLOT(readingReadyBytes(QNetworkReply*)));
        QUrl url(fileURL);
        QNetworkRequest request(url);
        QNetworkReply *currentDownloadRequest = manager->get(request);
        connect(currentDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)),
                SLOT(progressChanged(qint64,qint64)));
        writeFileNameToDownload("Downloading...", 1);
        writeFileDownloadProgress("", 1);

        QAndroidJniObject javaNotification = QAndroidJniObject::fromString("Downloading the file...");
        QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                           "toast",
                                           "(Ljava/lang/String;)V",
                                           javaNotification.object<jstring>());
#endif
    }
}

void MyFiles::progressChanged(qint64 a, qint64 b)
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
        if (a != b) writeFileNameToDownload(QString::fromStdString(numberToString(downloadedA, 2) + fAbbrevA + " / " +
                                                                   numberToString(downloadedB, 2) + fAbbrevB), 1);
    }
}

void MyFiles::readingReadyBytes(QNetworkReply *reply)
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
        writeFileNameToDownload("Download Complete.", 2);
        writeFileDownloadProgress(currentFileName, 2);
    }
    else {
        writeFileNameToDownload("Download Failed.", 0);
    }
    enableButtons();

    //    if (reply->error() == 0){
    //        int count = 1;
    //        QFile *file = new QFile();
    //        file->write(reply->read(reply->bytesAvailable()));
    //        QString fileName = file->fileName();

    //        QString filePath = getDownloadDir() + '/' + fileName;
    //        while (ifFileExist(filePath)){
    //            count++;
    //            string path = filePath.toStdString();
    //            string strToTrim = "." + path.substr(path.find_last_of(".") + 1);
    //            removeSubstring(path, strToTrim);
    //            filePath = QString::fromStdString(path + "(" + numberToString(count, 0) + ")" + strToTrim);
    //        }
    //        QFile savedFile(filePath);
    //        if (savedFile.open(QIODevice::WriteOnly)) {
    //            // Write the file content.
    //        }
    //        writeFileNameToDownload("Download Complete.", 2);
    //    }
    //    else {
    //        writeFileNameToDownload("Download Failed.", 0);
    //    }
    //    currentDownloadRequest->deleteLater();
}

void MyFiles::readingReadyBytesDesktop(QNetworkReply *reply)
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
            writeFileNameToDownload("Download Complete.", 2);
        }
        else{
            writeFileNameToDownload("Download Canceled.", 0);
        }
    }
    else {
        writeFileNameToDownload("Download Failed.", 0);
    }
    enableButtons();
}
QListWidget *MyFiles::getListOfFilesWidget() const
{
    return ui->listOfFiles;
}

void MyFiles::refreshList()
{
    ui->listOfFiles->clear();
    for (FileData &fData : widgetsManager->getUserInfo()->getMyFiles()){
        ui->listOfFiles->addItem(fData.fileName);
    }
}


QString MyFiles::returnFileURL(QString fileName)
{
    for (FileData &fData : widgetsManager->getUserInfo()->getMyFiles()){
        if (fData.fileName == fileName) return fData.fileURL;
    }
}

QString MyFiles::getDownloadDir()
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

void MyFiles::writeFileNameToDownload(QString fileName, int state)
{
    ui->fileNameToDownload->clear();
    switch (state){
    case 0: ui->fileNameToDownload->setText("<font color='red'>" + fileName + "</font>"); break;
    case 1: ui->fileNameToDownload->setText("<font color='blue'>" + fileName + "</font>"); break;
    case 2: ui->fileNameToDownload->setText("<font color='green'>" + fileName + "</font>"); break;
    default: break;
    }
}

void MyFiles::writeFileDownloadProgress(QString progress, int state)
{
    ui->downloadProgress->clear();
    switch (state){
    case 0: ui->downloadProgress->setText("<font color='red'>" + progress + "</font>"); break;
    case 1: ui->downloadProgress->setText("<font color='blue'>" + progress + "</font>"); break;
    case 2: ui->downloadProgress->setText("<font color='green'>" + progress + "</font>"); break;
    default: break;
    }
}

bool MyFiles::ifFileExist(QString fileName)
{
    QFileInfo checkFile(fileName);
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}

void MyFiles::disableButtons()
{
    ui->upload->setDisabled(true);
    ui->download->setDisabled(true);
    ui->deleteB->setDisabled(true);
    ui->back->setDisabled(true);
}

void MyFiles::enableButtons()
{
    ui->upload->setDisabled(false);
    ui->download->setDisabled(false);
    ui->deleteB->setDisabled(false);
    ui->back->setDisabled(false);
}

void MyFiles::on_deleteB_clicked()
{
    if (ui->listOfFiles->count() != 0){
        disableButtons();
        writeFileNameToDownload("Deleting", 1);
        writeFileDownloadProgress("", 0);

        QString fileName = ui->listOfFiles->currentItem()->text();
        QString filePath = widgetsManager->getUserInfo()->getProfName();
        //    QString userToken = widgetsManager->getUserInfo()->getUserToken();

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        QNetworkRequest request(QUrl("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                                     + "/files/userFiles/" + filePath + "Files/" + fileName));
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
        QNetworkReply *reply = manager->deleteResource(request);
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if (reply->error() == 0){
            vector<FileData> myFiles = widgetsManager->getUserInfo()->getMyFiles();
            for (int i = 0; i < myFiles.size(); i++){
                FileData fData = myFiles.at(i);
                if (fData.fileName == fileName){
                    myFiles.erase(myFiles.begin() + i);
                    break;
                }
            }
            widgetsManager->getUserInfo()->setMyFiles(myFiles);
            bool success = widgetsManager->getUserInfo()->updateUserFiles(this);
            if (success){
                refreshList();
                writeFileNameToDownload("File Deleted", 2);
                writeFileDownloadProgress("", 0);
            }
            else {
                writeFileNameToDownload("Error deleting file.", 0);
                writeFileDownloadProgress("", 0);
            }
        }
        else {
            writeFileNameToDownload(reply->errorString(), 0);
            writeFileDownloadProgress("", 0);
        }
        enableButtons();
    }
}
