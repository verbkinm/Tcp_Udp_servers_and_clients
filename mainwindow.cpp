#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "server.h"
#include "client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_triggered()
{
    close();
}

void MainWindow::on_actionTcp_server_triggered()
{
    Server* server = new Server(Server::TCP);
    ui->mdiArea->addSubWindow(server);
    server->show();
}

void MainWindow::on_actionTcp_client_triggered()
{
    Client* client = new Client(Client::TCP);
    ui->mdiArea->addSubWindow(client);
    client->show();
}

void MainWindow::on_actionTile_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actionCascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}
