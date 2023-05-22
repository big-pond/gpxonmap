#ifndef MAPINFODLG_H
#define MAPINFODLG_H
#include <QDialog>

class QLabel;
class QDialogButtonBox;

class MapInfoDlg : public QDialog
{
    Q_OBJECT
public:
    explicit MapInfoDlg(QDialog *parent = nullptr);

    void setMapProperty(const QString &prop);
signals:

private:
    QLabel* lbInfo;
    QDialogButtonBox* buttonBox;
};

#endif // MAPINFODLG_H
