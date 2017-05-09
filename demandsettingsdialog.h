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

public slots:
    void UpdatePrices(QString label, int top, int left, int bottom, int right);
    void UpdateConsumption(QString label, int top, int left, int bottom, int right);
    void ToggleButton(bool status);

private slots:
    void on_modelingLinear_toggled(bool checked);
    void on_modelingExponential_toggled(bool checked);
    void on_modelingExponentiated_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::DemandSettingsDialog *ui;

    int topPrice;
    int leftPrice;
    int bottomPrice;
    int rightPrice;

    int topConsumption;
    int leftConsumption;
    int bottomConsumption;
    int rightConsumption;
};

#endif // DEMANDSETTINGSDIALOG_H
