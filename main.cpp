/*utf8*/
#include "maindialog.h"
#include <QApplication>
#include <QFile>
#include <QMutex>
#include <QLockFile>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>

QFile   g_logFile("SignalSave2.log");
QMutex  g_mutex;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {

    QMutexLocker locker(&g_mutex);
    QString txt;

    QDateTime date_time = QDateTime::currentDateTime ();
    QString   stamp     = date_time.toString("hh:mm:ss.zzz");

    switch (type) {
    case QtInfoMsg:
        txt = QString("%1 <%2> INFO: [%3 %4] %5\r\n").arg(stamp).arg(context.category).arg(context.file).arg(context.line).arg(msg);
        break;
    case QtDebugMsg:
        txt = QString("%1 <%2> DEBUG: [%3 %4] %5\r\n").arg(stamp).arg(context.category).arg(context.file).arg(context.line).arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("%1 <%2> WARNING: [%3 %4] %5\r\n").arg(stamp).arg(context.category).arg(context.file).arg(context.line).arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("%1 <%2> CRITICAL: [%3 %4] %5\r\n").arg(stamp).arg(context.category).arg(context.file).arg(context.line).arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("%1 <%2> FATAL: [%3 %4] %5\r\n").arg(stamp).arg(context.category).arg(context.file).arg(context.line).arg(msg);
        break;
    }

    g_logFile.write(txt.toLocal8Bit());
    g_logFile.flush();
}

int main(int argc, char *argv[]){

    qRegisterMetaType<MCU::SDU_MYOGRAPH_DATA>("MCU::SDU_MYOGRAPH_DATA");

    g_logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    qInstallMessageHandler(myMessageOutput);

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QLockFile lockFile(QDir::temp().absoluteFilePath(QString::fromUtf8("SignalSave2.lock")));
    /* Trying to close the Lock File, if the attempt is unsuccessful for 100 milliseconds,
        * then there is a Lock File already created by another process.
        / Therefore, we throw a warning and close the program
        * */
    if(!lockFile.tryLock(100)){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(QString::fromUtf8("Приложение уже запущено"));
        msgBox.exec();
        g_logFile.close();
        return 1;
    }

    int retcode;
    { // чтобы деструктор окна записался в отладку перед закрытием файла
        MainDialog w;
        //w.setWindowState(w.windowState() ^ Qt::WindowFullScreen);
        //w.setWindowState(Qt::WindowMaximized);
        w.show();
        retcode = a.exec();
    }

    g_logFile.close();
    return retcode;
}
