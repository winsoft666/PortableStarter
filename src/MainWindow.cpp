#include "MainWindow.h"
#include "DSLLayout.hpp"
#include "AppVersion.h"
#include <QMessageBox>
#include <QDesktopServices>
#include "EditDialog.h"
#include "AppModel.h"
#include "AppDelegate.h"
#include <QProcess>
#ifdef Q_OS_WINDOWS
#include <Windows.h>
#include <shellapi.h>
#endif

using namespace tk;

MainWindow::MainWindow(QWidget* parent) :
    QWidget(parent) {
    resize(600, 300);

    setupUi();
    bindSignals();

    this->installEventFilter(this);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    trayMenu_ = new QMenu();
    trayMenu_->addAction(tr("PortableStarter %1").arg(APP_VERSION), [this]() { this->show(); });
    trayMenu_->addAction(tr("Options"), []() {
    });
    trayMenu_->addAction(tr("Homepage"), [this]() { QDesktopServices::openUrl(QUrl("https://github.com/winsoft666/")); });
    trayMenu_->addAction(tr("Exit"), [this]() {
        if (QMessageBox::question(this, "PortableStarter", tr("Exit Portable Starter?")) == QMessageBox::Yes) {
            allowExit_ = true;
            close();
        }
    });

    trayIcon_ = new QSystemTrayIcon();
    trayIcon_->setIcon(QIcon(":/images/logo.png"));
    trayIcon_->setContextMenu(trayMenu_);
    trayIcon_->show();

    editSearch_ = new QLineEdit();
    editSearch_->setObjectName("editSearch");
    editSearch_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editSearch_->setFixedHeight(26);
    editSearch_->setPlaceholderText(tr("Search..."));

    listApp_ = new QListView();
    listApp_->setObjectName("listApp");
    listApp_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listApp_->setSelectionBehavior(QAbstractItemView::SelectRows);
    listApp_->setSelectionMode(QAbstractItemView::SingleSelection);
    listApp_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listApp_->setItemDelegate(new AppDelegate());
    listApp_->setModel(new AppModel());
    listApp_->setContextMenuPolicy(Qt::CustomContextMenu);

    btnNew_ = new QPushButton(tr("New"));
    btnNew_->setObjectName("btnNew");

    auto root = VBox(
        HBox(editSearch_, btnNew_),
        listApp_);
    this->setLayout(root);
}

void MainWindow::bindSignals() {
    connect(trayIcon_, &QSystemTrayIcon::activated, this, [this]() { this->show(); });

    connect(btnNew_, &QPushButton::clicked, this, [this]() {
        EditDialog* dlg = new EditDialog(nullptr, this);
        connect(dlg, &QDialog::finished, this, [dlg, this](int result) {
            if (result == 100) {
                AppMeta app = dlg->getAppMeta();
                AppModel* appModel = dynamic_cast<AppModel*>(listApp_->model());
                if (appModel) {
                    appModel->addApp(app);
                }
            }

            dlg->deleteLater();
        });
        dlg->open();
    });

    connect(listApp_, &QListView::customContextMenuRequested, this, [this](const QPoint& pt) {
        QMenu* popMenu = new QMenu(this);
        popMenu->addAction(tr("Edit"), [this]() {});
        popMenu->addAction(tr("Delete"), [this]() {
            EditDialog* dlg = new EditDialog(nullptr, this);
            dlg->open();
        });
        popMenu->exec(QCursor::pos());
        popMenu->deleteLater();
    });
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if (allowExit_) {
        e->accept();
    }
    else {
        e->ignore();
        this->hide();
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* e) {
    if (obj == listApp_) {
        if (e->type() == QEvent::KeyRelease) {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                if (listApp_->selectionModel()->hasSelection()) {
                    if (const auto pModel = dynamic_cast<AppModel*>(listApp_->selectionModel()->model())) {
                        AppMeta app = pModel->getApp(listApp_->selectionModel()->selectedRows()[0].row());
#ifdef Q_OS_WINDOWS
                        if (app.runAsAdmin) {
                            bool result = (INT_PTR)(::ShellExecuteW(
                                              nullptr,
                                              L"runas",
                                              app.path.toStdWString().c_str(),
                                              app.parameter.toStdWString().c_str(),
                                              L"",
                                              SW_SHOWDEFAULT)) > 31;
                        }
                        else {
                            QProcess proc;
                            proc.setProgram(app.path);
                            proc.setNativeArguments(app.parameter);
                            //proc.setWorkingDirectory();
                            proc.startDetached();
                        }
#else
#endif
                    }
                }
            }
        }
    }
    else {
        if (e->type() == QEvent::KeyRelease) {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
            if (keyEvent->key() == Qt::Key_Escape) {
                this->hide();
            }
        }
    }

    return false;
}
