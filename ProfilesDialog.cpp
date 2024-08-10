#include "ProfilesDialog.hpp"
#include <QAction>
#include <QFont>
#include <QIcon>
#include <QMenu>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QtDebug>
#include "./ui_ProfilesDialog.h"
#include "AWSCredentials.hpp"

ProfilesDialog::ProfilesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfilesDialog)
{
    ui->setupUi(this);
    populate();
    createTrayIcon();
}

ProfilesDialog::~ProfilesDialog()
{
    delete ui;
}

void ProfilesDialog::populate()
{
    // create new empty container in scroll area
    auto widget = new QWidget();
    widget->setLayout(new QVBoxLayout());
    ui->saProfiles->setWidget(widget);

    // populate with buttons
    auto credentials = AWSCredentials();
    foreach (QString profile, credentials.getProfiles()) {
        addButton(profile);
    }
}

void ProfilesDialog::addButton(const QString &profile)
{
    QPushButton *button = new QPushButton(profile, this);

    button->setObjectName(profile);
    button->setFont(QFont("Monospace", 20));
    button->setMinimumHeight(80);

    connect(button, &QPushButton::clicked, this, [this, button]() {
        updateProfile(button->objectName());
    });
    ui->saProfiles->widget()->layout()->addWidget(button);
}

void ProfilesDialog::createTrayIcon()
{
    QMenu *systrayIconMenu = new QMenu(this);
    QSystemTrayIcon *systrayIcon = new QSystemTrayIcon(this);
    systrayIcon->setContextMenu(systrayIconMenu);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, []() {
        qDebug() << "quit requested";
        QCoreApplication::quit();
    });
    systrayIconMenu->addAction(quitAction);

    QIcon *icon = new QIcon(":/resources/icon.png");
    systrayIcon->setIcon(*icon);

    systrayIcon->show();
}

void ProfilesDialog::updateProfile(const QString &profile)
{
    auto credentials = AWSCredentials();
    credentials.setProfileFromText(
        profile,
        "[default]\naws_access_key=hans\naws_secret_access_key=peter\naws_session_token=blub\n");

    qInfo() << "update profile: " << profile;
}
