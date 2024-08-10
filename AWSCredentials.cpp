#include "AWSCredentials.hpp"
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QTextStream>

AWSCredentials::AWSCredentials(QObject *parent)
    : QObject(parent)
{
    auto filename = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                    + "/.aws/credentials";
    iniFile = new QSettings(filename, QSettings::IniFormat, this);
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

void AWSCredentials::setProfileFromText(const QString &profile, const QString &text) {
    auto tempFile = QTemporaryFile();
    if (tempFile.open()) {
        QTextStream textStream(&tempFile);
        textStream << text;
        tempFile.close();

        auto tempSettings = QSettings(tempFile.fileName(), QSettings::IniFormat);
        setAccessKey(profile, tempSettings.value("default/aws_access_key").toString());
        setSecretAccessKey(profile, tempSettings.value("default/aws_secret_access_key").toString());
        setSessionToken(profile, tempSettings.value("default/aws_session_token").toString());
        sync();
    } else {
        qCritical("failed to open temporary file");
    }
}
