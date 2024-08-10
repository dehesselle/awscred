#include "ProfilesDialog.hpp"
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include "./ui_ProfilesDialog.h"
#include "AWSCredentials.hpp"

ProfilesDialog::ProfilesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfilesDialog)
{
    ui->setupUi(this);

    auto awsCred = AWSCredentials();

    awsCred.setProfileFromText(
        "foo",
        "[default]\naws_access_key=hans\naws_secret_access_key=peter\naws_session_token=blub\n");

    createTrayIcon();
}

ProfilesDialog::~ProfilesDialog()
{
    delete ui;
}

void ProfilesDialog::createTrayIcon()
{
    QMenu *systrayIconMenu = new QMenu(this);
    QSystemTrayIcon *systrayIcon = new QSystemTrayIcon(this);
    systrayIcon->setContextMenu(systrayIconMenu);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    systrayIconMenu->addAction(quitAction);

    QIcon *icon = new QIcon(":/resources/icon.png");
    systrayIcon->setIcon(*icon);

    systrayIcon->show();
}
