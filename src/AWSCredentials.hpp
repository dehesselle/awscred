/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef AWSCREDENTIALS_HPP
#define AWSCREDENTIALS_HPP

#include <QStringList>
#include "IniFile.hpp"

class AWSCredentials : public QObject
{
    Q_OBJECT

public:
    AWSCredentials(QObject *parent = nullptr);

    QStringList getProfiles();
    static bool containsCredentials(const QString &text);

    void setAccessKeyId(const QString &profile, const QString &accessKeyId);
    void setSecretAccessKey(const QString &profile, const QString &secretAccessKey);
    void setSessionToken(const QString &profile, const QString &sessionToken);
    bool setProfileFromText(const QString &profile, const QString &text);

private:
    IniFile *iniFile;
};

#endif // AWSCREDENTIALS_HPP
