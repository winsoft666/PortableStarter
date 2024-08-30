#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QMenu>
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
    bool eventFilter(QObject* obj, QEvent* e);

    void runApp(const AppMeta& app);

   protected slots:
    void selectFirstRow();

   protected:
    bool allowExit_ = false;
    QHotkey* qucikStartHotkey_ = nullptr;
    QMenu* trayMenu_ = nullptr;
    QSystemTrayIcon* trayIcon_ = nullptr;
    QLineEdit* editSearch_ = nullptr;
    QPushButton* btnNew_ = nullptr;
    QListView* listApp_ = nullptr;
    AppModel* appModel_ = nullptr;
};
#endif  // MAINWINDOW_H
