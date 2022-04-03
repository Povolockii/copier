#ifndef TESTENGINE_H
#define TESTENGINE_H

#include "../app/engine.h"

#include <QThread>
#include <QObject>
#include <QDir>

class TestEngine : public QObject
{
    Q_OBJECT
public:
    explicit TestEngine(QObject *parent = nullptr);
    ~TestEngine() override;

    void _on_finished(bool canceled, const QList<Engine::Result>&);

private slots:
    void _on_canceled();

signals:
    void run(QString, QString);
    void cancelRun();

    void quitTest();

private:
    void init();

    void touchFiles(const QString& path, int count);
    void mkTestDir(int countNesting, int &currentNesting);
    void rmTestDir();

    Engine* m_engine;
    QThread m_engineThread;

    QString m_source;
    QString m_target;

    bool m_canceled;
};

#endif // TESTENGINE_H
