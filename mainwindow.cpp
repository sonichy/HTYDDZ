#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>
#include <QDesktopWidget>
#include <QPushButton>
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_pass1->hide();
    ui->label_pass2->hide();
    ui->label_pass3->hide();
    ui->horizontalWidget->hide();
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
    ui->label_avatar1->setPixmap(QPixmap("./icons/farmer.gif").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->label_avatar2->setPixmap(QPixmap("./icons/farmer.gif").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->label_avatar3->setPixmap(QPixmap("./icons/landowner.gif").scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    setStyleSheet("QLabel{color:white;}");

    score1 = score2 = score3 = 0;
    QString s = "abcdefghijklm";
    QStringList SL = s.split("",QString::SkipEmptyParts);
    //qDebug() << SL;
    for (int i=0; i<SL.size(); i++) {
        for (int j=1; j<5; j++) {
            QString a = SL.at(i) + QString::number(j);
            poker << a;
        }
    }
    poker << "n1" << "n2";
    //qDebug() << poker;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap("./icons/background.jpg"));
}

void MainWindow::on_action_shuffle_triggered()
{
    player_last.clear();
    player1_selected.clear();
    player2_selected.clear();
    player3_selected.clear();
    ui->pushButton_start->hide();

    QList<QPushButton*> btns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, btns) {
        if(btn->objectName().contains("player") || btn->objectName().contains("played"))
            delete btn;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(poker.begin(), poker.end(), g);
    //qDebug() << poker;
    player1.clear();
    player2.clear();
    player3.clear();
    for(int i=0;i<poker.size();i+=3){
        player1 << poker.at(i);
        player2 << poker.at(i+1);
        player3 << poker.at(i+2);
    }
    sort(player1.rbegin(), player1.rend());
    sort(player2.rbegin(), player2.rend());
    sort(player3.rbegin(), player3.rend());
    for(int i=0; i<player1.size(); i++){
        QPushButton *pushButton = new QPushButton(this);
        pushButton->setObjectName("player1" + player1.at(i));
        pushButton->setFixedSize(110,160);
        pushButton->setIcon(QIcon("./icons/" + player1.at(i) + ".svg"));
        //pushButton->setIcon(QIcon("./icons/poker-back.jpg"));
        pushButton->setIconSize(QSize(110,160));
        pushButton->move(70, i*20+50);
        pushButton->setFlat(true);
        pushButton->show();
    }
    for(int i=0; i<player2.size(); i++){
        QPushButton *pushButton = new QPushButton(this);
        pushButton->setObjectName("player2" + player2.at(i));
        pushButton->setFixedSize(110,160);
        pushButton->setIcon(QIcon("./icons/" + player2.at(i) + ".svg"));
        //pushButton->setIcon(QIcon("./icons/poker-back.jpg"));
        pushButton->setIconSize(QSize(110,160));
        pushButton->move(width()-180, i*20+50);
        pushButton->setFlat(true);
        pushButton->show();
    }
    for(int i=0; i<player3.size(); i++){
        QPushButton *pushButton = new QPushButton(this);
        pushButton->setObjectName("player3" + player3.at(i));
        pushButton->setFixedSize(110,160);
        pushButton->setIcon(QIcon("./icons/" + player3.at(i) + ".svg"));
        pushButton->setIconSize(QSize(110,160));
        pushButton->move(i*30+190, height()-170);
        pushButton->setCheckable(true);
        pushButton->setFlat(true);
        pushButton->show();
        //connect(pushButton,SIGNAL(released()),this,SLOT(selectPoker()));
        connect(pushButton,SIGNAL(toggled(bool)),this,SLOT(togglePoker(bool)));
    }
    ui->horizontalWidget->show();
    current_player = 3;
}

void MainWindow::selectPoker()
{
    QPushButton *pushButton = (QPushButton*) sender();
    pushButton->move(pushButton->x(),pushButton->y()-20);
}

void MainWindow::togglePoker(bool b)
{
    QPushButton *pushButton = (QPushButton*) sender();
    if(b){
        pushButton->move(pushButton->x(),pushButton->y()-20);
        player3_selected << pushButton->objectName().replace("player3","");
    }else{
        pushButton->move(pushButton->x(),pushButton->y()+20);
        player3_selected.removeOne(pushButton->objectName().replace("player3",""));
    }
    if(checkRule(player_last, player3_selected)){
        ui->pushButton_play->setEnabled(true);
    }else{
        ui->pushButton_play->setEnabled(false);
    }
}

void MainWindow::on_pushButton_start_released()
{
    on_action_shuffle_triggered();
}

void MainWindow::on_pushButton_pass_released()
{
    turn = 2;
    QList<QPushButton*> btns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, btns) {
        if(btn->objectName().contains("played3"))
            delete btn;
    }
    ui->label_pass3->show();
    circle();
}

void MainWindow::circle()
{
    ui->label_pass1->hide();
    ui->label_pass2->hide();
    ui->label_pass3->hide();
    sleep(1000);
    QList<QPushButton*> btns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, btns) {
        if(btn->objectName().contains("played"))
            delete btn;
    }
    if(bigger==1 || turn==1){
        if(bigger==1)player1_selected = AI1(player1);
        if(turn==1) player2_selected = AI(player2,player_last);
        for(int i=0; i<player1_selected.size(); i++){
            player1.removeOne(player1_selected.at(i));
            QPushButton *button = findChild<QPushButton*>("player1" + player1_selected.at(i));
            button->move(200, 50+30*i);
            button->raise();
            button->setObjectName(button->objectName().replace("player","played"));
        }
        if (checkWin()) return;
        player_last = player1_selected;
    }else if(bigger==2 || turn==2){
        if(bigger==2){
            player2_selected = AI1(player2);
        }else if(turn==2){
            player2_selected = AI(player2,player_last);
        }
        for(int i=0; i<player2_selected.size(); i++){
            player2.removeOne(player2_selected.at(i));
            QPushButton *button = findChild<QPushButton*>("player2" + player2_selected.at(i));
            button->move(600, 50+30*i);
            button->raise();
            button->setObjectName(button->objectName().replace("player","played"));
        }
        if (checkWin()) return;
        player_last = player2_selected;
        turn=1;
        circle();
    }
}

void MainWindow::on_pushButton_play_released()
{
    bigger = 3;
    ui->label_pass1->hide();
    ui->label_pass2->hide();
    ui->label_pass3->hide();
    player1_selected.clear();
    player2_selected.clear();
    player_last = player3_selected;
    qDebug() << "player3" << player3_selected;

    QList<QPushButton*> btns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, btns) {
        if(btn->objectName().contains("played3"))
            delete btn;
    }
    int count = player3_selected.size();
    for(int i=0; i<count; i++){
        player3.removeOne(player3_selected.at(i));
        QPushButton *button = findChild<QPushButton*>("player3" + player3_selected.at(i));
        button->move(300+i*30, 300);
        button->raise();
        button->setObjectName(button->objectName().replace("player","played"));
        //qDebug() << button->objectName();
    }
    if (checkWin()) return;
    ui->pushButton_play->setEnabled(false);

    player2_selected = AI(player2, player_last);
    sleep(1000);
    btns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, btns) {
        if(btn->objectName().contains("played2"))
            delete btn;
    }
    if (player2_selected.isEmpty()){
        ui->label_pass2->show();
    } else {
        ui->label_pass2->hide();
        player_last = player2_selected;
        bigger = 2;
        qDebug() << player2_selected;
        for(int i=0; i<player2_selected.size(); i++){
            player2.removeOne(player2_selected.at(i));
            QPushButton *button = findChild<QPushButton*>("player2" + player2_selected.at(i));
            button->move(600, 50+30*i);
            button->raise();
            button->setObjectName(button->objectName().replace("player","played"));
        }
        if (checkWin()) return;
    }

    player1_selected = AI(player1, player_last);
    sleep(1000);
    btns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, btns) {
        if(btn->objectName().contains("played1"))
            delete btn;
    }
    if (player1_selected.isEmpty()){
        ui->label_pass1->show();
    } else {
        ui->label_pass2->hide();
        player_last = player1_selected;
        bigger = 1;
        qDebug() << player1_selected;
        for(int i=0; i<player1_selected.size(); i++){
            player1.removeOne(player1_selected.at(i));
            QPushButton *button = findChild<QPushButton*>("player1" + player1_selected.at(i));
            button->move(200, 50+30*i);
            button->raise();
            button->setObjectName(button->objectName().replace("player","played"));
        }
        if (checkWin()) return;
    }

    if (player1_selected.isEmpty() && player2_selected.isEmpty()) player_last.clear();

    player3_selected.clear();
    ui->pushButton_play->setEnabled(true);
}

bool MainWindow::checkRule(QStringList pokers_last, QStringList pokers_selected)
{
    pokers_selected.sort();
    qDebug() <<  pokers_last << pokers_selected;
    if(pokers_last.isEmpty()){
        int count = pokers_selected.size();
        if(count == 1){
            return true;
        }else if(count == 2){
            QString a1 = pokers_selected.at(0);
            a1.chop(1);
            QString a2 = pokers_selected.at(1);
            a2.chop(1);
            if(a1==a2){
                return true;
            }else{
                return false;
            }
        }else if(count==3){
            QString a1 = pokers_selected.at(0);
            a1.chop(1);
            QString a2 = pokers_selected.at(1);
            a2.chop(1);
            QString a3 = pokers_selected.at(2);
            a3.chop(1);
            if(a1==a2 && a2==a3){
                return true;
            }else{
                return false;
            }
        }else if(count==4){
            QString a1 = pokers_selected.at(0);
            a1.chop(1);
            QString a2 = pokers_selected.at(1);
            a2.chop(1);
            QString a3 = pokers_selected.at(2);
            a3.chop(1);
            QString a4 = pokers_selected.at(3);
            a4.chop(1);
            if(a1==a2 && a2==a3 && a3==a4){
                return true;
            }else if((a1==a2 && a2==a3) || (a2==a3 && a3==a4)){
                return true;
            }else {
                return false;
            }
        }else if(count>4){
            for(int i=0; i<pokers_selected.size()-1; i++){
                QString a = player3_selected.at(i);
                a.chop(1);
                QString b = player3_selected.at(i+1);
                b.chop(1);
                ushort as = a.at(0).unicode();
                ushort bs = b.at(0).unicode();
                qDebug() << a << as << b << bs;
                if (as != bs-1) return false;
            }
            return true;
        }
    }else{
        int count = pokers_last.size();
        if(pokers_selected.size() == count){
            if(count == 1){
                QString a = pokers_last.at(0);
                a.chop(1);
                QString b = pokers_selected.at(0);
                b.chop(1);
                if(a!="n"){
                    if(b>a){
                        return true;
                    }else{
                        return false;
                    }
                }else{
                    if(pokers_selected.at(0) > pokers_last.at(0)){
                        return true;
                    }else{
                        return false;
                    }
                }
            }else if(count == 2){
                QString a1 = pokers_last.at(0);
                a1.chop(1);
                QString a2 = pokers_last.at(1);
                a2.chop(1);
                QString b1 = pokers_selected.at(0);
                b1.chop(1);
                QString b2 = pokers_selected.at(1);
                b2.chop(1);
                if(a1==a2 && b1==b2 && b1>a1){
                    return true;
                }else{
                    return false;
                }
            }else if(count==3){
                QString a1 = pokers_last.at(0);
                a1.chop(1);
                QString a2 = pokers_last.at(1);
                a2.chop(1);
                QString a3 = pokers_last.at(2);
                a3.chop(1);
                QString b1 = pokers_selected.at(0);
                b1.chop(1);
                QString b2 = pokers_selected.at(1);
                b2.chop(1);
                QString b3 = pokers_selected.at(2);
                b3.chop(1);
                if(a1==a2 && a2==a3 && b1==b2 && b2==b3 && b1>a1){
                    return true;
                }else{
                    return false;
                }
            }else if(count==4){
                QString a1 = pokers_last.at(0);
                a1.chop(1);
                QString a2 = pokers_last.at(1);
                a2.chop(1);
                QString a3 = pokers_last.at(2);
                a3.chop(1);
                QString a4 = pokers_last.at(3);
                a4.chop(1);
                QString b1 = pokers_selected.at(0);
                b1.chop(1);
                QString b2 = pokers_selected.at(1);
                b2.chop(1);
                QString b3 = pokers_selected.at(2);
                b3.chop(1);
                QString b4 = pokers_selected.at(3);
                b4.chop(1);
                if(a1==a2 && a2==a3 && a3==a4 && b1==b2 && b2==b3 && b3==b4 && b1>a1){
                    return true;
                }else if(a1==a2 && a2==a3){
                    if(b1==b2 && b2==b3 && b1>a1){
                        return true;
                    }else if(b2==b3 && b3==b4 && b2>a1){
                        return true;
                    }
                }else if(a2==a3 && a3==a4){
                    if(b1==b2 && b2==b3 && b1>a2){
                        return true;
                    }else if(b2==b3 && b3==b4 && b2>a2){
                        return true;
                    }
                }
            }else if(count>4){
                for(int i=0; i<pokers_selected.size()-1; i++){
                    QString a = player3_selected.at(i);
                    a.chop(1);
                    QString b = player3_selected.at(i+1);
                    b.chop(1);
                    ushort as = a.at(0).unicode();
                    ushort bs = b.at(0).unicode();
                    qDebug() << a << as << b << bs;
                    if(as != bs-1) return false;
                }
                if(player3_selected.at(0) > pokers_last.at(0)){
                    return true;
                }else{
                    return false;
                }
            }
        }
    }
    return false;
}

bool MainWindow::checkWin()
{
    if(player1.size()==0){
        score1 += 3;
        ui->label_score1->setText(QString::number(score1));
        ui->pushButton_start->show();
        ui->horizontalWidget->hide();
        QMessageBox MB(QMessageBox::NoIcon, "结局", "玩家1赢了！");
        const QPixmap *pixmap = ui->label_avatar1->pixmap();
        MB.setIconPixmap(*pixmap);
        MB.exec();
        return true;
    }else if(player2.size()==0){
        score2 += 3;
        ui->label_score2->setText(QString::number(score2));
        ui->pushButton_start->show();
        ui->horizontalWidget->hide();
        QMessageBox MB(QMessageBox::NoIcon, "结局", "玩家2赢了！");
        const QPixmap *pixmap = ui->label_avatar2->pixmap();
        MB.setIconPixmap(*pixmap);
        MB.exec();
        return true;
    }else if(player3.size()==0){
        score3 += 3;
        ui->label_score3->setText(QString::number(score3));
        ui->pushButton_start->show();
        ui->horizontalWidget->hide();
        QMessageBox MB(QMessageBox::NoIcon, "结局", "玩家3赢了！");
        const QPixmap *pixmap = ui->label_avatar3->pixmap();
        MB.setIconPixmap(*pixmap);
        MB.exec();
        return true;
    }
    return false;
}

void MainWindow::sleep(int ms)
{
    QTime time_end = QTime::currentTime().addMSecs(ms);
    while(QTime::currentTime() < time_end)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

QStringList MainWindow::AI(QStringList source, QStringList last)
{
    QStringList selected;
    int count = last.size();
    if(count ==1) {
        for(int i=source.size()-1; i>=0; i--){
            QString a = last.at(0);
            a.chop(1);
            QString b = source.at(i);
            b.chop(1);
            if(b>a){
                selected << source.at(i);
                break;
            }
        }
    }else{
        QStringList temp;
        for(int i=0; i<source.size(); i++){
            QString a = source.at(i);
            a.chop(1);
            temp << a;
        }

        if(count == 2){
            for(int i=temp.size()-1; i>0; i--){
                if(temp.at(i) > last.at(0)){
                    if(temp.at(i) == temp.at(i-1)){
                        selected << source.at(i) << source.at(i-1);
                        break;
                    }
                }
            }
        }else if(count == 3){
            for(int i=temp.size()-1; i>=0; i--){
                if(temp.at(i) > last.at(0)){
                    if(temp.at(i) == temp.at(i+1) && temp.at(i) == temp.at(i+2)){
                        selected << source.at(i) << source.at(i+1) << source.at(i+2);
                        break;
                    }
                }
            }
        }else if(count == 4){
            QString a1 = last.at(0);
            a1.chop(1);
            QString a2 = last.at(1);
            a2.chop(1);
            QString a3 = last.at(2);
            a3.chop(1);
            QString a4 = last.at(3);
            a4.chop(1);
            if(a1==a2 && a2==a3 && a3==a4){
                for(int i=temp.size()-1; i>=0; i--){
                    if(temp.at(i) > last.at(0)){
                        if(temp.at(i) == temp.at(i+1) && temp.at(i) == temp.at(i+2) && temp.at(i) == temp.at(i+3)){
                            selected << source.at(i) << source.at(i+1) << source.at(i+2) << source.at(i+3);
                            break;
                        }
                    }
                }
            }else if(a1==a2 && a2==a3){

            }else if(a2==a3 && a3==a4){

            }
        }
    }
    return selected;
}

QStringList MainWindow::AI1(QStringList source)
{
    QStringList temp, decision;
    for(int i=0; i<source.size(); i++){
        QString a = source.at(i);
        a.chop(1);
        temp << a;
    }
    QMap<QString,int> stringCount;
    for (int i=0; i<temp.count(); i++) {
        stringCount[temp[i]]++;
    }
    if(stringCount.key(3)!=""){
        qDebug() << "AI1：3个" << stringCount.key(3);
        for(int i=0; i<source.size(); i++){
            if(source.at(i).contains(stringCount.key(3)))
                decision << source.at(i);
        }
    }else if(stringCount.key(2)!=""){
        qDebug() << "AI1：2个" << stringCount.key(2);
        for(int i=0; i<source.size(); i++){
            if(source.at(i).contains(stringCount.key(2)))
                decision << source.at(i);
        }
    }else{
        decision << source.at(source.size()-1);
    }
    return decision;
}