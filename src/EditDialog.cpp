#include "EditDialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFileIconProvider>
#include "Helper.h"
#include "DSLLayout.hpp"
using namespace tk;

EditDialog::EditDialog(const AppMeta* app, QWidget* parent /*= nullptr*/) :
    QDialog(parent) {
    if (app)
        app_ = *app;

    setupUi();
    this->setWindowTitle(app_.name.isEmpty() ? tr("Add") : tr("Edit"));
    this->resize(500, 320);

    connect(btnCancel_, &QPushButton::clicked, this, [this]() { done(0); });

    connect(btnOK_, &QPushButton::clicked, this, [this]() {
        app_.path = editPath_->text().trimmed();
        app_.parameter = editParameter_->text().trimmed();
        app_.name = editName_->text().trimmed();
        app_.triggerKey = editTriggerKey_->text().trimmed();

        if (!app_.path.isEmpty()) {
            if (IsUrl(app_.path)) {
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
            tr("Executable Files (*.exe *.bat *.cmd *.ps1 *.msc *.dmg);;All Files (*.*)"));
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
        if (!IsUrl(text)) {
            QFileInfo fi(text);
            editName_->setText(fi.baseName());
        }
    });

    connect(editName_, &QLineEdit::textChanged, this, [this](const QString& text) {
        QList<QString> letterList;
        if (GetStringLetters(text, letterList)) {
            editTriggerKey_->setText(letterList.join(","));
        }
    });
}

EditDialog::~EditDialog() {
}

AppMeta EditDialog::getAppMeta() const {
    return app_;
}

void EditDialog::setupUi() {
    editPath_ = new QLineEdit(app_.path);
    editParameter_ = new QLineEdit(app_.parameter);
    editName_ = new QLineEdit(app_.name);
    editTriggerKey_ = new QLineEdit(app_.triggerKey);

    chkRunAsAdmin_ = new QCheckBox(tr("Run as administrator"));
    chkRunAsAdmin_->setCheckState(app_.runAsAdmin ? Qt::Checked : Qt::Unchecked);

    btnOK_ = new QPushButton(tr("OK"));
    btnCancel_ = new QPushButton(tr("Cancel"));

    btnBrowserExe_ = new QPushButton(QIcon(":/images/exe.png"), "");
    btnBrowserFolder_ = new QPushButton(QIcon(":/images/folder.png"), "");

    auto root = VBox(
        HBox(new QLabel(tr("Application, URL or Folder:")), Stretch()),
        HBox(new QLabel(tr("(Support relative path, If PortableStarter is installed in X:\\PortableStarter, \nyou can use ..\\prog\\prog.exe to start the application from X:\\prog\\prog.exe)")), Stretch()),
        HBox(editPath_, btnBrowserExe_, btnBrowserFolder_),
        Spacing(10),
        HBox(new QLabel(tr("Parameter:")), Stretch()),
        editParameter_,
        Spacing(10),
        HBox(new QLabel(tr("Name:")), Stretch()),
        editName_,
        Spacing(10),
        HBox(new QLabel(tr("Trigger Keyword (Split multiple keywords with commas):")), Stretch()),
        editTriggerKey_,
        Spacing(10),
#ifdef Q_OS_WINDOWS
        HBox(chkRunAsAdmin_, Stretch()),
#endif
        Stretch(),
        HBox(Stretch(), btnOK_, btnCancel_, Stretch()));
    this->setLayout(root);
}
