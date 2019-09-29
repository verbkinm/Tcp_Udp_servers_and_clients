#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QScrollBar>

#include <QDebug>

#include "server.h"
#include "ui_server.h"
#include "hex_dump.h"

#define START_SERVER  ui->status_line_edit->setText("starting"); ui->status_Change->setIcon(QIcon(":/stop")); ui->listenAddr->setDisabled(true);
#define STOP_SERVER   ui->status_line_edit->setText("stoped"); ui->status_Change->setIcon(QIcon(":/play")); ui->listenAddr->setDisabled(false);

Server::Server(server_type type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    this->type = type;

    QNetworkInterface ifaces;
    for(auto i : ifaces.allAddresses())
        ui->listenAddr->addItem(i.toString());

    if(type == server_type::TCP)
    {
        m_pTcpServer = new QTcpServer(this);
        connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
        ui->lineEdit_client->hide();
    }
    else if(type == server_type::UDP){
        m_pUdpSocket = new QUdpSocket;
        m_pUdpSocket->setObjectName("UDP");
        connect(m_pUdpSocket, SIGNAL(readyRead()), this, SLOT(slotReadData()));

        ui->comboBox_clients->hide();
        ui->label_clients_count->hide();
        ui->count_client->hide();
        ui->client_list->hide();
    }

    connect(ui->client_list, SIGNAL(clicked()), this, SLOT(slotConnectionList()));

//    connectionList = new ConnectionsList;
}

Server::~Server()
{
//    connectionList->close();

    delete ui;

    if(type == server_type::TCP){
        for(auto socket : socketList)
            socket->close();

        m_pTcpServer->close();
        delete m_pTcpServer;

        if(connectionList != nullptr){
            connectionList->close();
            delete connectionList;
        }
    }
    else  if(type == server_type::UDP){
        m_pUdpSocket->close();
        delete m_pUdpSocket;
    }
}

void Server::on_actionStatusChange_triggered()
{
    if(type == server_type::TCP)
    {
        if(!m_pTcpServer->isListening()){
            if(m_pTcpServer->listen(QHostAddress(ui->listenAddr->currentText()), quint16(ui->listenPort->value()))){
                ui->listenPort->setValue(m_pTcpServer->serverPort());
                START_SERVER;
            }
        }
        else {
            for(auto socket : socketList)
                socket->close();
            m_pTcpServer->close();

            ui->count_client->setValue(0);
            STOP_SERVER;
        }
    }
    else if(type == server_type::UDP)
    {
        if(m_pUdpSocket->state() == QAbstractSocket::UnconnectedState)
        {
            m_pUdpSocket->bind(QHostAddress(ui->listenAddr->currentText()), quint16(ui->listenPort->value()));
            ui->listenPort->setValue(m_pUdpSocket->localPort());
            START_SERVER;
        }
        else {
            m_pUdpSocket->close();
            STOP_SERVER;
        }
    }
}

void Server::slotNewConnection()
{
    QTcpSocket* socket = m_pTcpServer->nextPendingConnection();
    socket->setObjectName("TCP");
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
    if(sender()->objectName() == "TCP"){
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
        else if(ui->encodingRX->currentIndex() == 3)
            hexDump(socket->readAll());
    }
    else if(sender()->objectName() == "UDP"){
        QUdpSocket* socket = qobject_cast<QUdpSocket*>(sender());
        while (socket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = socket->receiveDatagram();
            ui->data_textEdit_receive->append("Host " +
                                              datagram.senderAddress().toString() +
                                              ":" +
                                              QString::number(datagram.senderPort()) +
                                              " send:");
            if(ui->encodingRX->currentIndex() == 0)
                ui->data_textEdit_receive->append(QString(datagram.data()).toLocal8Bit());
            else if(ui->encodingRX->currentIndex() == 1)
                ui->data_textEdit_receive->append(QString(datagram.data()).toLatin1());
            else if(ui->encodingRX->currentIndex() == 2)
                ui->data_textEdit_receive->append(QString(datagram.data()).toUtf8());
            else if(ui->encodingRX->currentIndex() == 3)
                hexDump(datagram.data());
        }
    }



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

void Server::hexDump(QByteArray array) const
{
    size_t size = size_t(array.length());
    byte *byteArray = new byte[size];
    FILE* output = nullptr;
    char  fileName[] = "tmp";

    output = fopen(fileName, "w+");
    if(output == nullptr)
    {
        fprintf(stderr, "Cannot create temporary file\n %s - %s() - %d line", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    for (int i = 0; i < int(size); ++i)
        byteArray[i] = byte(array[i]);

    hex_Dump(byteArray, &size, output);

    QByteArray arrayToTextEdit;
    char ch;
    while ( (ch = char(fgetc(output))) != EOF)
        arrayToTextEdit.append(ch);

    fclose(output);
    remove(fileName);

    ui->data_textEdit_receive->append(arrayToTextEdit + "\n");

    delete[] byteArray;
}

void Server::on_actionSendData_triggered()
{
    if(ui->comboBox_clients->count())
    {
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
