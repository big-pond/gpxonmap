#ifndef LEVELWDG_H
#define LEVELWDG_H

#include <QWidget>

class QToolButton;
class QComboBox;

class LevelWdg : public QWidget
{
    Q_OBJECT
public:
    explicit LevelWdg(QWidget *parent = 0);
    int getLevel();
    void setLevel(const int& val);
signals:
    void sendLevel(int level);
private slots:
    void prevLevel();
    void nextLevel();
    void levelChanged(int);
private:
    QToolButton* tbPlus;
    QToolButton* tbMinus;
    QComboBox* cbLevel;
};

#endif // LEVELWDG_H
