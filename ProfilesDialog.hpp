#ifndef PROFILESDIALOG_HPP
#define PROFILESDIALOG_HPP

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class ProfilesDialog;
}
QT_END_NAMESPACE

class ProfilesDialog : public QDialog
{
    Q_OBJECT

public:
    ProfilesDialog(QWidget *parent = nullptr);
    ~ProfilesDialog();

private:
    Ui::ProfilesDialog *ui;
};
#endif // PROFILESDIALOG_HPP
