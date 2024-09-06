/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "AWSCredentials.hpp"
#include <QApplication>
#include <QDir>
#include <QRegularExpression>
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

    iniFile = new IniFile(awsDir.filePath("credentials"), this);
}

QStringList AWSCredentials::getProfiles()
{
    return iniFile->getSections();
}

void AWSCredentials::setAccessKeyId(const QString &profile, const QString &accessKeyId)
{
    iniFile->setValue(profile, "aws_access_key_id", accessKeyId);
}

void AWSCredentials::setSecretAccessKey(const QString &profile, const QString &secretAccessKey)
{
    iniFile->setValue(profile, "aws_secret_access_key", secretAccessKey);
}

void AWSCredentials::setSessionToken(const QString &profile, const QString &sessionToken)
{
    iniFile->setValue(profile, "aws_session_token", sessionToken);
}

bool AWSCredentials::setProfileFromText(const QString &profile, const QString &text)
{
    auto tempFile = QTemporaryFile();
    if (tempFile.open()) {
        QTextStream textStream(&tempFile);
        textStream << text;
        tempFile.close();
        qDebug() << "tempfile is" << tempFile.fileName();

        auto tempIni = IniFile(tempFile.fileName());
        auto accessKeyId = tempIni.value("default", "aws_access_key_id");
        auto secretAccessKey = tempIni.value("default", "aws_secret_access_key");
        auto sessionToken = tempIni.value("default", "aws_session_token");

        if (accessKeyId.isEmpty() or secretAccessKey.isEmpty() or sessionToken.isEmpty()) {
            qCritical() << "unable to update profile " << profile << ":" << accessKeyId.length()
                        << secretAccessKey.length() << sessionToken.length();
            // Do not remove temporary file in debug mode.
            if (qApp->property("debug").toBool()) {
                tempFile.setAutoRemove(false);
            }
            return false;
        }

        setAccessKeyId(profile, accessKeyId);
        setSecretAccessKey(profile, secretAccessKey);
        setSessionToken(profile, sessionToken);
        return true;
    } else {
        qCritical() << "failed to create a temporary file";
        return false;
    }
}

bool AWSCredentials::containsCredentials(const QString &text)
{
    // step 1: simple string check for one mandatory string
    if (text.contains("aws_secret_access_key=")) {
        // step 2: use a regex to check for the whole (multiline-) string
        QRegularExpression regex;
        regex.setPatternOptions(QRegularExpression::MultilineOption);
        regex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
        regex.setPattern(
            R"(\[default\].aws_access_key_id=.+aws_secret_access_key=.+aws_session_token=.+)");
        return regex.globalMatch(text).hasNext();
    }
    return false;
}
