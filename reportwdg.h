#ifndef REPORTWDG_H
#define REPORTWDG_H

#include <QWidget>

class QComboBox;
class QPushButton;
class QStandardItem;
class QStandardItemModel;
class QLabel;
class QTableView;
class Gpx;

class ReportWdg : public QWidget
{
    Q_OBJECT
public:
    static const int COL_COUNT;
    static const int COL_COUNT2;
    static const int defaultColW[];
    enum {RPoint, RDist, RArr, RDep, RStopt, RStopd, RMovet, RSpeed, RTrjamt, RTrjamd, RStopti, RStopdi, RPath, RFuel};
    enum {RParam, RValue, RNote2};
    explicit ReportWdg(QWidget *parent = nullptr);
    void createReport(Gpx* gpx);

signals:
protected:
    void closeEvent(QCloseEvent* event);
private:
    QString codec_name;
    QString float_separator;
    QComboBox* cbCodec;
    QComboBox* cbSeparator;
    QPushButton* pbSaveCsv;
    QStandardItemModel* model;
    QStandardItemModel* sum_model;

    QLabel* lbTimeOffset;
    QTableView* tableView;
    QTableView* tableViewSum;
    void readSettings();
    void writeSettings();
    QList<QStandardItem *> *createRow(int col_count);
};

#endif // REPORTWDG_H
