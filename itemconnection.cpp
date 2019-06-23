#include "itemconnection.h"

ItemConnection::ItemConnection(QTcpSocket *socket, QWidget *parent) : QWidget(parent)
{
    this->socket = socket;
    layout = new QHBoxLayout;
    this->setLayout(layout);

    layout->addWidget(&label);
    layout->addWidget(&disconnect);

    label.setAlignment(Qt::AlignCenter);
    label.setText(QTime::currentTime().toString("hh:mm:ss") + " " +
                  socket->peerName().toUtf8() + " " +
                  (socket->peerAddress()).toString() + " " +
                  QString::number(socket->peerPort()) );

    disconnect.setIcon(QIcon(":/close"));
    disconnect.setFixedSize(24, 24);

    connect(&disconnect, SIGNAL(clicked()), this, SLOT(slotDisconnect()));
    connect(socket,      SIGNAL(disconnected()), this, SLOT(slotDisconnect()));
}

ItemConnection::~ItemConnection()
{
    delete layout;
}

void ItemConnection::slotDisconnect()
{
//    emit signalClosed(socket);
//    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->close();
}
