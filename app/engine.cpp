#include "engine.h"

#include <QtConcurrent/QtConcurrent>
#include <QMetaObject>
#include <QThreadPool>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDebug>

Engine::Engine(QObject *parent) : QObject{parent},
    m_totalBytes(0),
    m_remainingBytes(0),
    m_bytesOfStatistics(0),
    m_progress(0),
    m_canceled(false)
{
    qRegisterMetaType<Engine::Result>("Engine::Result");
    qRegisterMetaType<QList<Engine::Result>>("QList<Engine::Result>");

    connect(this, &Engine::progressTask, this, &Engine::_on_progressTask);

    m_timerOfStatistics = new QTimer(this);
    connect(m_timerOfStatistics, &QTimer::timeout, this, &Engine::_on_timeoutOfStatistics);
    m_timerOfStatistics->start(1000);
}

void Engine::_on_run(QString source, QString target)
{
    m_canceled = false;

    QVector<QPair<QString, QString> > tasks;
    fillTasks(source, target, tasks);

    m_remainingBytes = m_totalBytes;
    m_bytesOfStatistics = m_totalBytes;

    runTasks(tasks);
}

void Engine::_on_cancel()
{
    m_canceled = true;
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection,
                              Q_ARG(bool, true),
                              Q_ARG(QList<Engine::Result>, m_completedTasks));
    reset();
}

void Engine::_on_progressTask(int countBytes)
{
    if (m_totalBytes == 0) {
        return;
    }

    m_remainingBytes -= countBytes;
    const int curr_progress = double(m_totalBytes - m_remainingBytes) / m_totalBytes * 100;
    if (m_progress != curr_progress) {
        QMetaObject::invokeMethod(this, "progress", Qt::QueuedConnection,
                                  Q_ARG(int, curr_progress));
        m_progress = curr_progress;
    }
}

void Engine::_on_timeoutOfStatistics()
{
    const int bytesSec = m_bytesOfStatistics - m_remainingBytes;
    QMetaObject::invokeMethod(this, "speedBytesSec", Qt::QueuedConnection,
                              Q_ARG(int, bytesSec));
    m_bytesOfStatistics = m_remainingBytes;
}

void Engine::reset()
{
    m_completedTasks.clear();
    m_totalBytes = 0;
    m_remainingBytes = 0;
    m_progress = 0;
    m_bytesOfStatistics = 0;
}

void Engine::fillTasks(QString source, QString target, QVector<QPair<QString, QString> >& tasks)
{
    QFileInfo srcInfo(source);
    if (srcInfo.isDir()) {
        QDir targetDir(target);
        targetDir.cdUp();

        target += QLatin1Char('/') + source.split(QLatin1Char('/')).back();
        targetDir.mkdir(target);

        QDir sourceDir(source);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        for (const QString &fileName : fileNames) {
            const QString newSrcPath = source + QLatin1Char('/') + fileName;
            fillTasks(newSrcPath, target, tasks);
        }
    } else {
        tasks.push_back({source, target + "/" + srcInfo.fileName()});
        m_totalBytes += srcInfo.size();
    }
}

void Engine::runTasks(const QVector<QPair<QString, QString> > &tasks)
{
    for (auto&& task : tasks) {
        const QString source = task.first;
        const QString target = task.second;

        QFuture<Result> resFuture = QtConcurrent::run(&m_pool, &Engine::copyFile, this, source, target);
        resFuture.then([this, source, target, tasks](Result&& result) {
            std::lock_guard<std::mutex> l(m_mutex);

            m_completedTasks.push_back(result);

            if (tasks.size() == m_completedTasks.size() && m_canceled == false) {
                const auto resTasks = m_completedTasks;
                reset();

                QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection,
                                          Q_ARG(bool, false),
                                          Q_ARG(QList<Engine::Result>, resTasks));
            }
        });
    }
}

Engine::Result Engine::copyFile(const QString &source, const QString &target)
{
    Result result;
    result.source = source;
    result.target = target;
    result.success = false;

    QFile fileSource(source);
    if (!fileSource.open(QIODevice::ReadOnly)) {
        qCritical() << "Don't open file " << source;
        return result;
    }

    QFile fileTarget(target);
    if (!fileTarget.open(QIODevice::WriteOnly)) {
        qCritical() << "Don't open file " << target;
        return result;
    }

    qint64 fSize = fileSource.size();
    qint64 fPos = fileSource.pos();
    while (fSize != fPos) {
        if (m_canceled) {
            return result;
        }

        constexpr int chunkSize = 4096;
        const auto data = fileSource.read(chunkSize);
        if (data.size() == 0) {
            qWarning() << "Cannot read data to file " << source;
            return result;
        }
        const auto writing = fileTarget.write(data);
        if (writing < 0) {
            qWarning() << "Cannot write data to file " << target;
            return result;
        }

        QMetaObject::invokeMethod(this, "progressTask", Qt::QueuedConnection,
                                  Q_ARG(int, writing));

        fPos = fileSource.pos();
    }

    fileSource.close();
    fileTarget.close();

    result.success = true;
    return result;
}
