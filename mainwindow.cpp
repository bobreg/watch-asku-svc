#include "mainwindow.h"
#include "ui_mainwindow.h"

//------реализация окна для важных сообщений------
bool msg_win(QString title, QString mess, int count_button, QMessageBox::Icon type_win){
    QMessageBox *msb = new QMessageBox();
    msb->setWindowTitle(title);
    msb->setText(mess);
    msb->setIcon(type_win);
    if(count_button == 1){
        msb->addButton(QMessageBox::Yes);
    }else if (count_button == 2) {
        msb->addButton(QMessageBox::Yes);
        msb->addButton(QMessageBox::Cancel);
    }

    int answer = msb->exec();
    delete msb;
    return answer == QMessageBox::Yes ? true : false;
}

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
    bool temp_answer = msg_win(trUtf8("Уверены..."),
                                      trUtf8("Закрытие приложения приведёт к "
                                             "прекращению слежения за важной "
                                             "частью программы АСКУ!"),
                                      2,
                                      QMessageBox::Warning);
    if(temp_answer == true){
        temp_answer = false;
        temp_answer = msg_win(trUtf8("Внимание!"),
                                       trUtf8("Когда-нибудь всё сломается, но вы не поймёте "
                                              "почему! И не вините потом АСКУ"),
                                       2,
                                       QMessageBox::Critical);
        if(temp_answer == true){
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
    flag_file = false;
    count_copy_run = 0;
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
    check_copy->start("ps -e");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::find_process_asku_svc(){
    if(flag_file == true and count_copy_run < 2){
        process_ps.start("ps", QStringList() << "-e");
    }else{
        timer.stop();
        if(flag_file == false){
            bool temp = msg_win(trUtf8("Ошибка"),
                                trUtf8("В системе не обнаружены файлы "
                                       "\"/etc/init.d/asku-svc\" и "
                                       "\"/opt/amcs-observer/asku-svc\"."
                                       "Программа не может быть запущена!"),
                                1,
                                QMessageBox::Critical);
            if(temp == true){
                qApp->quit();
            }
        }else if(count_copy_run > 1){
            bool temp = msg_win(trUtf8("Ииии..."),
                                trUtf8("Одна копия программы слежения за asku-svc "
                                       "уже запущена!"),
                                1,
                                QMessageBox::Critical);

            if(temp == true){
                qApp->quit();
            }
        }
    }
}

void MainWindow::ps(){  // основная логика (работает после запуска программы "ps")
    counter++;
    //qDebug() << counter;
    QString temp;
    QString msge = "";
    msge.append(QDateTime::currentDateTime().toString());
    msge.append(": ");
    QString asku_svc_process = "not found";

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
        msge.append("Process not found.\nTry restart...");
        restart_asku_svc.start("sudo service asku-svc start");
        tray.tray.showMessage(trUtf8("Во блин!"), trUtf8("asku-svc отвалилась. Перезапускаю"),
                         QSystemTrayIcon::Warning, 3000);
        ui->plainTextEdit->appendPlainText(msge);
        counter_attempt++;
        if(counter_attempt == MAX_ATTEMPTS){
            timer.stop();
            bool temp = msg_win(trUtf8("Ошибка!"),
                                    trUtf8("Что-то не так! Превышено максимальное "
                                           "число попыток запуска. Проверьте "
                                           "программу АСКУ или обратитесь к "
                                           "кому-нибудь. Программа остановлена"),
                                    1,
                                    QMessageBox::Critical);
            if(temp == true){
                qApp->quit();
            }
        }
    }else{
        counter_attempt = 0;
        if(counter % 3600 == 1){
            msge.append("asku-svc is working.");
            tray.tray.showMessage(trUtf8("Всё норм."), trUtf8("asku-svc работает"),
                             QSystemTrayIcon::Information, 3000);
            ui->plainTextEdit->appendPlainText(msge);
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
    QString asku_svc_process = "";
    temp = check_copy->readAllStandardOutput();
    list_process = temp.split("\n");
    for(int i = 0; i != list_process.length() - 1; i++){
        //qDebug() << list_process[i].indexOf("asku-svc");
        if(list_process[i].indexOf("check_asku_svc") != -1){
            count_copy_run++;
        }
    }
}
