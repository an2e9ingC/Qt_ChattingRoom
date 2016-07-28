#include "server.h"
#include "ui_server.h"
#include <QString>
#include <QtGlobal>
#include <QTextCursor>
#include <QMessageBox>
#include <QTextStream>
#include <QByteArray>

const quint16 port  = 8888;

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    initWindow();
    initServer();

    connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConn()));
    connect(server,SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this, SLOT(slotErr(QAbstractSocket::SocketError)));

    connect(ui->sendBtn,SIGNAL(clicked()), this, SLOT(slotSend()));
    connect(ui->msgLineEdit,SIGNAL(returnPressed()), this, SLOT(slotSend()));
}

Server::~Server()
{
    server->close();
    delete server;
    delete ui;
}

void Server::initWindow()
{
    setWindowTitle(tr("Server waiting for connection..."));
    ui->recordTextEdit->setReadOnly(true);
    ui->recordTextEdit->setText(tr("Waiting for connection..."));
    ui->sendBtn->setEnabled(false);
}

void Server::initServer()
{
    setConnFlag(false);
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, port);
}

void Server::setConnFlag(bool flg)
{
    connFlag = flg;
}

void Server:: setConnected()
{
    if(!(QAbstractSocket::ConnectedState == socket->state()))
        return;

    qDebug() << __func__<< ": socket state = " <<socket->state();
    setConnFlag(true);
    setWindowTitle(tr("Conneted."));
    ui->recordTextEdit->setText(tr("-----------------Connected---------------\n"));
    ui->recordTextEdit->moveCursor(QTextCursor::End);
    ui->recordTextEdit->append("");
    printCurTime();

    showPeerInfo();
    ui->sendBtn->setEnabled(true);
    //after connected, check the socket signal of readyRead();
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotRecv()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotDisConn()));
}

void Server::showPeerInfo()
{
    QString ip = server->serverAddress().toString();
    QString port = QString::number(server->serverPort(),10);
    ui->recordTextEdit->moveCursor(QTextCursor::End);
    ui->recordTextEdit->append("");
    ui->recordTextEdit->insertPlainText("Server Current Address: <"+ ip
                                        + ":" + port+ ">\n");
}

void Server::printCurTime()
{
    ui->recordTextEdit->insertPlainText("   ( "+
                                        QTime::currentTime().toString()
                                        + " ) ");
}


void Server::slotSend()
{
    if((!connFlag) || (!socket->isValid())){
        QMessageBox msgBox;
        msgBox.setText("Connection is Invalid.");
        msgBox.exec();
        return;
    }
    QString msg(ui->msgLineEdit->text());
    QByteArray arr =  msg.toLatin1();
    char *data = arr.data();
    socket->write(data);


    ui->recordTextEdit->append(tr("Server: "));
    printCurTime();
    ui->recordTextEdit->append(msg);

    ui->msgLineEdit->clear();
}

void Server::slotNewConn()
{
    if(server->hasPendingConnections()){
        ui->recordTextEdit->setText(tr("A new connection request incoming...\n"
                                       "Please wait to build the connection...\n"));
        QTextStream str("Server current <Ip:Port> : <");
        str <<  server->serverAddress().toString()<<":"
             << server->serverPort()<< ">\n";

        ui->recordTextEdit->setText(str.readAll());

        socket  = server->nextPendingConnection();
        setConnected();
    }
    else{
        ui->recordTextEdit->setText(QString("New connection failed to establishe.\n"
                                            "errorString:" + server->errorString()));
        setConnFlag(false);
    }
}

void Server::slotRecv()
{
    //read the data from the connectedSocket,and print on the record textEdit
    QByteArray arr;
    arr = socket->readAll();


    QTextStream str(&arr,QIODevice::ReadWrite);
    str<<arr;
    ui->recordTextEdit->append(tr("Client: "));
    printCurTime();
    ui->recordTextEdit->append(str.readAll());
}

void Server::slotDisConn()
{
    //when connection is closed, reset the windown title
    setConnFlag(false);
    socket->close();
    setWindowTitle("Disconnected.");
    ui->recordTextEdit->append(tr("-------The socket is not connected!-----\n"));
    ui->recordTextEdit->insertPlainText(tr("Error:  ") +socket->errorString());
    ui->recordTextEdit->append(tr("Waiting for connection...\n"));
}

void Server::slotErr(QAbstractSocket::SocketError err)
{
    if(QAbstractSocket::RemoteHostClosedError == err){
        setWindowTitle(tr("Server waiting for connection..."));
        ui->recordTextEdit->append(tr("Disconnected: "));
        ui->recordTextEdit->append(server->errorString());
        ui->recordTextEdit->append("\n");
        slotDisConn();
    }else{
        QMessageBox msgBox;
        msgBox.setText(server->errorString());
        msgBox.exec();
        slotDisConn();
    }
}

