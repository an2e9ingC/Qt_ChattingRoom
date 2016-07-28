#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTime>

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

protected:
    void initWindow();
    void initServer();
    void setConnFlag(bool);
    void setConnected();
    void showPeerInfo();
    void printCurTime();


protected slots:
    void slotSend();
    void slotNewConn();
    void slotRecv();
    void slotDisConn();
    void slotErr(QAbstractSocket::SocketError);

private:
    Ui::Server *ui;
    QTcpSocket *socket;
//    QTcpSocket *connectedSocket;
    QTcpServer *server;
    bool connFlag;  //show the connection status

};

#endif // SERVER_H
