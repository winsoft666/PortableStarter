#include "MainWindow.h"
#include "DSLLayout.hpp"
#include "AppVersion.h"
#include <stdlib.h>
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
    setWindowTitle("PortableStarter");

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
    trayIcon_->setIcon(QIcon(":/images/logo.ico"));
    trayIcon_->setToolTip("Portable Starter");
    trayIcon_->setContextMenu(trayMenu_);
    trayIcon_->show();

    editSearch_ = new QLineEdit();
    editSearch_->setObjectName("editSearch");
    editSearch_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editSearch_->setFixedHeight(24);
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
                QSharedPointer<AppMeta> app = dlg->getAppMeta();
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
                QSharedPointer<AppMeta> app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                Q_ASSERT(app);
                if (runApp(app, false)) {
                    editSearch_->clear();
                    this->hide();
                }
            }
        });

#ifdef Q_OS_WINDOWS
        popMenu->addAction(tr("Run as administrator (&R)"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                QSharedPointer<AppMeta> app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                Q_ASSERT(app);
                if (runApp(app, true)) {
                    editSearch_->clear();
                    this->hide();
                }
            }
        });
#else
#endif
        popMenu->addAction(tr("Edit (&E)"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                QSharedPointer<AppMeta> app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                Q_ASSERT(app);
                EditDialog* dlg = new EditDialog(app, this);
                connect(dlg, &QDialog::finished, this, [dlg, this](int result) {
                    if (result == 100) {
                        appModel_->flush();
                    }
                });
                dlg->open();
            }
        });

        popMenu->addAction(tr("Delete (&D)"), [this]() {
            if (listApp_->selectionModel()->hasSelection()) {
                QSharedPointer<AppMeta> app = appModel_->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                Q_ASSERT(app);
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
                        QSharedPointer<AppMeta> app = pModel->getApp(listApp_->selectionModel()->selectedRows()[0].row());
                        if (runApp(app, false)) {
                            editSearch_->clear();
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

bool MainWindow::runApp(const QSharedPointer<AppMeta>& app, bool forceAdmin) {
    bool result = false;
    QString path = app->path;
    QString param = app->parameter;

    if (IsUrl(path)) {
        result = QDesktopServices::openUrl(QUrl(path));
    }
    else {
        QFileInfo fi(path);
        if (fi.isDir()) {
            result = QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(path)));
        }
        else {
            if (app->cmdTool) {
                QString appPath;
                if (QDir::isRelativePath(path)) {
                    QDir dir(QCoreApplication::applicationDirPath());
                    appPath = QDir::toNativeSeparators(QDir::cleanPath(dir.absoluteFilePath(path)));
                }
                else {
                    appPath = path;
                }
#ifdef Q_OS_WINDOWS
                path = QString("C:\\Windows\\System32\\cmd.exe");
                param = QString("/K CD /D %1 & DIR").arg(appPath.mid(0, appPath.lastIndexOf("\\")));

                result = (INT_PTR)(::ShellExecuteW(
                             nullptr,
                             (forceAdmin || app->runAsAdmin) ? L"runas" : L"open",
                             path.toStdWString().c_str(),
                             param.toStdWString().c_str(),
                             L"",
                             SW_SHOWDEFAULT)) > 31;
#else
                // TODO
#endif
            }
            else {
                if (QDir::isRelativePath(path)) {
                    QDir dir(QCoreApplication::applicationDirPath());
                    path = QDir::toNativeSeparators(QDir::cleanPath(dir.absoluteFilePath(path)));
                }

#ifdef Q_OS_WINDOWS
                if (forceAdmin || app->runAsAdmin) {
                    result = (INT_PTR)(::ShellExecuteW(
                                 nullptr,
                                 L"runas",
                                 path.toStdWString().c_str(),
                                 param.toStdWString().c_str(),
                                 L"",
                                 SW_SHOWDEFAULT)) > 31;
                }
                else {
                    QProcess proc;
                    proc.setProgram(path);
                    proc.setNativeArguments(param);
                    proc.setWorkingDirectory(path.mid(0, path.lastIndexOf(QDir::separator())));
                    result = proc.startDetached();
                }
#else
                QProcess proc;
                proc.setProgram(path);
                proc.setNativeArguments(param);
                proc.setWorkingDirectory(path.mid(0, path.lastIndexOf(QDir::separator())));
                result = proc.startDetached();
#endif
            }
        }
    }

    if (!result) {
        QMessageBox::critical(this,
                              "PortableStarter",
                              tr("Unable to start %1%2%3.").arg(path).arg(param.isEmpty() ? "" : " ").arg(param));
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