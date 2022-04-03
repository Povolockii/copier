#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

namespace {
const QString label_path = "%1 path: %2";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_engine = new Engine();
    m_engine->moveToThread(&m_engineThread);

    connect(this, &MainWindow::run, m_engine, &Engine::_on_run);
    connect(this, &MainWindow::cancelRun, m_engine, &Engine::_on_cancel);
    connect(m_engine, &Engine::progress, this, &MainWindow::_on_progress);
    connect(m_engine, &Engine::speedBytesSec, this, &MainWindow::_on_speedBytesSec);
    connect(m_engine, &Engine::finished, this, &MainWindow::_on_finished);
    connect(&m_engineThread, &QThread::finished, m_engine, &QObject::deleteLater);

    m_engineThread.start();

    reset();
    qInfo() << "start app";
}

MainWindow::~MainWindow()
{
    m_engineThread.quit();
    m_engineThread.wait();

    qInfo() << "stop app";

    delete ui;
}

void MainWindow::on_pushButton_select_clicked()
{
    const auto errMessage = [](const QString& path) {
        QMessageBox msgBox;
        msgBox.setText(tr("%1 is epmty, please correct %1").arg(path));
        msgBox.exec();
    };

    QString source = ui->radioButton_dir->isChecked() ? QFileDialog::getExistingDirectory(this, tr("Source dir")) :
                                                        QFileDialog::getOpenFileName(this, tr("Source file"));
    if (source.isEmpty()) {
        errMessage("source");
        return;
    }

    QString target = QFileDialog::getExistingDirectory(this, tr("target dir"));
    if (target.isEmpty()) {
        errMessage("target");
        return;
    }

    ui->label_source->setProperty("path", source);
    ui->label_target->setProperty("path", target);

    ui->label_source->setText(label_path.arg("source").arg(source));
    ui->label_target->setText(label_path.arg("target").arg(target));

    ui->radioButton_dir->setEnabled(false);
    ui->radioButton_file->setEnabled(false);

    ui->pushButton_run->setEnabled(true);

    ui->pushButton_select->setEnabled(false);

    qInfo() << "select source " << source << "      target" << target;
}

void MainWindow::on_pushButton_run_clicked()
{
    qInfo() << "run start";

    ui->pushButton_run->setEnabled(false);
    ui->pushButton_cancel->setEnabled(true);

    QString source = ui->label_source->property("path").toString();
    QString target = ui->label_target->property("path").toString();
    emit run(source, target);
}

void MainWindow::on_pushButton_cancel_clicked()
{
    qInfo() << "run cancel";

    ui->pushButton_run->setEnabled(true);
    ui->pushButton_cancel->setEnabled(false);

    emit cancelRun();
}

void MainWindow::_on_finished(bool canceled, const QList<Engine::Result> &results)
{
    qInfo() << "run end";

    QString outText;
    if (!canceled) {
        QStringList failed;
        for (const auto& res : results) {
            if (!res.success) {
                failed.push_back(res.target);
            }
        }

        if (failed.empty()) {
            outText = "All files copied successfully!";
        } else {
            outText = "Failed to copy files:\n";
            for (const auto& file : failed) {
                outText += file + "\n\n";
            }
        }
    } else {
        outText = "File copying canceled";
    }

    QMessageBox msgBox;
    msgBox.setText(outText);
    msgBox.exec();

    reset();
}

void MainWindow::_on_progress(int v)
{
    qDebug() << "copy progress " << v << "%";
    ui->progressBar->setValue(v);
}

void MainWindow::_on_speedBytesSec(int value)
{
    QString speedStr(QString::number(value / 1024 / 1024));
    ui->label_speed->setText(speedStr + " Mbyte/sec");
}

void MainWindow::reset()
{
    ui->label_source->setText(label_path.arg("source").arg("empty"));
    ui->label_target->setText(label_path.arg("target").arg("empty"));
    ui->label_speed->setText("");
    ui->radioButton_dir->setEnabled(true);
    ui->radioButton_file->setEnabled(true);
    ui->pushButton_select->setEnabled(true);
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_cancel->setEnabled(false);
    ui->progressBar->setValue(0);
}
