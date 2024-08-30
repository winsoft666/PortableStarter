#pragma once
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include "AppDef.h"

class EditDialog : public QDialog {
    Q_OBJECT
   public:
    EditDialog(const AppMeta* app, QWidget* parent = nullptr);
    ~EditDialog();

    AppMeta getAppMeta() const;

   protected:
    void setupUi();

   protected:
    AppMeta app_;
    QLineEdit* editPath_ = nullptr;
    QLineEdit* editParameter_ = nullptr;
    QLineEdit* editName_ = nullptr;
    QLineEdit* editTriggerKey_ = nullptr;
    QCheckBox* chkRunAsAdmin_ = nullptr;

    QPushButton* btnBrowserExe_ = nullptr;
    QPushButton* btnBrowserFolder_ = nullptr;

    QPushButton* btnOK_ = nullptr;
    QPushButton* btnCancel_ = nullptr;
};
