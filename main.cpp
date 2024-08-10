#include "ProfilesDialog.hpp"

#include <QApplication>
#include <QLocale>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <stdio.h>
#include <stdlib.h>

// https://doc.qt.io/qt-6/qtlogging.html
QtMessageHandler originalHandler = nullptr;
auto MESSAGE_PATTERN = "[%{time yyyyMMdd h:mm:ss.zzz t} "
                       "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-"
                       "critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}";

void logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message = qFormatLogMessage(type, context, msg);
    static FILE *f = fopen("/tmp/log.txt", "a");
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
        w.show();
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
