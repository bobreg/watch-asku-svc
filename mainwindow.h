#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
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



};

#endif // MAINWINDOW_H
