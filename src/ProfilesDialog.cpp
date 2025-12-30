/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "ProfilesDialog.hpp"
#include <QAction>
#include <QClipboard>
#include <QFont>
#include <QFontDatabase>
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
#include "version.hpp"

ProfilesDialog::ProfilesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfilesDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(QPixmap(":/icon.svg")));
    populate();
    createTrayIcon();
    setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
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
    for (auto profile : credentials.getProfiles()) {
        qDebug() << "profile found:" << profile;
        addProfile(profile);
    }
}

void ProfilesDialog::addProfile(const QString &profile)
{
    QPushButton *button = new QPushButton(profile, this);

    button->setObjectName(profile);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(20);
    font.setBold(true);
    button->setFont(font);
    button->setMinimumHeight(60);

    connect(button, &QPushButton::clicked, this, [this, button]() {
        updateProfile(button->objectName());
    });
    ui->saProfiles->widget()->layout()->addWidget(button);
}

void ProfilesDialog::showAboutDialog()
{
    QMessageBox::about(this,
                       "About",
                       QString("AWS Credential Helper, version %1\n\n").arg(PROJECT_VERSION)
                           + "This tool monitors your clipboard for AWS credentials and offers\n"
                             "to update/create a profile in your ~/.aws/credentials file.\n\n"
                             "https://github.com/dehesselle/awscred\n"
                             "Licensed under GPL-2.0-or-later.\n");
}

void ProfilesDialog::showAboutQtDialog()
{
    QMessageBox::aboutQt(this);
}

void ProfilesDialog::createTrayIcon()
{
    QMenu *systrayIconMenu = new QMenu(this);
    QSystemTrayIcon *systrayIcon = new QSystemTrayIcon(this);
    systrayIcon->setContextMenu(systrayIconMenu);

    QAction *aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, &QAction::triggered, this, &ProfilesDialog::showAboutDialog);
    systrayIconMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About Qt"), this);
    connect(aboutQtAction, &QAction::triggered, this, &ProfilesDialog::showAboutQtDialog);
    systrayIconMenu->addAction(aboutQtAction);

    systrayIconMenu->addAction(systrayIconMenu->addSeparator());

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, []() {
        qInfo() << "quit requested";
        QCoreApplication::quit();
    });
    systrayIconMenu->addAction(quitAction);

    // click systray icon to show dialog in read-only mode
    connect(systrayIcon,
            &QSystemTrayIcon::activated,
            this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (QSystemTrayIcon::Trigger == reason) {
                    // disable editing capabilities unless we're in debug mode
                    if (not qApp->property("debug").toBool()) {
                    this->setEnabledForAllProfiles(false);
                    this->ui->pbNew->setEnabled(false);
                    }

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
    qDebug() << "parsing clipboard";
    auto text = QApplication::clipboard()->text();

    if (AWSCredentials::containsCredentials(text)) {
        populate();
        setEnabledForAllProfiles(true);
        ui->pbNew->setEnabled(true);
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

void ProfilesDialog::setEnabledForAllProfiles(const bool &isEnabled)
{
    for (auto button : ui->saProfiles->widget()->findChildren<QPushButton *>()) {
        button->setEnabled(isEnabled);
    }
}
