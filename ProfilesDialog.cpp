#include "ProfilesDialog.hpp"
#include "./ui_ProfilesDialog.h"
#include "AWSCredentials.hpp"

ProfilesDialog::ProfilesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProfilesDialog)
{
    ui->setupUi(this);

    auto awsCred = AWSCredentials();

    awsCred.setProfileFromText(
        "foo",
        "[default]\naws_access_key=hans\naws_secret_access_key=peter\naws_session_token=blub\n");
}

ProfilesDialog::~ProfilesDialog()
{
    delete ui;
}
