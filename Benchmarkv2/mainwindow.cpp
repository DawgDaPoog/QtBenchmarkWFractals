#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>
#include <Windows.h>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    testProcess = new QProcess(this);
    program = "E:\\cpp-git\\Big Projects\\Benchmarking using Fractals\\Fractal Debugging\\x64\\Debug\\Fractal.exe";

    initInformationProcess = new QProcess(this);
    QDir dir;
    dir.filePath(QDir::currentPath());
    QString path = dir.relativeFilePath("InitialInfoReader/Debug/InitialInfoReader.exe");

    initInformationProcess->start(path,QStringList());

    initInformationProcess->waitForFinished();

    QString initoutput(initInformationProcess->readAllStandardOutput());

    QRegExp rx("(\\t)"); //RegEx for '\t'
    QStringList query = initoutput.split(rx);

    ui->labelCPUHardware->setText(query[0]);
    ui->labelAmountOfCores->setText(query[1]);
    ui->labelAmountOfRAM->setText(query[2]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if( testProcess->state() == QProcess::NotRunning )
    {
        testProcess->start(program, QStringList());
        testProcess->waitForFinished();
        output = QString(testProcess->readAllStandardOutput());
        ui->pushButton->setDisabled(true);
    }
   // qDebug() << output ;
}


void MainWindow::on_pushButton_2_clicked()
{
    if ( testProcess->state() == QProcess::Running ) {
        testProcess->kill();
        ui->pushButton->setDisabled(false);
        qDebug() << output ;
    }

   // system("taskkill /im Fractal.exe /f");
}
