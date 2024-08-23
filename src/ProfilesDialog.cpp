#include "ProfilesDialog.hpp"
#include <QAction>
#include <QClipboard>
#include <QFont>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
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
    connect(QApplication::clipboard(),
            &QClipboard::dataChanged,
            this,
            &ProfilesDialog::parseClipboard);
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

    QIcon *icon = new QIcon(":/icon.png");
    systrayIcon->setIcon(*icon);

    systrayIcon->show();
}

void ProfilesDialog::updateProfile(const QString &profile)
{
    auto credentials = AWSCredentials();
    if (credentials.setProfileFromText(profile, QApplication::clipboard()->text())) {
        hide();
        qInfo() << "profile updated: " << profile;
    } else {
        QMessageBox::critical(this,
                              QObject::tr("update profile"),
                              QObject::tr("Failed to update profile.\nNo changes have been made."),
                              QMessageBox::Close);
    }
}

void ProfilesDialog::parseClipboard()
{
    qDebug() << __FUNCTION__;
    auto text = QApplication::clipboard()->text();

    if (AWSCredentials::containsCredentials(text)) {
        populate();
        show();
    }
}