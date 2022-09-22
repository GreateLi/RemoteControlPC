/******
 * WakeOnLAN
 * 注意:需要设置广播地址，例如：192.168.1.255
 ******/

#include <QHostAddress>
#include <QUdpSocket>
#include <QProcess>

#include "TRemoteCtrlPC.h"


TRemoteCtrlPC::TRemoteCtrlPC(QObject *parent)
{
    Q_UNUSED(parent)
}

/* Note:
 * 被控计算机需要支持网络唤醒功能(Wake on LAN)
 * 1.组织魔法包(MgicPacket) 由6字节0xFF和重复16次的MAC地址  102字节
 * 2.通过UDP的方式广播 IP地址为路由的广播地址 如：网段为192.168.1.x,IP设为192.168.1.255
 * @strMac:  格式示例 "20:af:0a:47:aa:bb"
 */
void TRemoteCtrlPC::wakeOnLAN(QString strMac)
{
    /*将字符串MAC转换成十六进制 存入数组*/
    char cstrMacAddr[6];
    bool isOK;
    for(int j = 0;j < 6; j++){
        if(j < 5){
            cstrMacAddr[j] = strMac.mid(strMac.indexOf(":",j*3) - 2,2).toInt(&isOK,16);
            if(false == isOK)
                return ;
        }else if(j == 5){
            cstrMacAddr[j] = strMac.mid(strMac.indexOf(":",j*3 - 1) + 1,2).toInt(&isOK,16);
            if(false == isOK)
                return ;
        }
    }

//    /*构建一个魔术唤醒包  6字节0xFF 和  16次的 Mac地址*/
    QByteArray magicPacket;

    /*6个 0xFF*/
    for(int i = 0;i < 6; i++){
        magicPacket[i] = 0xFF;
    }

    /*16个 MAC*/
    for(int k = 0; k < 16; k++)
    {
        for(int l = 0; l < 6; l++)
            magicPacket[(k+1)*6 + l] = cstrMacAddr[l];
    }

    QHostAddress FakeAddress;
    FakeAddress.setAddress ("192.168.0.255");

    QUdpSocket udpSocket;
    udpSocket.writeDatagram(magicPacket, 102, FakeAddress, 9);

    /*将字符串MAC转换成十六进制 存入数组*/


//      qDebug()<<"strMac "<<strMac;
//      char cstrMacAddr[6];
//      bool isOK;
//      for(int j = 0;j < 6; j++){
//          if(j < 5){
//              cstrMacAddr[j] = strMac.mid(strMac.indexOf(":",j*3) - 2,2).toInt(&isOK,16);
//              if(false == isOK)
//                  return ;
//          }else if(j == 5){
//              cstrMacAddr[j] = strMac.mid(strMac.indexOf(":",j*3 - 1) + 1,2).toInt(&isOK,16);
//              if(false == isOK)
//                  return ;
//          }
//      }
//      /*构建一个魔术唤醒包  6字节0xFF 和  16次的 Mac地址*/
//      QByteArray magicPacket;

//      /*6个 0xFF*/
//      for(int i = 0;i < 6; i++){
//          magicPacket[i] = 0xFF;
//      }

//      /*16个 MAC*/
//      for(int k = 0; k < 16; k++)
//      {
//          for(int l = 0; l < 6; l++)
//              magicPacket[(k+1)*6 + l] = cstrMacAddr[l];
//      }

//      QHostAddress FakeAddress;
//      FakeAddress.setAddress ("192.168.0.255");

//      QUdpSocket udpSocket;

//       int ggg = udpSocket.writeDatagram(magicPacket, 102, FakeAddress, 50209);
//       qDebug()<<"发送结束  "<<ggg;
//      int length = udpSocket.writeDatagram(magicPacket, magicPacket.length(), FakeAddress, 0);

//      qDebug()<<"发送结束 "<<length;


   //UDP初始化
//    QByteArray sMacAddr("C8-5A-CF-0C-CC-68");
//    char chMacAddr[9];//应该是6个字符，但是如果写6个字符，下面sscanf_s会报错，所以写大点
//    char chMagicPacket[104];
//    const char *lpMacAddr=sMacAddr.data();

//    sscanf_s( lpMacAddr,"%2x-%2x-%2x-%2x-%2x-%2x",&chMacAddr[0],&chMacAddr[1],&chMacAddr[2],&chMacAddr[3],&chMacAddr[4],&chMacAddr[5] );

//    memset(chMagicPacket,0xff,6);
//    int packetsize=6;
//    for(int i=0;i<16;i++)
//    {
//        memcpy(chMagicPacket+packetsize,chMacAddr,6);
//        packetsize+=6;
//    }
//    QUdpSocket udpSocket;
//   qint64 length =  udpSocket.writeDatagram(chMagicPacket, 102, QHostAddress("255.255.255.255"), 50209);

//    qDebug()<<"length "<<length;
}

/* https://blog.csdn.net/smstong/article/details/16879347 */
/* https://www.cnblogs.com/shenjieblog/p/5455691.html
* Note:实现远程关机原理
* 1.用户管理：被控计算机启用来宾账户(Guest)
* 2.运行gpedit.msc  允许Guest强制关机  拒绝从网络访问计算机里删除  Guest
*/

void TRemoteCtrlPC::remoteShutDown(QString strIp)
{
    m_strIp = strIp;
    /*命令的执行过程会阻塞 放入线程执行*/
    this->start();
}

void TRemoteCtrlPC::run()
{
    QProcess cmd;
    QString strResult;
    QByteArray arrayOut;
    QString strCmd = "shutdown -s -t 5 -m \\\\"; //四个斜杠  转义
    strCmd += m_strIp;


    qDebug()<<strCmd;

    cmd.start(strCmd);
    cmd.waitForFinished();       // !!!阻塞操作 放在主线程会导致窗口卡死  直至执行完成

    arrayOut = cmd.readAllStandardOutput();
    strResult = QString::fromLocal8Bit(arrayOut);

    qDebug()<<"signalCmdFinished "<<strResult;
    emit signalCmdFinished(strResult); //命令执行完成   返回命令执行结果
}

