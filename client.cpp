#include "client.h"
#include "ui_client.h"
#include <QDebug>

#include <QScrollBar>


Client::Client(client_type type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    if(type == TCP)
        m_socket = new QTcpSocket;

//    m_socketType = type;

    connect(m_socket, SIGNAL(connected()), this, SLOT(slotNewConnection()));
}

Client::~Client()
{
    delete ui;

    m_socket->close();
    delete m_socket;
}

void Client::hexDump(QAbstractSocket *socket)
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

void Client::slotNewConnection()
{
    ui->data_textEdit_receive->append("Connected to " + m_socket->peerAddress().toString() + ":" + QString::number(m_socket->peerPort()) );

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnect()));
}

void Client::slotSocketDisconnect()
{
    ui->data_textEdit_receive->append("Disconnected " + m_socket->peerAddress().toString() + ":" + QString::number(m_socket->peerPort()) );
    disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
    disconnect(m_socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnect()));
}

void Client::slotReadData()
{
    ui->data_textEdit_receive->append("Host " +
                                      m_socket->peerAddress().toString() +
                                      ":" +
                                      QString::number(m_socket->peerPort()) +
                                      " send:");

    if(ui->radioButton_ascii_receive->isChecked())
        ui->data_textEdit_receive->append(m_socket->readAll());
    else if(ui->radioButton_hex_receive->isChecked())
        hexDump(m_socket);

    QScrollBar *sb = ui->data_textEdit_receive->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void Client::on_actionStatusChange_triggered()
{
    if(m_socket->state() == QAbstractSocket::UnconnectedState){
        m_socket->connectToHost(ui->ipaddr_line_edit->text(), quint16(ui->port->value()), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
        ui->status_Change->setIcon(QIcon(":/stop"));
        ui->status_line_edit->setText("Connected");
    }
    else
    {
        m_socket->close();
        ui->status_Change->setIcon(QIcon(":/play"));
        ui->status_line_edit->setText("Disconnected");
    }
}

void Client::on_actionSendData_triggered()
{
    if(ui->radioButton_ascii_send->isChecked())
        m_socket->write(ui->data_textEdit_send->toPlainText().toLocal8Bit());
    else if(ui->radioButton_latin1_send->isChecked())
        m_socket->write(ui->data_textEdit_send->toPlainText().toLatin1());
    else if(ui->radioButton_UTF8_send->isChecked())
        m_socket->write(ui->data_textEdit_send->toPlainText().toUtf8());
}
