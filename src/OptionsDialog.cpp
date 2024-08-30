#include "OptionsDialog.h"
#include <QMessageBox>
#include "Helper.h"
#include "MainWindow.h"
#include "DSLLayout.hpp"
using namespace tk;

OptionsDialog::OptionsDialog(MainWindow* parent /*= nullptr*/) :
    QDialog(parent) {
    setupUi();

    this->resize(360, 220);

    connect(btnOK_, &QPushButton::clicked, this, [this]() { this->done(0); });
    connect(chkStartWithOS_, &QCheckBox::stateChanged, this, [this](int state) {
        GetSettings().setValue("StartWithOS", state == Qt::Checked ? 1 : 0);
        GetSettings().sync();

        SetStartWithOS(state == Qt::Checked);
    });

    connect(keyEditQuickStart_, &QKeySequenceEdit::editingFinished, this, [this]() {
        QString str = keyEditQuickStart_->keySequence().toString();
        GetSettings().setValue("QuickStartHotkey", str);
        GetSettings().sync();
        if (!static_cast<MainWindow*>(parentWidget())->setQuickStartHotkey(str)) {
            QMessageBox::critical(this, "PortableStarter", tr("Failed to register quick start hotkey!"));
        }
    });
}

void OptionsDialog::setupUi() {
    this->setWindowTitle(tr("Options"));

    QSettings& settings = GetSettings();

    chkStartWithOS_ = new QCheckBox(tr("Start with OS"));
    chkStartWithOS_->setCheckState(settings.value("StartWithOS").toInt() == 1 ? Qt::Checked : Qt::Unchecked);

    keyEditQuickStart_ = new QKeySequenceEdit(settings.value("QuickStartHotkey").toString());

    btnOK_ = new QPushButton(tr("OK"));

    auto root = VBox(
        HBox(chkStartWithOS_, Stretch()),
        Spacing(10),
        HBox(new QLabel(tr("Shortcut to quick start (eg. Alt+R)")), Stretch()),
        HBox(keyEditQuickStart_, Stretch()),
        Stretch(),
        HBox(Stretch(), btnOK_, Stretch()));
    this->setLayout(root);
}
