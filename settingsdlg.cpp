#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QFileDialog>
#include "wptmrk.h"
#include "def.h"

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDlg)
{
    ui->setupUi(this);
    ui->leGpxDir->setText(Def::gpxdir);
    connect(ui->tbGpxDir, SIGNAL(clicked()), SLOT(selectGpxDir()));
}

SettingsDlg::~SettingsDlg()
{
    delete ui;
}

void SettingsDlg::accept()
{
    if (Def::gpxdir != ui->leGpxDir->text())
        Def::gpxdir = ui->leGpxDir->text();
    QDialog::accept();
}

void SettingsDlg::setWptMark(WptMrk *val)
{
    wptmrk = val;
    ui->tvSign->setModel(wptmrk);
    ui->tvSign->resizeColumnsToContents();
    ui->tvSign->resizeRowsToContents();
}

void SettingsDlg::selectGpxDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory"), ui->leGpxDir->text(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        ui->leGpxDir->setText(dir);
}
