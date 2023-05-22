#include "levelwdg.h"
#include <QToolButton>
#include <QComboBox>
#include <QHBoxLayout>

LevelWdg::LevelWdg(QWidget *parent) :
    QWidget(parent)
{
    tbPlus = new QToolButton();
    tbPlus->setIcon(QIcon(":/resources/plus_l.png"));
    tbPlus->setToolTip(tr("Следующий уровень"));
    tbPlus->setCursor(Qt::ArrowCursor);
    tbMinus = new QToolButton();
    tbMinus->setIcon(QIcon(":/resources/minus_l.png"));
    tbMinus->setToolTip(tr("Предыдущий уровень"));
    tbMinus->setCursor(Qt::ArrowCursor);
    cbLevel = new QComboBox();
    cbLevel->setCursor(Qt::ArrowCursor);
    for(int i = 1; i<21; i++)
        cbLevel->addItem(QString("z %1").arg(i));
    QHBoxLayout* vbl = new QHBoxLayout();
    vbl->setContentsMargins(0, 0, 0, 0);
    vbl->setSpacing(0);
    vbl->addWidget(tbPlus);
    vbl->addWidget(cbLevel);
    vbl->addWidget(tbMinus);
    setLayout(vbl);

    setMinimumHeight(30);
    setMaximumHeight(30);
//    setGeometry(QRect(6, 6, 120, 30));
    cbLevel->setCurrentIndex(0);

    connect(tbMinus, SIGNAL(clicked()), this, SLOT(prevLevel()));
    connect(tbPlus, SIGNAL(clicked()), this, SLOT(nextLevel()));
    connect(cbLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(levelChanged(int)));
}

int LevelWdg::getLevel()
{
   return cbLevel->currentIndex();
}

void LevelWdg::setLevel(const int& val)
{
    cbLevel->setCurrentIndex(val);
}

void LevelWdg::prevLevel()
{
    cbLevel->setCurrentIndex(cbLevel->currentIndex()-1);
}

void LevelWdg::nextLevel()
{
    cbLevel->setCurrentIndex(cbLevel->currentIndex()+1);
}

void LevelWdg::levelChanged(int idx)
{
    emit sendLevel(idx);
    tbPlus->setEnabled(idx<cbLevel->count()-1);
    tbMinus->setEnabled(idx>0);
}
