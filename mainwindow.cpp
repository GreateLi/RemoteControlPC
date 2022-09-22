#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QProcess>
#include <QDir>


#include <QSettings>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_2->setVisible(false);
    ui->pushButton->setVisible(false);

    m_pRemoteCtrlPC = new TRemoteCtrlPC(this);

    mUDPSocket = new QUdpSocket(this);
    //绑定
    mUDPSocket->bind(10001);
  //  setWindowTitle("服务端端口为: 9999");

    connect(mUDPSocket, &QUdpSocket::readyRead, this, &MainWindow::slotDealMsg);

    QString path = QDir::currentPath()+"/ControlConfig.ini";

     qDebug() <<"path "<< path;

    QSettings *configIniRead = new QSettings(path, QSettings::IniFormat);
    configIniRead->setIniCodec("utf-8");
    procePath = configIniRead->value("first").toString();

    appName  = configIniRead->value("appName").toString();

    qDebug() <<"appName "<<  appName;
    qDebug() <<"procePath "<< procePath;

    delete configIniRead;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotDealMsg()
{
    while(mUDPSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;
        datagram.resize(mUDPSocket->pendingDatagramSize());
        quint64 size=mUDPSocket->readDatagram(datagram.data(),datagram.size(),&sender,&senderPort);

        if(datagram.contains("Shutdown"))
        {
             on_pushButton_3_clicked();
        }
        if(datagram.contains("StartSoftwareCmd"))
        {
             on_pushButton_4_clicked();
        }
        if(datagram.contains("EndSoftwareCmd"))
        {
             on_killApp_clicked();
        }

    }
}

void MainWindow::on_pushButton_clicked()
{
    qDebug("wakeOnLAN ");
    QString strIP="192.168.124.100";
   // QString strMac="00:1B:21:E5:DA:A3";
    QString strMac="C8:5A:CF:0C:CC:68";
    m_pRemoteCtrlPC->wakeOnLAN(strMac);

    /*将字符串MAC转换成十六进制 存入数组*/
    char cstrMacAddr[6];
    bool isOK;
    for(int j = 0;j < 6; j++)
    {
        if(j < 5){
            cstrMacAddr[j] = strMac.mid(strMac.indexOf(":",j*3) - 2,2).toInt(&isOK,16);
            if(false == isOK)
            {
               qDebug()<<"false == isOK====111 " ;
               return ;
            }

        }else if(j == 5){
            cstrMacAddr[j] = strMac.mid(strMac.indexOf(":",j*3 - 1) + 1,2).toInt(&isOK,16);
            if(false == isOK)
            {
                qDebug()<<"false == isOK==== " ;
                return ;
            }
        }
    }

    /*构建一个魔术唤醒包  6字节0xFF 和  16次的 Mac地址*/
    QByteArray magicPacket;

    /*6个 0xFF*/
    for(int i = 0;i < 6; i++)
    {
        magicPacket[i] = 0xFF;
    }

    /*16个 MAC*/
    for(int k = 0; k < 16; k++)
    {
        for(int l = 0; l < 6; l++)
            magicPacket[(k+1)*6 + l] = cstrMacAddr[l];
    }

    //IP地址转换为广播地址
    QStringList qlseat;
    qlseat = strIP.split(".");
    QString qIPnew;

    qIPnew = strIP[0] + "." + strIP[1] + "." + strIP[2] + "." + "255";

    QHostAddress FakeAddress;
    FakeAddress.setAddress (qIPnew);

    QUdpSocket udpSocket;
    qint64 length = udpSocket.writeDatagram(magicPacket, 102, FakeAddress, 9);
   // qint64 length = udpSocket.writeDatagram(magicPacket, 102, QHostAddress("255.255.255.255"), 50209);

   qDebug()<<"length==== "<<length;

}


void MainWindow::on_pushButton_2_clicked()
{
    qDebug("shutdown ");
    QString strIp="192.168.124.100";
    m_pRemoteCtrlPC->remoteShutDown(strIp);
}


void MainWindow::on_pushButton_3_clicked()
{
    system("shutdown -s -t 00");
}

void MainWindow::on_pushButton_4_clicked()
{
    QProcess *pro = new QProcess;
    pro->start(procePath);
}

void MainWindow::on_killApp_clicked()
{
#ifdef Q_OS_WIN
    QProcess process;
    QString command="taskkill /im " + appName + " /f";
    process.execute(command);                 //execute执行后会一直阻塞到返回外部程序退出的代码，比如无法关闭会返回-2
    process.close();
#endif
}
