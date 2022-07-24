#ifndef ACCUEIL_H
#define ACCUEIL_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonDocument>
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
    void envoyerMsg(QString msg, bool etat);
    void actionStart();
    void actionStop();

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
    double sumPower = 0;
    long prvTime = 0;

signals:
    void testSend(QString msg);

};

#endif // ACCUEIL_H
