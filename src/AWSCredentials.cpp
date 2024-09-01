/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "AWSCredentials.hpp"
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QTextStream>
#include <QtDebug>

AWSCredentials::AWSCredentials(QObject *parent)
    : QObject(parent)
{
    auto awsDir = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                     + "/.aws");
    if (awsDir.exists("credentials"))
        qDebug() << "credentials file found in" << awsDir.path();
    else
        qDebug() << "no credentials file";

    iniFile = new QSettings(awsDir.filePath("credentials"), QSettings::IniFormat, this);
    sync();
}

void AWSCredentials::sync()
{
    iniFile->sync();
}

int AWSCredentials::count()
{
    return iniFile->childGroups().count();
}

QStringList AWSCredentials::getProfiles()
{
    return iniFile->childGroups();
}

void AWSCredentials::setAccessKey(const QString &profile, const QString &accessKey)
{
    iniFile->setValue(profile + "/aws_access_key", accessKey);
}

void AWSCredentials::setSecretAccessKey(const QString &profile, const QString &secretAccessKey)
{
    iniFile->setValue(profile + "/aws_secret_access_key", secretAccessKey);
}

void AWSCredentials::setSessionToken(const QString &profile, const QString &sessionToken)
{
    iniFile->setValue(profile + "/aws_session_token", sessionToken);
}

bool AWSCredentials::setProfileFromText(const QString &profile, const QString &text)
{
    auto tempFile = QTemporaryFile();
    if (tempFile.open()) {
        QTextStream textStream(&tempFile);
        textStream << text;
        tempFile.close();
        qDebug() << "tempfile" << tempFile.fileName();

        auto tempSettings = QSettings(tempFile.fileName(), QSettings::IniFormat);
        auto accessKey = tempSettings.value("default/aws_access_key").toString();
        auto secretAccessKey = tempSettings.value("default/aws_secret_access_key").toString();
        auto sessionToken = tempSettings.value("default/aws_session_token").toString();

        if (accessKey.isEmpty() or secretAccessKey.isEmpty() or sessionToken.isEmpty()) {
            qCritical() << "unable to update profile " << profile << ":" << accessKey.length()
                        << secretAccessKey.length() << sessionToken.length();
            return false;
        }

        setAccessKey(profile, accessKey);
        setSecretAccessKey(profile, secretAccessKey);
        setSessionToken(profile, sessionToken);
        sync();
        return true;
    } else {
        qCritical() << "failed to create a temporary file";
        return false;
    }
}

bool AWSCredentials::containsCredentials(const QString &text)
{
    return text.contains("aws_secret_access_key=");
}
