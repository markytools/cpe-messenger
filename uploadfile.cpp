#include "uploadfile.h"
#include "ui_uploadfile.h"

UploadFile::UploadFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploadFile)
{
    ui->setupUi(this);

    ui->response->setWordWrap(true);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->reset();
}

UploadFile::~UploadFile()
{
    delete ui;
}
void UploadFile::setWidgetsManager(QWidgetManager *value)
{
    widgetsManager = value;
}

void UploadFile::setListOfFiles(QListWidget *value)
{
    listOfFiles = value;
}


void UploadFile::on_browse_clicked()
{
    resetUI();
    const QStringList downloadPaths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
    currentFileToUploadPath = QFileDialog::getOpenFileName(this, tr("Select File to Upload"),
                                                           downloadPaths.isEmpty() ? QDir::homePath() : downloadPaths.first(),
                                                           tr("All files (*.*)"));
    ui->filePath->setText(currentFileToUploadPath);

    string path = currentFileToUploadPath.toStdString();
    fileName = QString::fromStdString(path.substr(path.find_last_of("/\\") + 1));
    ui->fileName->setText(fileName);
}

void UploadFile::on_upload_clicked()
{
    if (ui->fileName->text().isEmpty() || ui->filePath->text().isEmpty()){
        flagErrors("Please select a file to upload.");
        return;
    }

    if (duplicateFileName(fileName)){
        flagErrors("File already exists.");
        return;
    }

    disableButtons();
    ui->response->setText("<font color='blue'>Uploading...</font>");
    QString filePath = widgetsManager->getUserInfo()->getProfName();
    QString userToken = widgetsManager->getUserInfo()->getUserToken();
    QFile *fileToUpload = new QFile(currentFileToUploadPath);

    QString bound = "HBfDmPIxA7FXHiJHTX593184";
    QString crlf;
    crlf = 0x0d;
    crlf += 0x0a;
    QString data;
    QByteArray dataToSend;

    if (fileToUpload->open(QIODevice::ReadOnly)){
        data = "--" + bound + crlf + "Content-Disposition: form-data; name=\"upload\"; ";
        data += "filename=\"" + fileName + "\"";
        data += crlf + "Content-Type: application/octet-stream" + crlf + crlf;
        dataToSend.insert(0, data);
        dataToSend.append(fileToUpload->readAll());
        dataToSend.append(crlf + "--" + bound + "--" + crlf);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply *)),
                         SLOT(uploadFinish(QNetworkReply *)));

        QUrl url("https://api.backendless.com/" + Settings::APPLICATION_ID + "/" + Settings::REST_API_KEY
                 + "/files/userFiles/" + filePath + "Files/" + fileName);
        QNetworkRequest request(url);
        request.setRawHeader("user-token", userToken.toUtf8());
        request.setRawHeader("application-type", Settings::APPLICATION_TYPE);
        request.setHeader(QNetworkRequest::ContentTypeHeader, tr("multipart/form-data; boundary=") + bound);

        QNetworkReply *reply = manager->post(request, dataToSend);
//        QNetworkReply *reply = manager->post(request, fileToUpload->readAll());
        fileToUpload->setParent(reply);
        connect(reply, SIGNAL(uploadProgress(qint64, qint64)), SLOT(progressChanged(qint64, qint64)));
    }
    else {
        ui->progressBar->reset();
        enableButtons();
        flagErrors("File Error.");
    }
}

void UploadFile::on_back_clicked()
{
    listOfFiles->clear();
    for (FileData &fData : widgetsManager->getUserInfo()->getMyFiles()){
        listOfFiles->addItem(fData.fileName);
    }
    resetUI();
    widgetsManager->showWidget(MY_FILES);
}

void UploadFile::progressChanged(qint64 a, qint64 b)
{
    qDebug() << a << "  " << b << endl;
    if (b > 0) {
        ui->progressBar->setValue(((float)a/(float)b)*100);
    }
}

void UploadFile::uploadFinish(QNetworkReply *reply)
{
    if (reply->error() == 0){
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        QString fileURL = json.value("fileURL").toString();

        FileData fData;
        fData.fileName = ui->fileName->text();
        fData.fileURL = fileURL;
        widgetsManager->getUserInfo()->addAdditionalFiles(fData);

        bool filesUploadComplete = widgetsManager->getUserInfo()->updateUserFiles(this);
        if (filesUploadComplete){
            enableButtons();
            ui->response->setText("<font color='green'>Uploading Complete.</font>");
        }
        else {
            ui->progressBar->reset();
            enableButtons();
            flagErrors("There was a problem uploading the file.");
        }
    }
    else {
        ui->progressBar->reset();
        enableButtons();
        flagErrors(reply->errorString());
    }
    reply->deleteLater();
}

void UploadFile::flagErrors(QString message)
{
    ui->response->setText("<font color='red'>" + message + "</font>");
}

void UploadFile::resetUI()
{
    ui->fileName->clear();
    ui->filePath->clear();
    ui->progressBar->reset();
    ui->response->clear();
}

void UploadFile::disableButtons()
{
    ui->browse->setDisabled(true);
    ui->back->setDisabled(true);
    ui->upload->setDisabled(true);
}

void UploadFile::enableButtons()
{
    ui->browse->setDisabled(false);
    ui->back->setDisabled(false);
    ui->upload->setDisabled(false);
}

bool UploadFile::duplicateFileName(QString name)
{
    vector<FileData> filesData = widgetsManager->getUserInfo()->getMyFiles();
    for (FileData &fData : filesData){
        if (fData.fileName == name) return true;
    }
    return false;
}

QByteArray UploadFile::buildUploadString(QFile *file, QString fileName)
{
    //    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    //    data.append("Content-Disposition: form-data; name=\"uploaded\"; filename=" + fileName + "\r\n"); //name of the input is "uploaded" in my form, next one is a file name.
    //    data.append("Content-Type: text\r\n\r\n"); //data type
    //    QFile file(path);
    //    if (!file.open(QIODevice::ReadOnly))
    //    return;
    //    data.append(file.readAll()); //let's read the file
    //    data.append("\r\n");
    //    data.append("--" + bound + "--\r\n"); //closing boundary according to rfc 1867
    return NULL;
}
