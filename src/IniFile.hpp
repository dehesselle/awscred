/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef INIFILE_HPP
#define INIFILE_HPP

#include <QObject>
#include <QString>
#include "iniparser.hpp"

class IniFile : public QObject
{
    Q_OBJECT
public:
    IniFile(QObject *parent = nullptr);
    IniFile(const QString &fileName, QObject *parent = nullptr);
    ~IniFile();

    QStringList getSections();
    void setValue(const QString &section, const QString &key, const QString &value);
    QString value(const QString &section, const QString &key);

    void load(const QString &fileName);
    void save(QString fileName = "");

    static IniFile *fromString(const QString &text, QObject *parent = nullptr);

private:
    INI::File parser;
    QString fileName;
};

#endif // INIFILE_HPP
