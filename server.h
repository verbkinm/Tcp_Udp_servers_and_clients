#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include "connectionslist.h"

//#include <QUdp


namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    enum server_type {TCP, UDP};

    explicit Server(server_type type, QWidget *parent = nullptr);
    ~Server();

//    QTcpServer*     m_pTcpServer    =   nullptr;

private slots:
    void on_actionStatusChange_triggered();

    void    slotNewConnection();
    void    slotSocketDisconnect();
    void    slotReadData();
    void    slotConnectionList();
    void    slotConnectionListClose();

    void    on_actionSendData_triggered();

signals:
    void    signalConnectionList(ConnectionsList*);

private:
    Ui::Server *ui;

    QTcpServer*     m_pTcpServer    =   nullptr;
    ConnectionsList* connectionList =   nullptr;

    QList<QTcpSocket*> socketList;

    void    updateConnectedList();
    void    hexDump(QTcpSocket* socket);
};

#endif // SERVER_H
