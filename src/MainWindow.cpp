#include "MainWindow.h"
#include "DSLLayout.hpp"
#include "AppVersion.h"
#include <QMessageBox>
#include <QDesktopServices>
#include "EditDialog.h"
#include "OptionsDialog.h"
#include "AppModel.h"
#include "AppDelegate.h"
#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#ifdef Q_OS_WINDOWS
#include <Windows.h>
#include <shellapi.h>
#endif
#include "Helper.h"

using namespace tk;

MainWindow::MainWindow(QWidget* parent) :
    QWidget(parent) {
    setupUi();
    bindSignals();

    listApp_->installEventFilter(this);
    editSearch_->installEventFilter(this);

    QString quickStartHotkey = GetSettings().value("QuickStartHotkey").toString();
    if (!quickStartHotkey.isEmpty()) {
        if (!qucikStartHotkey_->setShortcut(quickStartHotkey, true)) {
            QMessageBox::critical(this, "PortableStarter", tr("Failed to register quick start hotkey!"));
        }
    }

    selectFirstRow();
}

MainWindow::~MainWindow() {}

bool MainWindow::setQuickStartHotkey(QString hotkey) {
    return qucikStartHotkey_->setShortcut(hotkey, true);
}

void MainWindow::setupUi() {
    setWindowTitle(QString("PortableStarter %1").arg(APP_VERSION));

    trayMenu_ = new QMenu();
    trayMenu_->addAction(QString("PortableStarter %1").arg(APP_VERSION), [this]() { this->show(); });
    trayMenu_->addAction(tr("Options"), [this]() {
        OptionsDialog* dlg = new OptionsDialog(this);
        dlg->open();
    });
    trayMenu_->addAction(tr("Homepage"), [this]() { QDesktopServices::openUrl(QUrl("https://github.com/winsoft666/PortableStarter/")); });
    trayMenu_->addAction(tr("Exit"), [this]() {
        if (QMessageBox::question(this, "PortableStarter", tr("Exit Portable Starter?")) == QMessageBox::Yes) {
            allowExit_ = true;
            this->close();
        }
    });

    trayIcon_ = new QSystemTrayIcon();
    trayIcon_->setIcon(QIcon(":/images/logo.png"));
    trayIcon_->setToolTip("Portable Starter");
    trayIcon_->setContextMenu(trayMenu_);
    trayIcon_->show();

    editSearch_ = new QLineEdit();
    editSearch_->setObjectName("editSearch");
    editSearch_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editSearch_->setFixedHeight(28);
    editSearch_->setPlaceholderText(tr("Search..."));

    appModel_ = new AppModel();

    listApp_ = new QListView();
    listApp_->setObjectName("listApp");
    listApp_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listApp_->setSelectionBehavior(QAbstractItemView::SelectRows);
    listApp_->setSelectionMode(QAbstractItemView::SingleSelection);
    listApp_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listApp_->setItemDelegate(new AppDelegate());
    listApp_->setModel(appModel_);
    listApp_->setContextMenuPolicy(Qt::CustomContextMenu);

    btnNew_ = new QPushButton(QIcon(":/images/new.png"), "");
    btnNew_->setObjectName("btnNew");
    btnNew_->setShortcut(QKeySequence("Ctrl+N"));

    auto root = VBox(
        HBox(editSearch_, btnNew_),
        listApp_);
    this->setLayout(root);

    qucikStartHotkey_ = new QHotkey(this);
}

void MainWindow::bindSignals() {
    connect(trayIcon_, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger)
            this->show();
    });

    connect(editSearch_, &QLineEdit::textChanged, this, [this](const QString& text) {
        appModel_->setFilter(text);
    });

    connect(btnNew_, &QPushButton::clicked, this, [this]() {
        EditDialog* dlg = new EditDialog(nullptr, this);
        connect(dlg, &QDialog::finished, this, [dlg, this](int result) {
            if (result == 100) {
                AppMeta app = dlg->getAppMeta();
                appModel_->appendApp(app);
            }

            dlg->deleteLater();
        });
        dlg->open();
    });

    // QListView
    //
    connect(appModel_, &QAbstractItemModel::modelReset, this, &MainWindow::selectFirstRow);

    connect(listApp_, &QListView::customContextMenuRequested, this, [this](const QPoint& pt) {
        QMenu* popMenu = new QMenu(this);
        popMenu->addAction(tr("Run"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                AppMeta app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                if (runApp(app)) {
                    this->hide();
                }
            }
        });

#ifdef Q_OS_WINDOWS
        popMenu->addAction(tr("Run as administrator (&R)"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                AppMeta app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                app.runAsAdmin = true;
                if (runApp(app)) {
                    this->hide();
                }
            }
        });
#else
#endif
        popMenu->addAction(tr("Edit (&E)"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                AppMeta app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                EditDialog* dlg = new EditDialog(&app, this);
                connect(dlg, &QDialog::finished, this, [dlg, this](int result) {
                    if (result == 100) {
                        AppMeta app = dlg->getAppMeta();
                        appModel_->updateApp(app);
                    }
                });
                dlg->open();
            }
        });

        popMenu->addAction(tr("Delete (&D)"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                AppMeta app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                appModel_->removeApp(app);
            }
        });

        popMenu->exec(QCursor::pos());
        popMenu->deleteLater();
    });

    connect(qucikStartHotkey_, &QHotkey::activated, this, [this]() {
        if (isVisible()) {
            editSearch_->setFocus();
        }
        else {
            this->show();
        }
    });
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if (allowExit_) {
        qApp->quit();
    }
    else {
        e->ignore();
        this->hide();
    }
}

void MainWindow::showEvent(QShowEvent* e) {
    editSearch_->setFocus();
    QWidget::showEvent(e);
}

void MainWindow::moveEvent(QMoveEvent* e) {
    recordWindowGeometry();
    QWidget::moveEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent* e) {
    recordWindowGeometry();
    QWidget::resizeEvent(e);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* e) {
    if (obj != editSearch_ && obj != listApp_) {
        return false;
    }

    QEvent::Type t = e->type();
    if (t == QEvent::KeyPress) {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
        int key = keyEvent->key();
        switch (key) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if (listApp_->selectionModel()->hasSelection()) {
                    if (const auto pModel = dynamic_cast<AppModel*>(listApp_->selectionModel()->model())) {
                        AppMeta app = pModel->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                        if (runApp(app)) {
                            this->hide();
                        }
                    }
                }
                break;
            case Qt::Key_Escape:
                editSearch_->clear();
                this->hide();
                break;
            case Qt::Key_Up:
            case Qt::Key_Down:
                const int rowCount = listApp_->model()->rowCount();
                if (rowCount > 0) {
                    int curSelectedRow = 0;
                    if (listApp_->selectionModel()->hasSelection()) {
                        curSelectedRow = listApp_->selectionModel()->selectedRows()[0].row();  // single selection
                        if (key == Qt::Key_Down) {
                            curSelectedRow += keyEvent->count();
                            if (curSelectedRow > rowCount - 1)
                                curSelectedRow = 0;
                        }
                        else {
                            curSelectedRow -= keyEvent->count();
                            if (curSelectedRow < 0)
                                curSelectedRow = rowCount - 1;
                        }
                    }

                    listApp_->setCurrentIndex(listApp_->model()->index(curSelectedRow, 0));
                }
                break;
        }
    }
    return false;
}

bool MainWindow::runApp(const AppMeta& app) {
    bool result = false;
    bool isUrl = false;
    QString path;

    if (IsUrl(app.path)) {
        isUrl = true;
        path = app.path;
    }
    else {
        if (QDir::isRelativePath(app.path)) {
            path = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + QString(QDir::separator()) + QDir::toNativeSeparators(app.path);
        }
        else {
            path = app.path;
        }
    }

    if (isUrl) {
        result = QDesktopServices::openUrl(QUrl(path));
    }
    else {
#ifdef Q_OS_WINDOWS
        if (app.runAsAdmin) {
            result = (INT_PTR)(::ShellExecuteW(
                         nullptr,
                         L"runas",
                         path.toStdWString().c_str(),
                         app.parameter.toStdWString().c_str(),
                         L"",
                         SW_SHOWDEFAULT)) > 31;
        }
        else {
            QProcess proc;
            proc.setProgram(path);
            proc.setNativeArguments(app.parameter);
            //proc.setWorkingDirectory();
            result = proc.startDetached();
        }
#else
        QProcess proc;
        proc.setProgram(path);
        proc.setNativeArguments(app.parameter);
        //proc.setWorkingDirectory();
        result = proc.startDetached();
#endif
    }

    if (!result) {
        QMessageBox::critical(this, "PortableStarter", tr("Unable to run application (%1 %2).").arg(path).arg(app.parameter));
    }

    return result;
}

void MainWindow::selectFirstRow() {
    const int rowCount = listApp_->model()->rowCount();
    if (rowCount > 0) {
        listApp_->setCurrentIndex(listApp_->model()->index(0, 0));
    }
}

void MainWindow::recordWindowGeometry() {
    QSettings& settings = GetSettings();
    if (settings.value("RememberWindowPosAndSize").toInt() == 1) {
        settings.setValue("WindowGeometry", this->geometry());
    }
}