#ifndef ACCUEIL_H
#define ACCUEIL_H

#include <QMainWindow>
#include <QJsonObject>
#include <QString>
#include <QTime>
#include <QElapsedTimer>
#include "mainwindow.h"

namespace Ui {
class Accueil;
}

class Accueil : public QMainWindow
{
    Q_OBJECT

public:
    explicit Accueil(QWidget *parent = nullptr);
    ~Accueil();

private slots:
    void originalWindow();
    void on_cbConnect_activated(int index);
    void jsonReceived(QJsonObject json);
    void notConnected();

private:
    void connectButtons();
    void connectTimers();
    void connected();
    Ui::Accueil *ui;
    MainWindow* mw;

    QTime startTime;
    QTime elapseTime;
    QElapsedTimer elapseCounter;
    QTimer* lostConnectionTimer;
    QFont timerFont;

    bool run = false;


signals:
    void testSend(QString msg);

};

#endif // ACCUEIL_H
