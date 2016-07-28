#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QTime>

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

    QHostAddress getServerAddr();
    quint16 getServerPort();
    QString getServerName();

protected:
    void initWindow();
    void initSocket();
    void printCurTime();
    void setConnected();
    void setConnFlag(bool);
    bool checkInputInfo();  // check the server info inputted by user

protected slots:
    void slotConnBtn();
    void slotSend();
    void slotRecv();
    void slotNewConn();
    void slotConnected();
    void slotDisConn();
    void slotHostFound();

private:
    Ui::Client *ui;
    QTcpSocket *socket;
    bool connFlag;  //show the connection status
    //get the server ip,port from the msg lineEdit inputted by user
    QString inputSerIp();
    QString inputSerPort();
    void showPeerInfo();
};

#endif // CLIENT_H
