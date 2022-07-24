#include "accueil.h"
#include "ui_accueil.h"
#include <QDebug>

Accueil::Accueil(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Accueil)
{
    ui->setupUi(this);
    mw = new MainWindow(1000);
    mw->portCensus(ui->cbConnect);
    ui->pbStatus->setEnabled(false);

    timerFont.setPointSize(12);
    startTime.setHMS(0,0,0);
    ui->lbTemps->setText(startTime.toString("mm:ss.zzz"));
    ui->lbTemps->setFont(timerFont);
    ui->lbTemps->setAlignment(Qt::AlignCenter);



    lostConnectionTimer = new QTimer(this);
    connect(mw,SIGNAL(jsonEmit(QJsonObject)),this,SLOT(jsonReceived(QJsonObject)));

    connectButtons();
    connectTimers();
    notConnected();
}

Accueil::~Accueil()
{
    delete ui;
}

void Accueil::originalWindow()
{
    mw->show();
}

void Accueil::on_cbConnect_activated(int index)
{
    mw->connectComboBox(ui->cbConnect);
}

void Accueil::jsonReceived(QJsonObject json)
{
    connected();
    qDebug() << json["voltage"].toDouble();

}

void Accueil::connectButtons()
{
    connect(ui->pbOriginal,&QPushButton::clicked,this,&Accueil::originalWindow);

    connect(ui->pbStart,&QPushButton::clicked,[this](){
        startTime = QTime::currentTime();
        run = true;
    });
    connect(ui->pbStop,&QPushButton::clicked,[this](){run = false;});
}

void Accueil::connectTimers()
{
    connect(lostConnectionTimer,&QTimer::timeout, this, &Accueil::notConnected);
}

void Accueil::connected()
{
    // Status Button timer
    lostConnectionTimer->stop();
    lostConnectionTimer->start(500);
    ui->pbStatus->setStyleSheet("background-color: rgb(50,164,49);");

    ui->pbOriginal->setEnabled(true);

    if(run)
    {
        QTime tempTime(0,0,0);
        elapseTime = tempTime.addMSecs(QTime::currentTime().msecsSinceStartOfDay() - startTime.msecsSinceStartOfDay());
        ui->lbTemps->setText(elapseTime.toString("mm:ss.zzz"));
        ui->lbTemps->setFont(timerFont);
        ui->lbTemps->setAlignment(Qt::AlignCenter);
    }

}

void Accueil::notConnected()
{
    lostConnectionTimer->stop();
    ui->pbStatus->setStyleSheet("background-color: rgb(204,2,2);");
    ui->pbOriginal->setEnabled(false);
}

