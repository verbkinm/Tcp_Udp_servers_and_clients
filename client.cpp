#include "client.h"
#include "ui_client.h"
#include <QDebug>
#include <QMessageBox>

#include <QScrollBar>
#include "hex_dump.h"

Client::Client(client_type type, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    if(type == client_type::TCP)
        m_socket = new QTcpSocket;

    connect(m_socket, SIGNAL(connected()), this, SLOT(slotNewConnection()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

}

Client::~Client()
{
    m_socket->close();
    delete m_socket;

    delete ui;
}

void Client::hexDump(QByteArray array)
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

void Client::slotNewConnection()
{
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnect()));

    ui->data_textEdit_receive->append("Connected to " + m_socket->peerAddress().toString() + ":" + QString::number(m_socket->peerPort()) );
    ui->status_Change->setIcon(QIcon(":/stop"));
    ui->status_line_edit->setText("Connected");
}

void Client::slotSocketDisconnect()
{
    disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadData()));
    disconnect(m_socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnect()));

    ui->data_textEdit_receive->append("Disconnected " + m_socket->peerAddress().toString() + ":" + QString::number(m_socket->peerPort()) );
    ui->status_Change->setIcon(QIcon(":/play"));
    ui->status_line_edit->setText("Disconnected");
}

void Client::slotReadData()
{
    ui->data_textEdit_receive->append("Host " +
                                      m_socket->peerAddress().toString() +
                                      ":" +
                                      QString::number(m_socket->peerPort()) +
                                      " send:");

    if(ui->encodingRX->currentIndex() == 0)
        ui->data_textEdit_receive->append(QString(m_socket->readAll()).toLocal8Bit());
    else if(ui->encodingRX->currentIndex() == 1)
        ui->data_textEdit_receive->append(QString(m_socket->readAll()).toLatin1());
    else if(ui->encodingRX->currentIndex() == 2)
        ui->data_textEdit_receive->append(QString(m_socket->readAll()).toUtf8());
    else if(ui->encodingRX->currentIndex() == 3)
        hexDump(m_socket->readAll());

    QScrollBar *sb = ui->data_textEdit_receive->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void Client::slotError(QAbstractSocket::SocketError error)
{
    QMessageBox msg(QMessageBox::Critical, "Error number", QString::number(error));
    msg.exec();
    qDebug() << error;
    QString strErr;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        strErr = "The connection was refused by the peer (or timed out).";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        strErr = "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
        break;
    case QAbstractSocket::HostNotFoundError:
        break;
    case QAbstractSocket::SocketAccessError:
        break;
    case QAbstractSocket::SocketResourceError:
        break;
    case QAbstractSocket::SocketTimeoutError:
        break;
    case QAbstractSocket::DatagramTooLargeError:
        break;
    case QAbstractSocket::NetworkError:
        break;
    case QAbstractSocket::AddressInUseError:
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        break;
    case QAbstractSocket::ProxyNotFoundError:
        break;
    case QAbstractSocket::ProxyProtocolError:
        break;
    case QAbstractSocket::OperationError:
        break;
    case QAbstractSocket::SslInternalError:
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        break;
    case QAbstractSocket::TemporaryError:
        break;
    case QAbstractSocket::UnknownSocketError:
        strErr = "UnknownSocketError";
        break;
    }
    QTextCharFormat fmt;
    fmt.setForeground(QColor(255, 0, 0));
    ui->data_textEdit_receive->mergeCurrentCharFormat(fmt);
    ui->data_textEdit_receive->append(strErr);
    fmt.setForeground(QColor(0, 0, 0));
    ui->data_textEdit_receive->mergeCurrentCharFormat(fmt);
}

void Client::on_actionStatusChange_triggered()
{
    if(m_socket->state() == QAbstractSocket::UnconnectedState){
        m_socket->connectToHost(ui->ipaddr_line_edit->text(), quint16(ui->port->value()), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    }
    else
    {
        m_socket->close();
    }
}

void Client::on_actionSendData_triggered()
{
    QString endl = "";
    if(ui->endLine->currentIndex() == 0)
        endl = "\r\n";
    else if(ui->endLine->currentIndex() == 1)
        endl = "\n";

    if(ui->encodingTX->currentIndex() == 0)
        m_socket->write((ui->data_textEdit_send->toPlainText() + endl).toLocal8Bit());
    else if(ui->encodingTX->currentIndex() == 1)
        m_socket->write((ui->data_textEdit_send->toPlainText() + endl).toLatin1());
    else if(ui->encodingTX->currentIndex() == 2   )
        m_socket->write((ui->data_textEdit_send->toPlainText() + endl).toUtf8());
    else if(ui->encodingTX->currentIndex() == 3   )
    {
        std::string str = ui->data_textEdit_send->toPlainText().toStdString();
        std::stringstream stream(str);

        int value;
        while(stream >> value)
        {
            QByteArray data;
            data.push_back(value);
            m_socket->write(data);
        }
    }
}
