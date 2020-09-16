#include "mainwindow.h"
#include "ui_mainwindow.h"

//------реализация класса меню трея-----
ParrotTray::ParrotTray(){
    //  создаём значёк в трее
        tray.setIcon(QIcon("bird.png"));
        tray.setToolTip(trUtf8("слежу за asku-svc!"));
        tray.show();
    //  создаём меню для иконки в трее
        show_w = new QAction(trUtf8("Показать окно"), this);
        close_p = new QAction(trUtf8("Закрыть программу"), this);
        tray_menu.addAction(show_w);
        tray_menu.addAction(close_p);
        tray.setContextMenu(&tray_menu);

        connect(close_p, SIGNAL(triggered()), this, SLOT(close_program()));
}

ParrotTray::~ParrotTray(){
    delete show_w;
    delete close_p;
}

void ParrotTray::close_program(){
    //qApp->quit();
    InfMessage alarm;
}


//------реализация класса системного окна о закрытии программы------
InfMessage::InfMessage(){
    msb = new QMessageBox();
    last_warning = new QMessageBox();
    msb->setIcon(QMessageBox::Question);
    msb->setWindowTitle(trUtf8("Уверены..."));
    msb->setText(trUtf8("Закрытие приложения приведёт к "
                        "прекращению слежения за важной "
                        "частью программы АСКУ!"));
    msb->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    answer = msb->exec();
    delete msb;
    if(answer == QMessageBox::Yes){
        last_warning->setIcon(QMessageBox::Warning);
        last_warning->setWindowTitle(trUtf8("Внимание!"));
        last_warning->setText(trUtf8("Когда-нибудь всё сломается, но вы не поймёте "
                                     "почему! И не вините потом АСКУ"));
        last_warning->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        answer = last_warning->exec();
        delete last_warning;
        if(answer == QMessageBox::Yes){
            qApp->quit();
        }
    }
}


//------реализация класса основного окна и основной логики программы-----
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    counter = 0;
    asku_svc_process = "not found";
    flag_file = false;
    flag_copy = false;
    counter_attempt = 0;
// создаём форму
    ui->setupUi(this);
    ui->plainTextEdit->setStyleSheet("background-color: black;"
                                     "color: white");
//  создаём таймеры и коннекты
    connect(&timer, SIGNAL(timeout()), this, SLOT(find_process_asku_svc()));
    timer.start(1000);
    connect(&process_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(ps()));
    connect(&restart_asku_svc, SIGNAL(readyReadStandardOutput()), this, SLOT(ras()));
    connect(tray.show_w, SIGNAL(triggered()), this, SLOT(show_window()));
// проверка наличия необходимых файлов
    if(QFile::exists("/opt/amcs-observer/asku-svc") and QFile::exists("/etc/init.d/asku-svc")){
        flag_file = true;
    }
// проверка на запуск ещё одной копии
    check_copy = new QProcess();
    connect(check_copy, SIGNAL(readyReadStandardOutput()), this, SLOT(copy_is()));
    check_copy->start("ps -e | grep check_asku_svc");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::find_process_asku_svc(){
    if(flag_file == true){
        process_ps.start("ps", QStringList() << "-e");
    }else{
        timer.stop();
        QMessageBox *msg_critical = new QMessageBox(QMessageBox::Critical, trUtf8("Ошибка"),
                                                    trUtf8("В системе не обнаружены файлы "
                                                           "\"/etc/init.d/asku-svc\" и "
                                                           "\"/opt/amcs-observer/asku-svc\"."
                                                           "Программа не может быть запущена!"),
                                                    QMessageBox::Yes);
        int temp = msg_critical->exec();
        delete msg_critical;
        if(temp == QMessageBox::Yes){
            qApp->quit();
        }
    }
}

void MainWindow::ps(){  // основная логика (работает после запуска программы "ps")
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
        tray.tray.showMessage(trUtf8("Во блин!"), trUtf8("asku-svc отвалилась. Перезапускаю"),
                         QSystemTrayIcon::Warning, 3000);
        ui->plainTextEdit->appendPlainText(msg);
        counter_attempt++;
        if(counter_attempt == MAX_ATTEMPTS){
            timer.stop();
            QMessageBox *msg_critical = new QMessageBox(QMessageBox::Critical, trUtf8("Ошибка"),
                                                        trUtf8("Что-то не так! Превышено максимальное"
                                                               "число попыток запуска. Проверьте"
                                                               "программу АСКУ или обратитесь к "
                                                               "кому-нибудь. Программа остановлена"),
                                                        QMessageBox::Yes);
            int temp = msg_critical->exec();
            delete msg_critical;
            if(temp == QMessageBox::Yes){
                qApp->quit();
            }
        }
    }else{
        counter_attempt = 0;
        if(counter % 3600 == 1){
            msg.append("asku-svc is working.");
            tray.tray.showMessage(trUtf8("Всё норм."), trUtf8("asku-svc работает"),
                             QSystemTrayIcon::Information, 3000);
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

void MainWindow::show_window(){
    this->show();
}

void MainWindow::copy_is(){
    QString temp = "";
    temp = check_copy->readAllStandardOutput();
    qDebug() << temp;
    if(temp != ""){
        qDebug() << "re-re";
    }
}


