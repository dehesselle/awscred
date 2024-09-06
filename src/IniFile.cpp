/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "IniFile.hpp"

IniFile::IniFile(const QString &filename, QObject *parent)
    : QObject{parent}
    , filename(filename)
{
    ini.Load(filename.toStdString());
}

IniFile::~IniFile()
{
    ini.Save(filename.toStdString());
}

QStringList IniFile::getSections()
{
    QStringList result;
    for (INI::File::sections_iter it = ini.SectionsBegin(); it != ini.SectionsEnd(); ++it) {
        result.append(QString::fromStdString(it->first));
    }
    return result;
}

void IniFile::setValue(const QString &section, const QString &key, const QString &value)
{
    ini.GetSection(section.toStdString())->SetValue(key.toStdString(), value.toStdString());
}

QString IniFile::value(const QString &section, const QString &key)
{
    return QString::fromStdString(
        ini.GetSection(section.toStdString())->GetValue(key.toStdString()).AsString());
}
