#ifndef TREMOTECTRLPC_H
#define TREMOTECTRLPC_H

#include <QObject>
#include <QThread>

class TRemoteCtrlPC : public QThread
{
    Q_OBJECT
public:
    explicit TRemoteCtrlPC(QObject *parent = 0);

    void wakeOnLAN(QString strMac);
    void remoteShutDown(QString strIp);
protected:
    void run();
signals:
    void signalCmdFinished(QString result);
public slots:
private:
    QString m_strIp;
};

#endif // TREMOTECTRLPC_H



