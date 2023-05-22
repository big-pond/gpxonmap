#ifndef TOPNTFROM_H
#define TOPNTFROM_H

#include <QWidget>

class QTableView;
class QLabel;
class QStandardItemModel;
class Gpx;

class ToPntFrom : public QWidget
{
    Q_OBJECT
public:
    explicit ToPntFrom(QWidget *parent = nullptr);

    void calcCharact(Gpx *gpx, int idx);
signals:
protected:
     virtual void closeEvent(QCloseEvent *event);
private:
    QTableView* tableView;
    QLabel* lbTotal;
    QStandardItemModel* model;
    void setRowTable(int row, const QString &charact, const QString &val1, const QString &val2);
    void writeSettings();
    void readSettings();
};

#endif // TOPNTFROM_H
