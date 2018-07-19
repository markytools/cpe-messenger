#ifndef UPLOADFILE_H
#define UPLOADFILE_H

#include <QWidget>
#include <QDialog>
#include <QStandardPaths>
#include <QFileDialog>
#include <qwidgetmanager.h>
#include <QListWidget>
#include <QHttpMultiPart>
#include "settings.h"

namespace Ui {
class UploadFile;
}

class UploadFile : public QWidget
{
    Q_OBJECT

public:
    explicit UploadFile(QWidget *parent = 0);
    ~UploadFile();

    void setWidgetsManager(QWidgetManager *value);
    void setListOfFiles(QListWidget *value);

private slots:
    void on_browse_clicked();
    void on_upload_clicked();
    void on_back_clicked();

    void progressChanged(qint64 a, qint64 b);
    void uploadFinish(QNetworkReply *reply);

private:
    Ui::UploadFile *ui;
    QWidgetManager *widgetsManager;
    QString currentFileToUploadPath;
    QString fileName;
    QListWidget *listOfFiles;

    void flagErrors(QString message);
    void resetUI();
    void disableButtons();
    void enableButtons();
    bool duplicateFileName(QString name);
    QByteArray buildUploadString(QFile *file, QString fileName);


    //public:
    //    explicit UploadDialog(QWidget *parent = 0);
    //    ~UploadDialog();

    //    void setWidgetsManager(QWidgetManager *value);
    //    void setListOfFiles(QListWidget *value);

    //private slots:
    //    void on_back_clicked();
    //    void on_browse_clicked();
    //    void on_upload_clicked();

    //    void progressChanged(qint64 a, qint64 b);
    //    void uploadFinish(QNetworkReply *reply);

    //private:
    //    Ui::UploadDialog *ui;
    //    QWidgetManager *widgetsManager;
    //    QString currentFileToUploadPath;
    //    QString fileName;
    //    QListWidget *listOfFiles;

    //    void flagErrors(QString message);
    //    void resetUI();
    //    void disableButtons();
    //    void enableButtons();
    //    bool duplicateFileName(QString name);
    //    QByteArray buildUploadString(QFile *file, QString fileName);
};

#endif // UPLOADFILE_H
