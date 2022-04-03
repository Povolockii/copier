#include "log.h"

#include <QCoreApplication>
#include <QTextStream>
#include <QDateTime>
#include <QFile>

namespace {
QFile logFile = QFile(QCoreApplication::applicationDirPath() + "/log.txt");
}

namespace logging {

bool openFileLog()
{
    return logFile.open(QFile::Append | QFile::Text);
}

void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(&logFile);

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
}

}
