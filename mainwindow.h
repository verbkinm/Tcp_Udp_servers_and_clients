#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "connectionslist.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_triggered();

    void on_actionTcp_server_triggered();

    void on_actionTcp_client_triggered();

    void on_actionTile_triggered();

    void on_actionCascade_triggered();

    void slotConnectionList(ConnectionsList* connectionList);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
