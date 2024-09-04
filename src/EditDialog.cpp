#include "EditDialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QCoreApplication>
#include "Helper.h"
#include "DSLLayout.hpp"
#include "CategoryEditDialog.h"

using namespace tk;

EditDialog::EditDialog(const QSharedPointer<AppMeta> app, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    app_(app) {
    setupUi();

    this->setWindowTitle(app_ == nullptr ? tr("Add") : tr("Edit"));
    this->resize(520, 420);

    connect(btnCancel_, &QPushButton::clicked, this, [this]() { done(0); });

    connect(btnOK_, &QPushButton::clicked, this, [this]() {
        if (!app_) {
            app_ = QSharedPointer<AppMeta>::create();
        }

        app_->category = cmbCategory_->currentText();
        app_->path = editPath_->text().trimmed();
        app_->parameter = editParameter_->text().trimmed();
        app_->name = editName_->text().trimmed();
        app_->triggerKey = editTriggerKey_->text().trimmed();
        app_->runAsAdmin = chkRunAsAdmin_->isChecked() ? 1 : 0;
        app_->cmdTool = chkIsCmdTool_->isChecked() ? 1 : 0;

        if (app_->path.isEmpty() || app_->name.isEmpty()) {
            done(0);
            return;
        }

        if (IsUrl(app_->path)) {
            app_->icon = QPixmap(":/images/website.png");
        }
        else {
            QString path;
            if (QDir::isRelativePath(app_->path)) {
                QDir dir(QCoreApplication::applicationDirPath());
                path = QDir::toNativeSeparators(QDir::cleanPath(dir.absoluteFilePath(app_->path)));
            }
            else {
                path = app_->path;
            }

            QFileInfo fi(path);
            if (fi.isDir()) {
                app_->icon = QPixmap(":/images/folder.png");
            }
            else if (fi.isFile()) {
                QFileIconProvider iconProvider;
                QIcon ico = iconProvider.icon(fi);
                if (!ico.isNull()) {
                    app_->icon = ico.pixmap(48, 48);
                }

                if (app_->icon.isNull()) {
                    app_->icon = QPixmap(":/images/exe.png");
                }
            }
            else {
                Q_ASSERT(false);
            }
        }

        done(100);
    });

    connect(btnEditCategory_, &QPushButton::clicked, this, [this]() {
        CategoryEditDialog* dlg = new CategoryEditDialog(this);
        connect(dlg, &QDialog::finished, this, [this](int result) {
            if (result == 100) {
                cmbCategory_->clear();
                cmbCategory_->addItems(GetSettings().value("Category").toStringList());
            }
        });
        dlg->open();
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
        if (!strPath.isEmpty()) {
            editPath_->setText(QDir::toNativeSeparators(strPath));
        }
    });

    connect(btnToRelativePath_, &QPushButton::clicked, this, [this]() {
        QString strPath = editPath_->text().trimmed();
        if (!QDir::isRelativePath(strPath)) {
            QDir dir(QCoreApplication::applicationDirPath());
            editPath_->setText(QDir::toNativeSeparators(dir.relativeFilePath(strPath)));
        }
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

QSharedPointer<AppMeta> EditDialog::getAppMeta() const {
    return app_;
}

void EditDialog::setupUi() {
    cmbCategory_ = new QComboBox();
    cmbCategory_->setMinimumWidth(180);
    QStringList categories = GetSettings().value("Category").toStringList();
    categories.push_front("");
    cmbCategory_->addItems(categories);
    cmbCategory_->setCurrentText(app_ ? app_->category : "");

    btnEditCategory_ = new QPushButton(tr("Edit"));

    editPath_ = new QLineEdit(app_ ? app_->path : "");
    editParameter_ = new QLineEdit(app_ ? app_->parameter : "");
    editName_ = new QLineEdit(app_ ? app_->name : "");
    editTriggerKey_ = new QLineEdit(app_ ? app_->triggerKey : "");

    chkRunAsAdmin_ = new QCheckBox(tr("Run as administrator"));
    chkRunAsAdmin_->setCheckState((app_ && app_->runAsAdmin) ? Qt::Checked : Qt::Unchecked);

    chkIsCmdTool_ = new QCheckBox(tr("Run as command line tool"));
    chkIsCmdTool_->setCheckState((app_ && app_->cmdTool) ? Qt::Checked : Qt::Unchecked);

    btnOK_ = new QPushButton(tr("OK"));
    btnCancel_ = new QPushButton(tr("Cancel"));

    btnBrowserExe_ = new QPushButton(QIcon(":/images/exe.png"), "");
    btnBrowserFolder_ = new QPushButton(QIcon(":/images/folder.png"), "");
    btnToRelativePath_ = new QPushButton(tr("Relative Path"));

    auto root = VBox(
        HBox(new QLabel(tr("Category:")), Stretch()),
        HBox(cmbCategory_, Spacing(20), btnEditCategory_, Stretch()),
        Spacing(10),
        HBox(new QLabel(tr("Application, URL or Folder:")), Stretch()),
        HBox(new QLabel(tr("    Support relative path, If PortableStarter is installed in X:\\PortableStarter, \n    you can use ..\\prog\\prog.exe to start the application from X:\\prog\\prog.exe")), Stretch()),
        HBox(editPath_, btnBrowserExe_, btnBrowserFolder_),
        HBox(Stretch(), btnToRelativePath_),
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
        HBox(chkIsCmdTool_, Stretch()),
        HBox(new QLabel(tr("    Open terminal and change working directory to where the tool is located")), Stretch()),
        Stretch(),
        HBox(Stretch(), btnOK_, btnCancel_, Stretch()));
    this->setLayout(root);
}
