#include "ProfilesDialog.hpp"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QSharedMemory>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTranslator>
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
    qDebug("begin log");

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "awscred_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        QApplication::setQuitOnLastWindowClosed(false);

        QSharedMemory shared("7df53930-074b-43a3-be84-4be1ecf642b5"); // uuidgen
        if (shared.create(512, QSharedMemory::ReadWrite)) {
            ProfilesDialog w;
            return a.exec();
        } else {
            qCritical("only one application instance allowed");
            return 1;
        }
    } else {
        QMessageBox::critical(nullptr,
                              QObject::tr("system tray"),
                              QObject::tr("System tray not found!"),
                              QMessageBox::Close);
        qCritical("system tray not found");
        return 1;
    }
}
