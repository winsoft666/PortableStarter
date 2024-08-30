#include "EditDialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFileIconProvider>
#include "DSLLayout.hpp"
using namespace tk;

EditDialog::EditDialog(const AppMeta* app, QWidget* parent /*= nullptr*/) :
    QDialog(parent) {
    if (app)
        app_ = *app;
    setupUi();
    this->setWindowTitle(app_.name.isEmpty() ? tr("Add") : tr("Edit"));
    this->setFixedSize(500, 260);

    connect(btnCancel_, &QPushButton::clicked, this, [this]() { done(0); });

    connect(btnOK_, &QPushButton::clicked, this, [this]() {
        app_.path = editPath_->text().trimmed();
        app_.parameter = editParameter_->text().trimmed();
        app_.name = editName_->text().trimmed();
        app_.triggerKey = editTriggerKey_->text().trimmed();

        if (!app_.path.isEmpty()) {
            if (app_.path.startsWith("http://") || app_.path.startsWith("https://")) {
                app_.icon = QPixmap(":/images/website.png");
            }
            else {
                QFileInfo fi(app_.path);
                if (fi.isDir()) {
                    app_.icon = QPixmap(":/images/folder.png");
                }
                else if (fi.isFile()) {
                    QFileIconProvider iconProvider;
                    QIcon ico = iconProvider.icon(fi);
                    if (ico.isNull()) {
                        app_.icon = QPixmap(":/images/exe.png");
                    }
                    else {
                        app_.icon = ico.pixmap(48, 48);
                    }
                }
            }
        }

        done(100);
    });

    connect(btnBrowserExe_, &QPushButton::clicked, this, [this]() {
        const QString strPath = QFileDialog::getOpenFileName(
            this,
            tr("Select an executable file"),
            QString(),
            tr("Executable Files (*.exe *.bat *.cmd *.ps1 *.msc)"));
        if (!strPath.isEmpty()) {
            editPath_->setText(QDir::toNativeSeparators(strPath));
        }
    });

    connect(btnBrowserFolder_, &QPushButton::clicked, this, [this]() {
        const QString strPath = QFileDialog::getExistingDirectory(this);
        if (!strPath.isEmpty())
            editPath_->setText(QDir::toNativeSeparators(strPath));
    });

    connect(editPath_, &QLineEdit::textChanged, this, [this](const QString& text) {
        if (!text.isEmpty()) {
            // TODO
        }
    });
}

EditDialog::~EditDialog() {
}

AppMeta EditDialog::getAppMeta() const {
    return app_;
}

void EditDialog::setupUi() {
    editPath_ = new QLineEdit();
    editParameter_ = new QLineEdit();
    editName_ = new QLineEdit();
    editTriggerKey_ = new QLineEdit();

    chkRunAsAdmin_ = new QCheckBox(tr("Run as administrator"));

    btnOK_ = new QPushButton(tr("OK"));
    btnCancel_ = new QPushButton(tr("Cancel"));

    btnBrowserExe_ = new QPushButton("...");
    btnBrowserFolder_ = new QPushButton("F");

    auto root = VBox(
        HBox(new QLabel(tr("Application, URL or Folder:")), Stretch()),
        HBox(editPath_, btnBrowserExe_, btnBrowserFolder_),
        HBox(new QLabel(tr("Parameter:")), Stretch()),
        editParameter_,
        HBox(new QLabel(tr("Name:")), Stretch()),
        editName_,
        HBox(new QLabel(tr("Trigger Key (Split with comma):")), Stretch()),
        editTriggerKey_,
        HBox(chkRunAsAdmin_, Stretch()),
        Stretch(),
        HBox(Stretch(), btnOK_, btnCancel_, Stretch()));
    this->setLayout(root);
}
