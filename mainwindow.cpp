#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit->setStyleSheet("background-color: black;"
                                     "color: white");
    connect(&timer, SIGNAL(timeout()), this, SLOT(find_process_asku_svc()));
    timer.start(1000);
    connect(&process_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(ps()));
    process_ps.start("ps", QStringList() << "-e");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::find_process_asku_svc(){
    ui->plainTextEdit->appendPlainText("re-re");
}


void MainWindow::ps(){
    QString temp;
    QStringList list_process;
    QString asku_svc_process = "not found";
    temp = process_ps.readAllStandardOutput();
    list_process = temp.split("\n");
    for(unsigned int i = 0; i != list_process.length() - 1; i++){
        //qDebug() << list_process[i].indexOf("asku-svc");
        if(list_process[i].indexOf("asku-svc") != -1){
            asku_svc_process = list_process[i];
        }

    }
    qDebug() << asku_svc_process;
    if(asku_svc_process == "not found"){
        QProcess temp;
        temp.startDetached("sudo service asku-svc start");
    }
}
