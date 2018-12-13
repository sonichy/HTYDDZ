#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *e);

private:
    Ui::MainWindow *ui;
    QStringList poker, player1, player2, player3, player1_selected, player2_selected, player3_selected, player_last;
    bool checkRule(QStringList pokers_last, QStringList pokers_selected);
    void sleep(int ms);
    int current_player;
    QStringList AI(QStringList source, QStringList last);
    QStringList AI1(QStringList source);
    int bigger, turn, score1, score2, score3;
    void circle();

private slots:
    void on_action_shuffle_triggered();
    void on_pushButton_start_released();
    void on_pushButton_pass_released();
    void on_pushButton_play_released();
    void selectPoker();
    void togglePoker(bool b);
    bool checkWin();

};

#endif // MAINWINDOW_H