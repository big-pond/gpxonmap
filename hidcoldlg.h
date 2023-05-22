#ifndef HIDCOLDLG_H
#define HIDCOLDLG_H

#include <QDialog>

class QTableView;
class QVBoxLayout;
class QDialogButtonBox;

class HidColDlg : public QDialog
{
    Q_OBJECT
private slots:
    virtual void accept ();
    void setColumnHidden();
public:
    explicit HidColDlg(QWidget *parent = 0);
    void setTableView(QTableView*);
private:
    QTableView* tv;
    QVBoxLayout *vlayout;
    QDialogButtonBox* buttonBox;
};

#endif // HIDCOLDLG_H
