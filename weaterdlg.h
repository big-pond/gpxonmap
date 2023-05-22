#ifndef WEATERDLG_H
#define WEATERDLG_H

#include <QDialog>

class QComboBox;
class QTextEdit;
class QDialogButtonBox;
class WeatherReader;

class WeaterDlg : public QDialog
{
    Q_OBJECT
public:
    explicit WeaterDlg(QWidget *parent = 0);
public slots:
    virtual void reject();
    void getWeather();
    void addResult(const QString& text);
protected:
    WeatherReader* weather_reader;
    QComboBox* cbTown;
    QTextEdit* textEdit;
    QDialogButtonBox* buttonBox;
    void readSettings();
    void writeSettings();

};

#endif // WEATERDLG_H
