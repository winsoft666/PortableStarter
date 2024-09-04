#pragma once
#include <QDialog>
#include <QPushButton>
#include <QPlainTextEdit>

class CategoryEditDialog : public QDialog {
    Q_OBJECT
   public:
    CategoryEditDialog(QWidget* parent = nullptr);
    ~CategoryEditDialog();

   protected:
    void setupUi();

   protected:
    QPlainTextEdit* edit_ = nullptr;
    QPushButton* btnOK_ = nullptr;
    QPushButton* btnCancel_ = nullptr;
};
