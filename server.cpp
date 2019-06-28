#include "server.h"
#include "ui_server.h"

#include <QNetworkInterface>
#include <QHostAddress>
#include <QScrollBar>

#include <QDebug>

Server::Server(server_type type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    QNetworkInterface ifaces;
    for(auto i : ifaces.allAddresses())
        ui->listenAddr->addItem(i.toString());

    if(type == TCP)
    {
        m_pTcpServer = new QTcpServer;
        connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    }

    connect(ui->client_list, SIGNAL(clicked()), this, SLOT(slotConnectionList()));

//    connectionList = new ConnectionsList;
}

Server::~Server()
{
//    connectionList->close();

    delete ui;

    for(auto socket : socketList)
        socket->close();
    m_pTcpServer->close();

    delete m_pTcpServer;

    if(connectionList != nullptr){
        connectionList->close();
        delete connectionList;
    }
}

void Server::on_actionStatusChange_triggered()
{
    if(!m_pTcpServer->isListening()){
        if(m_pTcpServer->listen(QHostAddress(ui->listenAddr->currentText()), quint16(ui->listenPort->value()))){
            ui->listenPort->setValue(m_pTcpServer->serverPort());
            ui->listenAddr->setDisabled(true);
            ui->status_line_edit->setText("starting");
            ui->status_Change->setIcon(QIcon(":/stop"));
        }
    }
    else {
        for(auto socket : socketList)
            socket->close();
        m_pTcpServer->close();

        ui->status_line_edit->setText("stoped");
        ui->count_client->setValue(0);
        ui->status_Change->setIcon(QIcon(":/play"));
        ui->listenAddr->setDisabled(false);
    }
}

void Server::slotNewConnection()
{
    QTcpSocket* socket = m_pTcpServer->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadData()));

    ui->count_client->setValue(ui->count_client->value() + 1);
    ui->data_textEdit_receive->append("Connected to " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));

    socketList.append(socket);
    updateConnectedList();    
}

void Server::slotSocketDisconnect()
{

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    ui->count_client->setValue(ui->count_client->value() - 1);
    ui->data_textEdit_receive->append("Host " + socket->peerAddress().toString() + QString::number(socket->peerPort())
                              + " disconnected:");

    socketList.removeOne(socket);
    updateConnectedList();

    socket->close();
}

void Server::slotReadData()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    ui->data_textEdit_receive->append("Host " +
                                      socket->peerAddress().toString() +
                                      ":" +
                                      QString::number(socket->peerPort()) +
                                      " send:");

    if(ui->encodingRX->currentIndex() == 0)
        ui->data_textEdit_receive->append(QString(socket->readAll()).toLocal8Bit());
    else if(ui->encodingRX->currentIndex() == 1)
        ui->data_textEdit_receive->append(QString(socket->readAll()).toLatin1());
    else if(ui->encodingRX->currentIndex() == 2)
        ui->data_textEdit_receive->append(QString(socket->readAll()).toUtf8());
//    else if(ui->encodingRX->currentIndex() == 3)
//        hexDump(m_socket);

    QScrollBar *sb = ui->data_textEdit_receive->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void Server::slotConnectionList()
{
    connectionList = new ConnectionsList;

    connectionList->reconstruction(socketList);
    emit signalConnectionList(connectionList);

    ui->client_list->setDisabled(true);

    connect(connectionList, SIGNAL(signalClose()), this, SLOT(slotConnectionListClose()));
}

void Server::slotConnectionListClose()
{
    ui->client_list->setEnabled(true);
    delete connectionList;
    connectionList = nullptr;
}

void Server::hexDump(QTcpSocket *socket)
{
    QByteArray answer = socket->readAll().toHex();
//    for(int i = 0; i < answer.length(); i++){
//        ui->data_textEdit->insertPlainText(QString(answer.at(i)));
//        if(i % 2 == 0)
//            ui->data_textEdit->insertPlainText(" ");
//        if(i % 16 == 0)
//            ui->data_textEdit->insertPlainText(" ");
//        if(i % 32 == 0)
//            ui->data_textEdit->insertPlainText("\n");
//    }
    ui->data_textEdit_receive->append(answer.toHex());
}

void Server::on_actionSendData_triggered()
{
    if(ui->comboBox_clients->count()){
        QTcpSocket* socket = socketList.at(ui->comboBox_clients->currentIndex());

        QString endl = "";
        if(ui->endLine->currentIndex() == 0)
            endl = "\r\n";
        else if(ui->endLine->currentIndex() == 1)
            endl = "\n";

        if(ui->encodingTX->currentIndex() == 0)
            socket->write((ui->data_textEdit_send->toPlainText() + endl).toLocal8Bit());
        else if(ui->encodingTX->currentIndex() == 1)
            socket->write((ui->data_textEdit_send->toPlainText() + endl).toLatin1());
        else if(ui->encodingTX->currentIndex() == 2   )
            socket->write((ui->data_textEdit_send->toPlainText() + endl).toUtf8());
    }
}

void Server::updateConnectedList()
{
    ui->comboBox_clients->clear();
    for(auto socket : socketList)
        ui->comboBox_clients->addItem(socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));

    if(connectionList != nullptr)
        connectionList->reconstruction(socketList);
}
