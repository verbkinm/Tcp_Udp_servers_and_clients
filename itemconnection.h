#ifndef ITEMCONNECTION_H
#define ITEMCONNECTION_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>

class ItemConnection : public QWidget
{
    Q_OBJECT
public:
    explicit ItemConnection(QTcpSocket* socket, QWidget *parent = nullptr);
    ~ItemConnection();

private:
    QHBoxLayout* layout = nullptr;

    QTcpSocket* socket  = nullptr;
    QLabel label;
    QPushButton disconnect;

signals:
    void    signalClosed(QTcpSocket*);

private slots:
    void    slotDisconnect();
public slots:
};

#endif // ITEMCONNECTION_H
