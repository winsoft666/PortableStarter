#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QCloseEvent>

class MainWindow : public QWidget {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:
    void setupUi();
    void bindSignals();

    void closeEvent(QCloseEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e);
   protected:
    bool allowExit_ = false;
    QMenu* trayMenu_ = nullptr;
    QSystemTrayIcon* trayIcon_ = nullptr;
    QLineEdit* editSearch_ = nullptr;
    QPushButton* btnNew_ = nullptr;
    QListView* listApp_ = nullptr;
};
#endif  // MAINWINDOW_H
