/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef PROFILESDIALOG_HPP
#define PROFILESDIALOG_HPP

#include <QAction>
#include <QDialog>
#include "version.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class ProfilesDialog;
}
QT_END_NAMESPACE

class ProfilesDialog : public QDialog
{
    Q_OBJECT

public:
    ProfilesDialog(QWidget *parent = nullptr);
    ~ProfilesDialog();

    void populate();
    void addProfile(const QString &profile);

public slots:
    bool updateProfile(const QString &profile);
    void parseClipboard();
    void showAboutDialog();
    void showAboutQtDialog();

private slots:
    void on_pbClose_clicked();
    void on_pbNew_clicked();

private:
    void createTrayIcon();
    void setEnabledForAllProfiles(const bool &isEnabled);

    Ui::ProfilesDialog *ui;
};
#endif // PROFILESDIALOG_HPP
