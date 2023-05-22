#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QApplication>
#include <QSettings>

#include "weaterdlg.h"
#include "weatherreader.h"

WeaterDlg::WeaterDlg(QWidget *parent) :  QDialog(parent)
{
    weather_reader = new WeatherReader(this);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(4,4,4,4);

    QGridLayout *gl0 = new QGridLayout;
    gl0->setVerticalSpacing(2);
    gl0->addWidget(new QLabel(tr("Town"), this),0,0);
    cbTown = new QComboBox(this);
    cbTown->setEditable(true);
    cbTown->setDuplicatesEnabled(false);
    cbTown->setInsertPolicy(QComboBox::InsertAlphabetically);
    gl0->addWidget(cbTown,0,1);
    QPushButton* pbGetWeather = new QPushButton("Get weather", this);
    gl0->addWidget(pbGetWeather,1,1);
    textEdit = new QTextEdit(this);
    gl0->addWidget(textEdit,2,0,2,2);
    vlayout->addLayout(gl0);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
//    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    vlayout->addWidget(buttonBox);
    setLayout(vlayout);
//    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pbGetWeather, SIGNAL(clicked()), SLOT(getWeather()));
    connect(weather_reader, SIGNAL(resultReceived(QString)), SLOT(addResult(QString)));

    setWindowTitle(tr("Weather"));
    readSettings();
    if (cbTown->count()==0)
        cbTown->addItem(tr("Moscow"));
}

void WeaterDlg::reject()
{
    writeSettings();
    QDialog::reject();
}

void WeaterDlg::readSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);
        settings.beginGroup("WeaterDlg");
        bool geom = settings.contains("geometry");
        if (geom)
            restoreGeometry(settings.value("geometry").toByteArray());
//        else
//            setGeometry(50, 50, 500, 300);
        settings.endGroup();

        settings.beginGroup("WeaterTownList");
        int size = settings.beginReadArray("town_count");
        for (int i=0; i<size; i++)
        {
            settings.setArrayIndex(i);
            QString town = settings.value("Town").toString();
            if (!town.isEmpty())
                cbTown->addItem(town);
        }
        settings.endArray();
        settings.endGroup();
}

void WeaterDlg::writeSettings()
{
    QSettings settings(QString("%1.ini").arg(QApplication::applicationName()), QSettings::IniFormat);

    settings.beginGroup("WeaterDlg");
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();

    settings.beginGroup("WeaterTownList");
    settings.beginWriteArray("town_count");
    for (int i=0; i<cbTown->count(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("Town", cbTown->itemText(i));
    }
    settings.endArray();
    settings.endGroup();
}

void WeaterDlg::getWeather()
{
    QString txt = cbTown->currentText();
    if (cbTown->findText(txt, Qt::MatchFixedString)==-1)
        cbTown->addItem(txt);

    weather_reader->getWeather(txt);
}

void WeaterDlg::addResult(const QString &text)
{
    textEdit->append(text);
}

