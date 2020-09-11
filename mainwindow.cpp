#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit->setStyleSheet("background-color: black;"
                                     "color: white");
    connect(&timer, SIGNAL(timeout()), this, SLOT(proccess()));
    timer.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::proccess(){
    ui->plainTextEdit->appendPlainText("re-re");
}
