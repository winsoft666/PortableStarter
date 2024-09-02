#ifndef OPTIONS_DIALOG_H_
#define OPTIONS_DIALOG_H_

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QKeySequenceEdit>

class MainWindow;
class OptionsDialog : public QDialog {
    Q_OBJECT
   public:
    OptionsDialog(MainWindow* parent = nullptr);

   protected:
    void setupUi();

   protected:
    QCheckBox* chkStartWithOS_ = nullptr;
    QCheckBox* chkRememberPosSize_ = nullptr;
    QCheckBox* chkShowPathAndParam_ = nullptr;
    QKeySequenceEdit* keyEditQuickStart_ = nullptr;
    QPushButton* btnOK_ = nullptr;
};
#endif  // !OPTIONS_DIALOG_H_
