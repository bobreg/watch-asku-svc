#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

namespace Ui {
class MainWindow;
}

class ParrotTray : public QObject
{
        Q_OBJECT
public:
    ParrotTray();
    ~ParrotTray();
    QSystemTrayIcon tray;
    QAction *show_w;
private:
    QMenu tray_menu;
    QAction *close_p;

public slots:
    void close_program();


};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    ParrotTray tray;

    QTimer timer;
    QProcess process_ps;
    QProcess restart_asku_svc;
    unsigned short counter = 0;

    QStringList list_process;
    QString asku_svc_process = "not found";

public slots:
    void find_process_asku_svc();
    void ps();
    void ras();
    void show_window();



};

#endif // MAINWINDOW_H
