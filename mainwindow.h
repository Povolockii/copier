#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    void on_progressBar_valueChanged(int value);

private:
    void reset();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H