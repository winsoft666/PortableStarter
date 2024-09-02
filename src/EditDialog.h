#ifndef EDIT_DIALOG_H_
#define EDIT_DIALOG_H_

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include "AppDef.h"

class EditDialog : public QDialog {
    Q_OBJECT
   public:
    EditDialog(const QSharedPointer<AppMeta> app, QWidget* parent = nullptr);
    ~EditDialog();

    QSharedPointer<AppMeta> getAppMeta() const;

   protected:
    void setupUi();

   protected:
    QSharedPointer<AppMeta> app_ = nullptr;
    QLineEdit* editPath_ = nullptr;
    QLineEdit* editParameter_ = nullptr;
    QLineEdit* editName_ = nullptr;
    QLineEdit* editTriggerKey_ = nullptr;
    QCheckBox* chkRunAsAdmin_ = nullptr;

    QPushButton* btnToRelativePath_ = nullptr;
    QPushButton* btnBrowserExe_ = nullptr;
    QPushButton* btnBrowserFolder_ = nullptr;

    QPushButton* btnOK_ = nullptr;
    QPushButton* btnCancel_ = nullptr;
};
#endif  // !EDIT_DIALOG_H_
