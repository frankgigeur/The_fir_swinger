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
    this->setFixedSize(QSize(450, 320));

    timerFont.setPointSize(12);
    startTime.setHMS(0,0,0);
    ui->lbTemps->setText(startTime.toString("mm:ss.zzz"));
    ui->lbTemps->setFont(timerFont);
    ui->lbTemps->setAlignment(Qt::AlignCenter);

    // Disable PB at startup
    ui->pbOriginal->setEnabled(false);
    ui->pbCalibrer->setEnabled(false);
    ui->pbStart->setEnabled(false);
    ui->pbStop->setEnabled(false);
    ui->pbDisconnect->setEnabled(false);


    lostConnectionTimer = new QTimer(this);
    connect(mw,SIGNAL(jsonEmit(QJsonObject)),this,SLOT(jsonReceived(QJsonObject)));
    connect(mw,&MainWindow::reOpen,this,[this](){this->show();});

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
    //qDebug() << json["position"].toDouble();

    if(run)
    {

        double power = json["current"].toDouble() * json["voltage"].toDouble();
        long actualTime = json["time"].toDouble();

        if(prvTime > 0)
            sumPower += (power * ((actualTime - prvTime)))/1000;

        prvTime = actualTime;


        ui->lbJ->setText(QString::number(sumPower));
        ui->lbWh->setText(QString::number(sumPower/3600));

    }


    if(json["state"].toString() == "CALIBRATION")
    {
        ui->pbCalibrer->setEnabled(true);
        ui->pbStart->setEnabled(false);
        ui->pbStop->setEnabled(false);
    }
    else if(json["state"].toString() == "PRISE_SAPIN")
    {
        ui->pbCalibrer->setEnabled(false);
        ui->pbStart->setEnabled(true);
        ui->pbStop->setEnabled(true);
    }
    else
    {
        ui->pbCalibrer->setEnabled(false);
        ui->pbStart->setEnabled(false);
        ui->pbStop->setEnabled(true);
    }



}

void Accueil::connectButtons()
{
    connect(ui->pbOriginal,&QPushButton::clicked,this,&Accueil::originalWindow);
    connect(ui->pbStart,&QPushButton::clicked,this,&Accueil::actionStart);
    connect(ui->pbStop,&QPushButton::clicked,this,&Accueil::actionStop);
    connect(ui->pbCalibrer,&QPushButton::clicked,this,[this](){envoyerMsg("calibrer",true);});
    connect(ui->pbDisconnect,&QPushButton::clicked,mw,&MainWindow::disconnectCom);
}

void Accueil::connectTimers()
{
    connect(lostConnectionTimer,&QTimer::timeout, this, &Accueil::notConnected);
}

void Accueil::connected()
{
    // Status Button timer
    ui->pbDisconnect->setEnabled(true);

    lostConnectionTimer->stop();
    lostConnectionTimer->start(500);
    ui->pbStatus->setStyleSheet("background-color: rgb(50,164,49);");

    ui->pbOriginal->setEnabled(true);

    if(run)
    {
        QTime tempTime(0,0,0);
        int runningTime = QTime::currentTime().msecsSinceStartOfDay() - startTime.msecsSinceStartOfDay();

        if(runningTime >= 60000)
            actionStop();

        elapseTime = tempTime.addMSecs(runningTime);
        ui->lbTemps->setText(elapseTime.toString("mm:ss.zzz"));
        ui->lbTemps->setFont(timerFont);
        ui->lbTemps->setAlignment(Qt::AlignCenter);
    }

}

void Accueil::notConnected()
{
    lostConnectionTimer->stop();
    ui->pbDisconnect->setEnabled(false);
    ui->pbStatus->setStyleSheet("background-color: rgb(204,2,2);");
}


void Accueil::envoyerMsg(QString msg, bool etat)
{
    QJsonObject jsonObject
    {
        {msg, etat}
    };
    QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << strJson;
    mw->sendMessage(strJson);
}

void Accueil::actionStart()
{
    startTime = QTime::currentTime();
    run = true;
    envoyerMsg("run",run);
}

void Accueil::actionStop()
{
    run = false;
    envoyerMsg("run",run);
    envoyerMsg("reInit",true);
    startTime.setHMS(0,0,0);
    ui->lbTemps->setText(startTime.toString("mm:ss.zzz"));
    ui->lbTemps->setFont(timerFont);
    ui->lbTemps->setAlignment(Qt::AlignCenter);
}

