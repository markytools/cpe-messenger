#ifndef MYFILES_H
#define MYFILES_H

#include <QWidget>
#include <qwidgetmanager.h>
#include <vector>
#include <userinfo.h>
#include <iostream>
#include <QtGlobal>
#include <QFileInfo>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <utils.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <uploadfile.h>
#include "settings.h"

using namespace std;

namespace Ui {
class MyFiles;
}

class MyFiles : public QWidget
{
    Q_OBJECT

public:
    explicit MyFiles(QWidget *parent = 0);
    ~MyFiles();

    void setWidgetsManager(QWidgetManager *value);
    void updateList();

    QListWidget *getListOfFilesWidget() const;
    void refreshList();

private slots:
    void on_back_clicked();
    void on_upload_clicked();
    void on_download_clicked();

    void progressChanged(qint64 a, qint64 b);
    void readingReadyBytes(QNetworkReply *reply);
    void readingReadyBytesDesktop(QNetworkReply *reply);

    void on_deleteB_clicked();

private:
    Ui::MyFiles *ui;
    QWidgetManager *widgetsManager;

    QString currentFileName;
    QString targetSavePath;
    QString returnFileURL(QString fileName);
    QString getDownloadDir();
    string downloadedFileExtension;
    QString downloadPath;

    void writeFileNameToDownload(QString fileName, int state);
    void writeFileDownloadProgress(QString progress, int state);
    bool ifFileExist(QString fileName);
    void disableButtons();
    void enableButtons();

protected:
    void showEvent(QShowEvent *);
};

#endif // MYFILES_H
