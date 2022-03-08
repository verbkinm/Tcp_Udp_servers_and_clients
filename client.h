#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

#include <sstream>

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    enum class client_type {TCP, UDP};
    explicit Client(client_type type, QWidget *parent = nullptr);
    ~Client();

    QAbstractSocket* m_socket = nullptr;

private:
    Ui::Client *ui;

//    int m_socketType;

    void    hexDump(QByteArray array);

private slots:
    void    slotNewConnection();
    void    slotSocketDisconnect();

    void    slotReadData();
    void    slotError(QAbstractSocket::SocketError error);

    void on_actionStatusChange_triggered();
    void on_actionSendData_triggered();


};

#endif // CLIENT_H
