#include "cpemessenger.h"
#include "ui_cpemessenger.h"

CPEMessenger::CPEMessenger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPEMessenger)
{
    ui->setupUi(this);

    widgetsManager = new QWidgetManager();
    newAccount = new Register();
    regSuccess = new RegisterSuccess();
    passwordRecovery = new PasswordRecovery();
    login = new Login();
    login->setPasswordRecovery(passwordRecovery);
    messengerHome = new MessengerHome();
    myFiles = new MyFiles();
    uploadFile = new UploadFile();
    uploadFile->setListOfFiles(myFiles->getListOfFilesWidget());
    groupNotifications = new GroupNotifications();
    groupSelection = new GroupSelection();
    groupSelection->setGroupNotifications(groupNotifications);
    newGroup = new NewGroup();
    joinGroup = new JoinGroup();
    buddies = new Buddies();
    buddies->setGroupNotifications(groupNotifications);
    jackpots = new Jackpots();
    jackpotsHome = new JackpotsHome();
    jackpotsRoom = new JackpotsRoom();
    jackpotsHome->setJackpots(jackpots);
    jackpotsHome->setJackpotsRoom(jackpotsRoom);
    jackpotsRoom->setJackpots(jackpots);

    widgetsManager->addWidget(this, HOME);
    widgetsManager->addWidget(newAccount, REG);
    widgetsManager->addWidget(regSuccess, REG_SUCCESS);
    widgetsManager->addWidget(login, LOGIN);
    widgetsManager->addWidget(messengerHome, MSGR_HOME);
    widgetsManager->addWidget(myFiles, MY_FILES);
    widgetsManager->addWidget(uploadFile, UPLOAD_FILE);
    widgetsManager->addWidget(groupSelection, GROUP_SELECTION);
    widgetsManager->addWidget(newGroup, CREATE_GROUP);
    widgetsManager->addWidget(joinGroup, JOIN_GROUP);
    widgetsManager->addWidget(groupNotifications, MESSAGING);
    widgetsManager->addWidget(passwordRecovery, PASS_RECOVERY);
    widgetsManager->addWidget(buddies, BUDDIES);
    widgetsManager->addWidget(jackpots, JACKPOTS);
    widgetsManager->addWidget(jackpotsRoom, JACKPOTS_ROOM);
    widgetsManager->addWidget(jackpotsHome, JACKPOTS_HOME);

    newAccount->setWidgetsManager(widgetsManager);
    regSuccess->setWidgetsManager(widgetsManager);
    login->setWidgetsManager(widgetsManager);
    messengerHome->setWidgetsManager(widgetsManager);
    myFiles->setWidgetsManager(widgetsManager);
    uploadFile->setWidgetsManager(widgetsManager);
    groupSelection->setWidgetsManager(widgetsManager);
    newGroup->setWidgetsManager(widgetsManager);
    joinGroup->setWidgetsManager(widgetsManager);
    groupNotifications->setWidgetsManager(widgetsManager);
    passwordRecovery->setWidgetsManager(widgetsManager);
    buddies->setWidgetsManager(widgetsManager);
    jackpots->setWidgetsManager(widgetsManager);
    jackpotsHome->setWidgetsManager(widgetsManager);
    jackpotsRoom->setWidgetsManager(widgetsManager);
}

CPEMessenger::~CPEMessenger()
{
    delete ui;
}

void CPEMessenger::on_registerB_clicked()
{
    widgetsManager->showWidget(REG);
}

void CPEMessenger::on_login_clicked()
{
    widgetsManager->showWidget(LOGIN);
}
