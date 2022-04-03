#ifndef LOG_H
#define LOG_H

#include <qlogging.h>

namespace logging {

bool openFileLog();
void logMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

} //namespace logging

#endif // LOG_H
