/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "IniFile.hpp"
#include <QDebug>
#include <sstream>

IniFile::IniFile(QObject *parent)
    : QObject(parent)
{}

IniFile::IniFile(const QString &fileName, QObject *parent)
    : QObject(parent)
{
    load(fileName);
}

IniFile::~IniFile()
{
    save();
}

QStringList IniFile::getSections()
{
    QStringList result;
    for (INI::File::sections_iter it = parser.SectionsBegin(); it != parser.SectionsEnd(); ++it) {
        result.append(QString::fromStdString(it->first));
    }
    return result;
}

void IniFile::setValue(const QString &section, const QString &key, const QString &value)
{
    parser.GetSection(section.toStdString())->SetValue(key.toStdString(), value.toStdString());
}

QString IniFile::value(const QString &section, const QString &key)
{
    return QString::fromStdString(
        parser.GetSection(section.toStdString())->GetValue(key.toStdString()).AsString());
}

void IniFile::load(const QString &fileName)
{
    this->fileName = fileName;
    parser.Load(fileName.toStdString());
}

void IniFile::save(QString fileName)
{
    if (fileName.isEmpty()) {
        if (this->fileName.isEmpty()) {
            qCritical() << "cannot save, no filename";
        } else {
            parser.Save(this->fileName.toStdString());
        }
    } else {
        parser.Save(fileName.toStdString());
    }
}

IniFile *IniFile::fromString(const QString &text, QObject *parent)
{
    IniFile *iniFile = new IniFile(parent);
    std::istringstream stream(text.toStdString());
    iniFile->parser.Load(stream);
    return iniFile;
}
