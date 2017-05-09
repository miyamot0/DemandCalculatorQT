#ifndef DEMANDSETTINGSDIALOG_H
#define DEMANDSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class DemandSettingsDialog;
}

class DemandSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DemandSettingsDialog(QWidget *parent = 0);
    ~DemandSettingsDialog();

private:
    Ui::DemandSettingsDialog *ui;
};

#endif // DEMANDSETTINGSDIALOG_H
