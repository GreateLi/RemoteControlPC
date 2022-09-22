#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TRemoteCtrlPC.h"
#include <QHostAddress>
#include <QUdpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void  slotDealMsg();
    void on_pushButton_4_clicked();

    void on_killApp_clicked();

private:
    Ui::MainWindow *ui;

    TRemoteCtrlPC *m_pRemoteCtrlPC;
    QUdpSocket *mUDPSocket ;

    QString procePath;
    QString appName;
};
#endif // MAINWINDOW_H
