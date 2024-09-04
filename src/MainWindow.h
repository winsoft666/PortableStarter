#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QMenu>
#include <QTabBar>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include "QHotkey"
#include "AppDef.h"
#include "AppModel.h"

class MainWindow : public QWidget {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool setQuickStartHotkey(QString hotkey);

   protected:
    void setupUi();
    void bindSignals();

    void closeEvent(QCloseEvent* e) override;
    void showEvent(QShowEvent* e) override;
    void moveEvent(QMoveEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e);

    bool runApp(const QSharedPointer<AppMeta>& app, bool forceAdmin = false);

   protected slots:
    void selectFirstRow();
    void recordWindowGeometry();
    void reloadCategoryTab();
   protected:
    bool allowExit_ = false;
    QHotkey* qucikStartHotkey_ = nullptr;
    QMenu* trayMenu_ = nullptr;
    QSystemTrayIcon* trayIcon_ = nullptr;
    QLineEdit* editSearch_ = nullptr;
    QPushButton* btnNew_ = nullptr;
    QTabBar* tabBar_ = nullptr;
    QListView* listApp_ = nullptr;
    AppModel* appModel_ = nullptr;
};
#endif  // MAINWINDOW_H
