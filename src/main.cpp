#include "ProfilesDialog.hpp"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <stdio.h>
#include <stdlib.h>

// https://doc.qt.io/qt-6/qtlogging.html
QtMessageHandler originalHandler = nullptr;
auto MESSAGE_PATTERN = "%{time yyMMdd-hmmss} | "
                       "%{if-debug}DBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARN%{endif}%{if-"
                       "critical}CRIT%{endif}%{if-fatal}FATL%{endif} | %{function} | %{message}";

void logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message = qFormatLogMessage(type, context, msg);
    static FILE *f = fopen(qPrintable(
                               QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                                    + "/awscred.log")
                                   .path()),
                           "a");
    fprintf(f, "%s\n", qPrintable(message));
    fflush(f);

    if (originalHandler)
        originalHandler(type, context, msg);
}

int main(int argc, char *argv[])
{
    originalHandler = qInstallMessageHandler(logToFile);
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

        ProfilesDialog w;
        return a.exec();
    } else {
        QMessageBox::critical(nullptr,
                              QObject::tr("system tray"),
                              QObject::tr("System tray not found!"),
                              QMessageBox::Close);
        qCritical("system tray not found");
        return 1;
    }
}
