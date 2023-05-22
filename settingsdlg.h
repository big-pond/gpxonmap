#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>

namespace Ui {
class SettingsDlg;
}

class WptMrk;

class SettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDlg(QWidget *parent = nullptr);
    ~SettingsDlg();
    void setWptMark(WptMrk* val);

public slots:
    void accept();
private slots:
    void selectGpxDir();
private:
    Ui::SettingsDlg *ui;
    WptMrk* wptmrk;
};

#endif // SETTINGSDLG_H
