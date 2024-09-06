/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef INIFILE_HPP
#define INIFILE_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include "iniparser.hpp"

class IniFile : public QObject
{
    Q_OBJECT
public:
    explicit IniFile(const QString &filename, QObject *parent = nullptr);
    ~IniFile();

    QStringList getSections();
    void setValue(const QString &section, const QString &key, const QString &value);
    QString value(const QString &section, const QString &key);

private:
    INI::File ini;
    QString filename;
};

#endif // INIFILE_HPP
