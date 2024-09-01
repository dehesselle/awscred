/* SPDX-FileCopyrightText: 2024 René de Hesselle <dehesselle@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "ProfilesDialog.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QSharedMemory>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <QtDebug>
#include <private/qguiapplication_p.h>
#include <stdio.h>
#include <stdlib.h>

// https://doc.qt.io/qt-6/qtlogging.html
QtMessageHandler ORIGINAL_HANDLER = nullptr;
auto MESSAGE_PATTERN = "%{time yyMMdd-hmmss} | "
                       "%{if-debug}DBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARN%{endif}%{if-"
                       "critical}CRIT%{endif}%{if-fatal}FATL%{endif} | %{function} | %{message}";

void logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message = qFormatLogMessage(type, context, msg);
    static FILE *f = fopen(qPrintable(
                               QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                                   .filePath("awscred.log")),
                           "a");
    fprintf(f, "%s\n", qPrintable(message));
    fflush(f);

    if (ORIGINAL_HANDLER)
        ORIGINAL_HANDLER(type, context, msg);
}

int main(int argc, char *argv[])
{
    ORIGINAL_HANDLER = qInstallMessageHandler(logToFile);
    qSetMessagePattern(MESSAGE_PATTERN);
    qInfo() << "begin log";

    QApplication app(argc, argv);

    { // check if we should run in debug mode
        QCommandLineParser parser;
        parser.addPositionalArgument("debug", "run in debug mode");
        parser.process(app);
        if (parser.positionalArguments().contains("debug")) {
            app.setProperty("debug", true);
        } else {
            app.setProperty("debug", false);
            // Disable debug logging.
            QLoggingCategory::setFilterRules("default.debug=false\n");
            // Disable logging to console.
            ORIGINAL_HANDLER = nullptr;
        }
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "awscred_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        QApplication::setQuitOnLastWindowClosed(false);

        QSharedMemory shared("7df53930-074b-43a3-be84-4be1ecf642b5"); // uuidgen
        if (shared.create(512, QSharedMemory::ReadWrite)) {
// Force windows to always pop up in front of all others when calling activate().
#ifdef Q_OS_WIN
            if (auto interface = qApp->nativeInterface<QNativeInterface::Private::QWindowsApplication>()) {
                interface->setWindowActivationBehavior(QNativeInterface::Private::QWindowsApplication::AlwaysActivateWindow);
            }
#endif

            ProfilesDialog w;
            return app.exec();
        } else {
            qCritical() << "not starting again, only one application instance allowed";
            return 1;
        }
    } else {
        qCritical() << "system tray not available";
        QMessageBox::critical(nullptr,
                              QObject::tr("system tray"),
                              QObject::tr("System tray not found!"),
                              QMessageBox::Close);
        return 1;
    }
}
