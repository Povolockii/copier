#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QList>
#include <QFuture>
#include <QThreadPool>
#include <QTimer>
#include <atomic>

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
    void finished(QList<Engine::Result>);

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
    QList<Result> m_completedTasks;

    QTimer* m_timerSpeed;

    int m_progress;

    std::atomic_bool m_canceled;
};

#endif // ENGINE_H
