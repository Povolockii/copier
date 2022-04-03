#include "test_engine.h"

#include <QDir>
#include <QFile>

#include <gtest/gtest.h>

TestEngine::TestEngine(QObject *parent): QObject{parent},
    m_source(QDir::currentPath() + QLatin1Char('/') + "source"),
    m_target(QDir::currentPath() + QLatin1Char('/') + "result"),
    m_canceled(false)
{
    m_engine = new Engine();
    m_engine->moveToThread(&m_engineThread);

    connect(this, &TestEngine::run, m_engine, &Engine::_on_run);
    connect(this, &TestEngine::cancelRun, m_engine, &Engine::_on_cancel);
    connect(m_engine, &Engine::finished, this, &TestEngine::_on_finished);
    connect(&m_engineThread, &QThread::finished, m_engine, &QObject::deleteLater);

    m_engineThread.start();

    init();
}

TestEngine::~TestEngine()
{
    m_engineThread.quit();
    m_engineThread.wait();
}

void TestEngine::_on_finished(bool canceled, const QList<Engine::Result> & results)
{
    ASSERT_EQ(canceled, m_canceled);
    if (canceled) {
        rmTestDir();
        emit quitTest();
        return;
    }

    for (const auto& res : results) {
        EXPECT_TRUE(res.success);
    }

    const auto sizeDir = [](const QDir& dir) {
        const auto sizeFiles = [](const QDir& dir) {
            qint64 size = 0;
            QStringList files = dir.entryList(QDir::Files);
            for (const auto& file : files) {
                QFileInfo fileInfo(file);
                size += fileInfo.size();
            }
            return size;
        };

        qint64 size = sizeFiles(dir);
        QStringList allDir = dir.entryList(QDir::Dirs);
        for (const auto& currDir : allDir) {
            size += sizeFiles(currDir);
        }
        return size;
    };

    QDir sourceDir(m_source);
    QDir targetDir(m_target);
    qint64 sourceSize = sizeDir(sourceDir);
    qint64 targetSize = sizeDir(targetDir);
    ASSERT_EQ(sourceSize, targetSize);

    rmTestDir();

    init();
    QTimer::singleShot(1000, this, &TestEngine::_on_canceled);
}

void TestEngine::_on_canceled()
{
    m_canceled = true;
    emit cancelRun();
}

void TestEngine::init()
{
    int countNesting = 10;
    int currentNesting = 0;
    mkTestDir(countNesting, currentNesting);

    QDir dir;
    if (dir.mkdir(m_target)) {
        dir.rmdir(m_target);
        ASSERT_TRUE(dir.mkdir(m_target));
    }

    emit run(m_source, m_target);
}

void TestEngine::touchFiles(const QString& path, int count)
{
    while (count) {
        const auto nameFile = path + QLatin1Char('/') + QString::number(count);
        QFile file(nameFile);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly));

        QByteArray data;
        data.resize(rand()%(1024*1024*100));
        ASSERT_TRUE(file.write(data) != -1);

        file.close();

        ASSERT_TRUE(file.setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther));

        --count;
    }
}

void TestEngine::mkTestDir(int countNesting, int& currentNesting)
{
    if (countNesting  == currentNesting) {
        return;
    }

    QString path(m_source);
    for (int i=0; i<currentNesting; ++i) {
        path += QLatin1Char('/') + "source_files";
    }

    QDir dir;
    if (dir.mkdir(path)) {
        dir.rmdir(path);
        ASSERT_TRUE(dir.mkdir(path));
    }

    const auto countFiles = rand()%1000;
    touchFiles(path, countFiles);

    mkTestDir(countNesting, ++currentNesting);
}

void TestEngine::rmTestDir()
{
    QDir sourceDir(m_source);
    sourceDir.removeRecursively();

    QDir targeDir(m_target);
    targeDir.removeRecursively();
}
