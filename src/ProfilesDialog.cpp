/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "ProfilesDialog.hpp"
#include <QAction>
#include <QClipboard>
#include <QFont>
#include <QIcon>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QtDebug>
#include "AWSCredentials.hpp"
#include "src/ui_ProfilesDialog.h"

ProfilesDialog::ProfilesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfilesDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(QPixmap(":/icon.svg")));
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

void ProfilesDialog::showAboutDialog()
{
    QMessageBox::about(this,
                       "AWS Credential Helper",
                       "This tool monitors your clipboard for AWS credentials and offers to\n"
                       "update/create a profile in your ~/.aws/credentials file.\n\n"
                       "Sourcecode available: https://github.com/dehesselle/awscred\n"
                       "Created using Qt."
                       );
}

void ProfilesDialog::createTrayIcon()
{
    QMenu *systrayIconMenu = new QMenu(this);
    QSystemTrayIcon *systrayIcon = new QSystemTrayIcon(this);
    systrayIcon->setContextMenu(systrayIconMenu);

    QAction *aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, &QAction::triggered, this, &ProfilesDialog::showAboutDialog);
    systrayIconMenu->addAction(aboutAction);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, []() {
        qDebug() << "quit requested";
        QCoreApplication::quit();
    });
    systrayIconMenu->addAction(quitAction);

    // click systray icon to show dialog in read-only mode
    // (read-only is for non-debug builds only)
    connect(systrayIcon,
            &QSystemTrayIcon::activated,
            this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (QSystemTrayIcon::Trigger == reason) {
#ifndef QT_DEBUG
                    this->ui->saProfiles->setDisabled(true);
                    this->ui->pbNew->setDisabled(true);
#endif
                    this->ui->lblDescription->setText(
                        tr("The following profiles have been found."));
                    this->show();
                }
            });

    QIcon *icon = new QIcon(QPixmap(":/icon.svg"));
    systrayIcon->setIcon(*icon);

    systrayIcon->show();
}

bool ProfilesDialog::updateProfile(const QString &profile)
{
    bool result = false;

    auto credentials = AWSCredentials();
    if (credentials.setProfileFromText(profile, QApplication::clipboard()->text())) {
        hide();
        qInfo() << "update profile" << profile;
        result = true;
    } else {
        QMessageBox::critical(this,
                              QObject::tr("update profile"),
                              QObject::tr("Failed to update profile.\nNo changes have been made."),
                              QMessageBox::Close);
    }

    return result;
}

void ProfilesDialog::parseClipboard()
{
    qDebug() << __FUNCTION__;
    auto text = QApplication::clipboard()->text();

    if (AWSCredentials::containsCredentials(text)) {
        populate();
        ui->saProfiles->setDisabled(false);
        ui->pbNew->setDisabled(false);
        ui->lblDescription->setText(tr("Which profile do you want to update?"));
        show();
        activateWindow();
    }
}

void ProfilesDialog::on_pbClose_clicked()
{
    this->hide();
}

void ProfilesDialog::on_pbNew_clicked()
{
    bool isOk = false;
    QString profile = QInputDialog::getText(this,
                                            tr("create new profile"),
                                            tr("profile name:"),
                                            QLineEdit::Normal,
                                            "default",
                                            &isOk);

    if (isOk and not profile.isEmpty()) {
        qInfo() << "new profile" << profile;
        if (updateProfile(profile)) {
            populate();
        }
    }
}
