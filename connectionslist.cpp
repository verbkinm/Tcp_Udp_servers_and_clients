#include "connectionslist.h"

ConnectionsList::ConnectionsList(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_pLayout = new QVBoxLayout;

    this->setLayout(m_pLayout);
}

ConnectionsList::~ConnectionsList()
{
    delete m_pLayout;
}

void ConnectionsList::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit signalClose();
}

void ConnectionsList::reconstruction(QList<QTcpSocket *> &list)
{
//    qDebug() << list.length();

    for(auto &item : itemList){
        itemList.removeOne(item);
        delete item;
    }

    for(auto &socket : list){
        ItemConnection* itemConection = new ItemConnection(socket);
        itemList.append(itemConection);
//        connect(itemConection, SIGNAL(signalClosed(QTcpSocket*)), this, SLOT(slotCloseItemConnection(QTcpSocket*)));
        m_pLayout->addWidget(itemConection);
    }
}
