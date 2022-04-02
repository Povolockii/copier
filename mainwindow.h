#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "engine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButton_select_clicked();

    void on_pushButton_run_clicked();

    void on_pushButton_cancel_clicked();

    void _on_progress(int);

    void _on_speedBytesSec(int);

    void _on_finished(QList<Engine::Result>);

signals:
    void run(QString, QString);
    void cancelRun();

private:
    void reset();

private:
    Ui::MainWindow *ui;

    Engine* m_engine;
    QThread m_engineThread;
};
#endif // MAINWINDOW_H
