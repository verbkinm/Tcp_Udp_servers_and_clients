#ifndef CONNECTIONSLIST_H
#define CONNECTIONSLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QVBoxLayout>

#include "itemconnection.h"

class ConnectionsList : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionsList(QWidget *parent = nullptr);
    ~ConnectionsList();

    void    reconstruction(QList<QTcpSocket *> &list);

private:

    QList<QTcpSocket*> socketList;
    QList<ItemConnection*> itemList;

    QVBoxLayout* m_pLayout  = nullptr;

private slots:

signals:
    void    signalClose();

protected:
    virtual void closeEvent(QCloseEvent *event);
};

#endif // CONNECTIONSLIST_H
