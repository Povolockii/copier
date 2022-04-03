#ifndef ENGINE_H
#define ENGINE_H

#include <atomic>

#include <QList>
#include <QTimer>
#include <QObject>
#include <QFuture>
#include <QThreadPool>

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = nullptr);

struct Result
{
    QString source;
    QString target;
    bool success;
};

public slots:
    void _on_run(QString source, QString target);
    void _on_cancel();

signals:
    void progressTask(int);

    void progress(int);
    void speedBytesSec(int);
    void finished(bool, QList<Engine::Result>);

private slots:
    void _on_progressTask(int count);
    void _on_timeoutOfStatistics();

private:
    void reset();
    void fillTasks(QString source, QString target, QVector<QPair<QString, QString> > &tasks);
    void runTasks(const QVector<QPair<QString, QString>>& tasks);
    Result copyFile(const QString &sourceFile, const QString &targetFile);

    qint64 m_totalBytes; //всего байт нужно скопировать
    qint64 m_remainingBytes; //сколько осталось скопировать
    qint64 m_bytesOfStatistics; //количество байт для статистики

    QThreadPool m_pool;
    QList<Result> m_completedTasks; //список выполненных задач

    QTimer* m_timerOfStatistics;

    int m_progress;

    std::atomic_bool m_canceled;
    std::mutex m_mutex;
};

#endif // ENGINE_H
