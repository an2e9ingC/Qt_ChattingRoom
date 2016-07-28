#include "client.h"
#include "ui_client.h"
#include <QTest>
#include <QAbstractSocket>
#include <QTextCursor>
#include <QMessageBox>

Client::Client(QWidget *parent) :
    QWidget(parent),ui(new Ui::Client)
{
    ui->setupUi(this);

    initWindow();
    initSocket();

    connect(socket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotDisConn()));
    connect(socket, SIGNAL(hostFound()), this, SLOT(slotHostFound()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotRecv()));
}

Client::~Client()
{
    socket->close();
    delete socket;
    delete ui;
}


void Client::initWindow()
{
    setConnFlag(false);
    setWindowTitle(tr("Client waiting to connect..."));
    ui->recordTextEdit->clear();
    ui->recordTextEdit->setReadOnly(true);
    ui->recordTextEdit->setText(tr("Please input the server ip."));
    ui->recordTextEdit->append(tr("Then click <Connect> Button to connect."));
    ui->recordTextEdit->ensureCursorVisible();
    ui->recordTextEdit->acceptRichText();   //accept pic,html,etc.
    ui->msgLineEdit->clear();
    ui->sendBtn->setEnabled(false);

    connect(ui->connectBtn,SIGNAL(clicked()), this, SLOT(slotConnBtn()));
    connect(ui->sendBtn,SIGNAL(clicked()), this, SLOT(slotSend()));
    connect(ui->msgLineEdit,SIGNAL(returnPressed()), this, SLOT(slotSend()));
}

void Client::initSocket()
{
    // the ip could receive from the constructor
    QHostAddress servIp = QHostAddress::LocalHost;
    quint16 serPort = 8888;

    socket = new QTcpSocket(this);
    socket->connectToHost(servIp, serPort);
}

void Client::printCurTime()
{
    ui->recordTextEdit->insertPlainText("   ( "+
                                        QTime::currentTime().toString()
                                        + " ) ");
}

void Client::setConnected()
{
    setWindowTitle(tr("Conneted."));
    ui->connectBtn->hide();
    ui->recordTextEdit->setText(tr("-----------------Connected---------------\n"));
    ui->recordTextEdit->moveCursor(QTextCursor::End);
    ui->recordTextEdit->append("");
    printCurTime();

    showPeerInfo();
    ui->sendBtn->setEnabled(true);
    setConnFlag(true);
}

void Client::setConnFlag(bool flg)
{
    connFlag = flg;
}

bool Client::checkInputInfo()
{
    return true;
}

void Client::slotConnBtn()
{
    qDebug() << __func__ << ": start";
    if(!checkInputInfo()){
        // show mess box

        return;
    }

    setWindowTitle(tr("connecting..."));
    ui->recordTextEdit->clear();

    if(QAbstractSocket::ConnectingState == socket->state())
    {
        ui->recordTextEdit->append(tr("Connecting...\n"
                                      "Please Hold...\n"));
        printCurTime();
        setConnFlag(false);
    }
}

void Client::slotSend()
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

    ui->recordTextEdit->append(tr("Client: "));
    printCurTime();
    ui->recordTextEdit->append(msg);

    ui->msgLineEdit->clear();
}

void Client::slotRecv()
{
    QByteArray arr = socket->readAll();
    QTextStream str(&arr,QIODevice::ReadWrite);
    str<<arr;
    ui->recordTextEdit->append(tr("Server: "));
    printCurTime();
    ui->recordTextEdit->append(str.readAll());
}

void Client::slotNewConn()
{

}

void Client::slotConnected()
{
    setConnFlag(true);
    if(QAbstractSocket::ConnectingState == socket->state()){

        ui->recordTextEdit->append(tr("Connecting...\n"
                                      "Please Hold...\n"));
        setConnFlag(false);
    }else if(QAbstractSocket::ConnectedState == socket->state()){
        setConnected();
    }else{
        ui->recordTextEdit->append(tr("The socket is not connected!\n"));
        setConnFlag(false);
    }
}

void Client::slotDisConn()
{
    setConnFlag(false);
    setWindowTitle("Disconnected.");
    ui->recordTextEdit->append(tr("-------The socket is not connected!-----\n"));
    ui->recordTextEdit->insertPlainText(tr("Error:  ") +socket->errorString());
    printCurTime();
}

void Client::slotHostFound()
{
    printCurTime();
    ui->recordTextEdit->append(tr("Host has found already, wait to build connection\n"));
}

QString Client::inputSerIp()
{
    QString ip;
    ip = ui->msgLineEdit->text();
    return ip;
}

QString Client::inputSerPort()
{
    QString port;
    port = ui->msgLineEdit->text();
    return port;
}

void Client::showPeerInfo()
{
    QString ip = getServerAddr().toString();
    QString port = QString::number(getServerPort(),10);
    QString name = getServerName();
    ui->recordTextEdit->moveCursor(QTextCursor::End);
    ui->recordTextEdit->append("");
    ui->recordTextEdit->insertPlainText("Peer Address: <"+ ip+":" + port+ ">\n"
                                                                          "Peer Name: " + name + "\n");
}

QHostAddress Client::getServerAddr()
{
    return socket->peerAddress();
}

QString Client::getServerName()
{
    return socket->peerName();
}

quint16 Client::getServerPort()
{
    return socket->peerPort();
}
