#include "server.h"
#include "ui_server.h"

#include <QHostAddress>
#include <QScrollBar>

#include <QDebug>

Server::Server(server_type type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    if(type == TCP)
        m_pTcpServer = new QTcpServer;
        connect(m_pTcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    }

Server::~Server()
{
    delete ui;

    for(auto socket : socketList)
        socket->close();
    m_pTcpServer->close();

    delete m_pTcpServer;
}

void Server::on_actionStatusChange_triggered()
{
    if(!m_pTcpServer->isListening()){
        if(m_pTcpServer->listen(QHostAddress(ui->ip_label->text()), quint16(ui->port->value()))){
            ui->port->setValue(m_pTcpServer->serverPort());
            ui->ipaddr_line_edit->setText(m_pTcpServer->serverAddress().toString());
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

    if(ui->radioButton_ascii_receive->isChecked())
        ui->data_textEdit_receive->append(QString(socket->readAll()).toLocal8Bit());
    else if(ui->radioButton_latin1_receive->isChecked())
        ui->data_textEdit_receive->append(QString(socket->readAll()).toLatin1());
    else if(ui->radioButton_UTF8_receive->isChecked())
        ui->data_textEdit_receive->append(QString(socket->readAll()).toUtf8());
    else if(ui->radioButton_hex_receive->isChecked())
        hexDump(socket);

    QScrollBar *sb = ui->data_textEdit_receive->verticalScrollBar();
    sb->setValue(sb->maximum());
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
    QTcpSocket* socket = socketList.at(ui->comboBox_clients->currentIndex());

    if(ui->radioButton_ascii_send->isChecked())
        socket->write(ui->data_textEdit_send->toPlainText().toLocal8Bit());
    else if(ui->radioButton_latin1_send->isChecked())
        socket->write(ui->data_textEdit_send->toPlainText().toLatin1());
    else if(ui->radioButton_UTF8_send->isChecked())
        socket->write(ui->data_textEdit_send->toPlainText().toUtf8());
}

void Server::updateConnectedList()
{
    ui->comboBox_clients->clear();
    for(auto socket : socketList)
        ui->comboBox_clients->addItem(socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));
}
