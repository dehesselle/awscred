#ifndef PROFILESDIALOG_HPP
#define PROFILESDIALOG_HPP

#include <QAction>
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

    void populate();
    void addButton(const QString &profile);

public slots:
    bool updateProfile(const QString &profile);
    void parseClipboard();
    void showAboutDialog();

private slots:
    void on_pbClose_clicked();
    void on_pbNew_clicked();

private:
    void createTrayIcon();

    Ui::ProfilesDialog *ui;
};
#endif // PROFILESDIALOG_HPP
