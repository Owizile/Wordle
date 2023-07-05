#include "wordlewindow.h"
#include "./ui_wordlewindow.h"
#include <QDir>
#include <QFileDialog>
#include <QTextStream>

WordleWindow::WordleWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WordleWindow)
{
    ui->setupUi(this);
    connect(ui->Keyboard, &QButtonGroup::buttonClicked, this, &WordleWindow::keyClicked);

    gameStarted();
    popUp = new PopUp();
}

WordleWindow::~WordleWindow()
{
    delete ui;
}


bool labelLessThan(const QLabel *label1, const QLabel *label2){
    return label1->objectName() < label2->objectName();
}


void WordleWindow::newWordle(){
    srand(time(NULL));
    wordle = quizWords[rand() % quizWords.length()];
    qDebug() << wordle;
}


void WordleWindow::fillDictionary(QStringList &dictionary, QFile &file){
    file.open(QIODevice::ReadOnly);
    QByteArray words = file.readAll();
    dictionary = QString(words).split(" ");
    file.close();
}


void WordleWindow::fillJsonObject(QJsonObject &jsonObject, QFile &file){
    QString data;
    file.open(QIODevice::ReadOnly);
    data = file.readAll();
    file.close();
    QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());
    jsonObject = document.object();
}


void WordleWindow::updateJsonFile(QFile &file, QJsonObject &jsonObject){
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(QJsonDocument(jsonObject).toJson(QJsonDocument::Indented));
    file.close();
}


void WordleWindow::setStatistics(){
    int firstTry = statistics["firstTry"].toString().toInt();
    int secondTry = statistics["secondTry"].toString().toInt();
    int thirdTry = statistics["thirdTry"].toString().toInt();
    int fourthTry = statistics["fourthTry"].toString().toInt();
    int fifthTry = statistics["fifthTry"].toString().toInt();
    int sixthTry = statistics["sixthTry"].toString().toInt();
    int gameCount = statistics["gameCount"].toString().toInt();
    double winCount = statistics["winCount"].toString().toDouble();
    int winRate = gameCount ? statistics["winCount"].toString().toDouble() / statistics["gameCount"].toString().toDouble() * 100 : 0;

    ui->PlayedLabel_1->setText(statistics["gameCount"].toString());
    ui->WinPercentLabel_1->setText(QString::number(winRate));
    ui->CurrentStreakLabel_1->setText(statistics["currentStreak"].toString());
    ui->MaxStreakLabel_1->setText(statistics["maxStreak"].toString());

    firstTry ? ui->progressBar_1->setValue(firstTry / winCount * 100) : void();
    secondTry ? ui->progressBar_2->setValue(secondTry / winCount * 100) : void();
    thirdTry ? ui->progressBar_3->setValue(thirdTry / winCount * 100) : void();
    fourthTry ? ui->progressBar_4->setValue(fourthTry / winCount * 100) : void();
    fifthTry ? ui->progressBar_5->setValue(fifthTry / winCount * 100) : void();
    sixthTry ? ui->progressBar_6->setValue(sixthTry / winCount * 100) : void();

    ui->progressBar_1->setFormat(statistics["firstTry"].toString());
    ui->progressBar_2->setFormat(statistics["secondTry"].toString());
    ui->progressBar_3->setFormat(statistics["thirdTry"].toString());
    ui->progressBar_4->setFormat(statistics["fourthTry"].toString());
    ui->progressBar_5->setFormat(statistics["fifthTry"].toString());
    ui->progressBar_6->setFormat(statistics["sixthTry"].toString());
}


QList<QLabel *>& WordleWindow::availableLabels(QList<QLabel *> &labelList){
    int currentIndex = 0;
    while(currentIndex < labelList.length()){
        if (labelList[currentIndex]->styleSheet().indexOf("background-color") != -1){
            labelList.remove(currentIndex);
            currentIndex = 0;
            continue;
        }
        currentIndex++;
    }
    return labelList;
}


int WordleWindow::emptyLabelIndex(QList<QLabel *> &labelList){
    int emptyLabelIndex = 0;
    labelList = availableLabels(labelList);

    for (auto& label : labelList){
        if(label->text() == ""){
            break;
        }
        emptyLabelIndex++;
    }

    if (emptyLabelIndex > labelList.length() || labelList.isEmpty()){
        return -1;
    }

    return emptyLabelIndex;
}


void WordleWindow::gameStarted(){
    ui->stackedWidget->setCurrentWidget(ui->QuizPage);

    QFile dictionaryFile(":/txt/resources/Dictionary.txt");
    QFile quizWordsFile(":/txt/resources/QuizWords.txt");
    QFile zeroStatisticsFile(":/json/resources/statistics.json");
    QString path("resources//");
    QDir dir;

    if (!dir.exists(path)){
        dir.mkpath(path);
    }
    QFile statisticsFile = (path + "statistics.json");
    if(!statisticsFile.exists()){
        QString zeroStats;
        zeroStatisticsFile.open(QIODevice::ReadOnly);
        zeroStats = zeroStatisticsFile.readAll();
        statisticsFile.open(QIODevice::WriteOnly);
        QTextStream out(&statisticsFile);
        out << zeroStats;
        statisticsFile.close();
    }

    fillDictionary(dictionary, dictionaryFile);
    fillDictionary(quizWords, quizWordsFile);
    fillJsonObject(statistics, statisticsFile);

    setStatistics();
    newWordle();

}


void WordleWindow::gameEnded(){
    QFile statisticsFile("resources//statistics.json");
    isGameStopped = true;

    QTimer::singleShot(3500, [this](){
        isGameStopped = false;
        QList quizLabels(ui->QuizWidget->findChildren<QLabel *>());

        for (auto& label : quizLabels){
            label->clear();
            label->setStyleSheet("");
        }

        newWordle();
    });

    updateJsonFile(statisticsFile, statistics);
    setStatistics();
}


void WordleWindow::winEvent(){
    popUp->setPopupText("Congrats! Wordle is " + wordle);
    popUp->show();

    QList quizLabels(ui->QuizWidget->findChildren<QLabel *>());
    std::sort(quizLabels.begin(), quizLabels.end(), labelLessThan);
    quizLabels = availableLabels(quizLabels);

    switch(quizLabels.length()){
    case 25: statistics["firstTry"] = QString::number(statistics["firstTry"].toString().toInt() + 1);
        break;
    case 20: statistics["secondTry"] = QString::number(statistics["secondTry"].toString().toInt() + 1);
        break;
    case 15: statistics["thirdTry"] = QString::number(statistics["thirdTry"].toString().toInt() + 1);
        break;
    case 10: statistics["fourthTry"] = QString::number(statistics["fourthTry"].toString().toInt() + 1);
        break;
    case 5: statistics["fifthTry"] = QString::number(statistics["fifthTry"].toString().toInt() + 1);
        break;
    case 0: statistics["sixthTry"] = QString::number(statistics["sixthTry"].toString().toInt() + 1);
        break;
    }

    statistics["gameCount"] = QString::number(statistics["gameCount"].toString().toInt() + 1);
    statistics["winCount"] = QString::number(statistics["winCount"].toString().toInt() + 1);
    statistics["currentStreak"] = QString::number(statistics["currentStreak"].toString().toInt() + 1);
    if(statistics["currentStreak"].toString().toInt() > statistics["maxStreak"].toString().toInt()){
        statistics["maxStreak"] = QString::number(statistics["currentStreak"].toString().toInt());
    }

    gameEnded();
}


void WordleWindow::loseEvent(){
    popUp->setPopupText("GL next time! Wordle is " + wordle);
    popUp->show();

    statistics["gameCount"] = QString::number(statistics["gameCount"].toString().toInt() + 1);
    statistics["currentStreak"] = "0";

    gameEnded();
}


void WordleWindow::keyClicked(QAbstractButton *key){
    if(isGameStopped){
        return;
    }

    QList quizLabels(ui->QuizWidget->findChildren<QLabel *>());
    std::sort(quizLabels.begin(), quizLabels.end(), labelLessThan);
    int labelIndex = emptyLabelIndex(quizLabels);
    if(labelIndex == -1 || labelIndex % 5 == 0 && labelIndex != 0){
        return;
    }
    quizLabels[labelIndex]->setText(key->text());
    quizLabels[labelIndex]->setStyleSheet("border: 2px solid black;\n");
}


void WordleWindow::on_BackespaceKey_clicked(){
    if(isGameStopped){
        return;
    }

    QList quizLabels(ui->QuizWidget->findChildren<QLabel *>());
    std::sort(quizLabels.begin(), quizLabels.end(), labelLessThan);
    int labelIndex = emptyLabelIndex(quizLabels);
    if(labelIndex == 0 || labelIndex == -1){
        return;
    }
    quizLabels[labelIndex - 1]->setText("");
    quizLabels[labelIndex - 1]->setStyleSheet("");
}


void WordleWindow::on_EnterKey_clicked(){
    if(isGameStopped){
        return;
    }

    QString currentWord;
    QString currentWordle = wordle;
    QList quizLabels(ui->QuizWidget->findChildren<QLabel *>());
    std::sort(quizLabels.begin(), quizLabels.end(), labelLessThan);

    quizLabels = availableLabels(quizLabels);

    for (int i = 0; i < 5; i++){
        currentWord += quizLabels[i]->text();
    }

    if(dictionary.indexOf(currentWord) == -1){
        if(currentWord.length() == 5){
            popUp->setPopupText("Not in word list");
            popUp->show();
            return;
        }

        popUp->setPopupText("Not enough letters");
        popUp->show();
        return;
    }

    for (int i = 0; i < 5; i++){
        if(quizLabels[i]->text() == wordle[i]){
            quizLabels[i]->setStyleSheet("background-color: #6aac64;\ncolor: white;\nborder-style:hidden;\n");
            currentWordle[i] = '0';
        }
    }

    for (int i = 0; i < 5; i++){
        if(quizLabels[i]->styleSheet().indexOf("background-color") != -1){
            continue;
        }
        if(wordle.indexOf(quizLabels[i]->text()) != -1 && currentWordle.indexOf(quizLabels[i]->text()) != -1){
            currentWordle[wordle.indexOf(quizLabels[i]->text())] = '0';
            quizLabels[i]->setStyleSheet("background-color: #cab557;\ncolor: white;\nborder-style:hidden;\n");
        }
        else {
            quizLabels[i]->setStyleSheet("background-color: #797d7f;\ncolor: white;\nborder-style:hidden;\n");
        }
    }

    quizLabels = availableLabels(quizLabels);

    if (currentWord == wordle){
        winEvent();
    }

    else if (quizLabels.isEmpty()){
        loseEvent();
    }
}


void WordleWindow::on_HelpButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->HelpPage);
}


void WordleWindow::on_WordleButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->QuizPage);
}


void WordleWindow::on_StatsButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->StatsPage);
}
