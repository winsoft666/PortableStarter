#include "CategoryEditDialog.h"
#include "Helper.h"
#include "DSLLayout.hpp"
using namespace tk;

CategoryEditDialog::CategoryEditDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent) {
    setupUi();
    this->resize(260, 400);

    connect(btnCancel_, &QPushButton::clicked, this, [this]() { done(0); });

    connect(btnOK_, &QPushButton::clicked, this, [this]() {
        QString text = edit_->toPlainText().trimmed();
        GetSettings().setValue("Category", text.split("\n", Qt::SkipEmptyParts));
        done(100);
    });

    QStringList categoryList = GetSettings().value("Category").toStringList();
    edit_->setPlainText(categoryList.join("\n"));
}

CategoryEditDialog::~CategoryEditDialog() {
}

void CategoryEditDialog::setupUi() {
    setWindowTitle(tr("Edit Category"));

    edit_ = new QPlainTextEdit();
    edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    btnOK_ = new QPushButton(tr("OK"));
    btnCancel_ = new QPushButton(tr("Cancel"));

    auto root = VBox(
        edit_,
        HBox(Stretch(), btnOK_, btnCancel_, Stretch()));
    this->setLayout(root);
}
