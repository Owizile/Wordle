#ifndef WORDLEWINDOW_H
#define WORDLEWINDOW_H

#include "popup.h"
#include <QMainWindow>
#include <QAbstractButton>
#include <QLabel>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class WordleWindow; }
QT_END_NAMESPACE

class WordleWindow : public QMainWindow
{
    Q_OBJECT

public:
    WordleWindow(QWidget *parent = nullptr);
    ~WordleWindow();


public slots:
    void keyClicked(QAbstractButton *key);

private slots:

    void on_EnterKey_clicked();
    void on_HelpButton_clicked();
    void on_WordleButton_clicked();
    void on_StatsButton_clicked();
    void on_BackespaceKey_clicked();

    void fillDictionary(QStringList &dictionary, QFile &file);
    void newWordle();
    void fillJsonObject(QJsonObject &jsonObject, QFile &file);
    void setStatistics();
    void updateJsonFile(QFile &file, QJsonObject &jsonObject);
    QList<QLabel *>& availableLabels(QList<QLabel *> &labelList);
    void gameStarted();
    void gameEnded();
    void winEvent();
    void loseEvent();
    int emptyLabelIndex(QList<QLabel *> &labelList);

private:
    Ui::WordleWindow *ui;
    bool isGameStopped = false;
    QJsonObject statistics;
    QString wordle = "";
    QStringList quizWords;
    QStringList dictionary;
    PopUp *popUp;
};
#endif // WORDLEWINDOW_H
