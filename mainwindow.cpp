#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QDebug>

namespace {
const QString label_path = "%1 path: %2";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reset();

    qInfo() << "start";
}

MainWindow::~MainWindow()
{
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

    QString result = ui->radioButton_dir->isChecked() ? QFileDialog::getExistingDirectory(this, tr("Result dir")) :
                                                        QFileDialog::getOpenFileName(this, tr("Result file"));
    if (result.isEmpty()) {
        errMessage("result");
        return;
    }

    ui->label_source->setProperty("path", source);
    ui->label_result->setProperty("path", result);

    ui->label_source->setText(label_path.arg("source").arg(source));
    ui->label_result->setText(label_path.arg("result").arg(result));

    ui->radioButton_dir->setEnabled(false);
    ui->radioButton_file->setEnabled(false);

    ui->pushButton_run->setEnabled(true);
    ui->pushButton_cancel->setEnabled(true);

    ui->pushButton_select->setEnabled(false);

    qInfo() << "select source " << source << "      result" << result;
}

void MainWindow::on_pushButton_run_clicked()
{
    qInfo() << "run start";

    QString source = ui->label_source->property("path").toString();
    QString result = ui->label_result->property("path").toString();

    ui->progressBar->setValue(100);
    qInfo() << "run end";
}

void MainWindow::on_pushButton_cancel_clicked()
{
    reset();
}

void MainWindow::on_progressBar_valueChanged(int value)
{
    if (value == ui->progressBar->minimum() || value == ui->progressBar->maximum()) {
        reset();
    }
}

void MainWindow::reset()
{
    ui->label_source->setText(label_path.arg("source").arg("empty"));
    ui->label_result->setText(label_path.arg("result").arg("empty"));
    ui->radioButton_dir->setEnabled(true);
    ui->radioButton_file->setEnabled(true);
    ui->pushButton_select->setEnabled(true);
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_cancel->setEnabled(false);
}
