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
    connect(&restart_asku_svc, SIGNAL(readyReadStandardOutput()), this, SLOT(ras()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::find_process_asku_svc(){
    process_ps.start("ps", QStringList() << "-e");
}


void MainWindow::ps(){
    counter++;
    //qDebug() << counter;
    QString temp;
    QString msg = "";
    msg.append(QDateTime::currentDateTime().toString());
    msg.append(": ");
    asku_svc_process = "not found";

    temp = process_ps.readAllStandardOutput();
    list_process = temp.split("\n");
    for(int i = 0; i != list_process.length() - 1; i++){
        //qDebug() << list_process[i].indexOf("asku-svc");
        if(list_process[i].indexOf("asku-svc") != -1){
            asku_svc_process = list_process[i];
        }
    }
    //qDebug() << asku_svc_process;
    if(asku_svc_process == "not found"){
        msg.append("Process not found.\nTry restart...");
        restart_asku_svc.start("sudo service asku-svc start");

        ui->plainTextEdit->appendPlainText(msg);
    }else{
        if(counter % 3600 == 1){
            msg.append("asku-svc is working.");
            ui->plainTextEdit->appendPlainText(msg);
        }
    }
    if(counter == 0){
        ui->plainTextEdit->clear();
    }
}

void MainWindow::ras(){
    QString temp;
    temp = restart_asku_svc.readAllStandardOutput();
    ui->plainTextEdit->appendPlainText(temp);
    counter = 0;
}
