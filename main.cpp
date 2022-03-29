#include "mainwindow.h"

#include <ostream>

#include <QApplication>
#include <QScopedPointer>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

static QScopedPointer<QFile> m_logFile;

void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    m_logFile.reset(new QFile(QCoreApplication::applicationDirPath() + "/log.txt"));
    m_logFile.data()->open(QFile::Append | QFile::Text);
    qInstallMessageHandler(logMessageOutput);

    MainWindow w;
    w.show();

    return a.exec();
}

void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(m_logFile.data());

    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type)
    {
    case QtInfoMsg:     out << "info "; break;
    case QtDebugMsg:    out << "debug "; break;
    case QtWarningMsg:  out << "warning "; break;
    case QtCriticalMsg: out << "critical "; break;
    case QtFatalMsg:    out << "fatal "; break;
    }

    out << context.category << ": " << msg << Qt::endl;
    out.flush();
}
