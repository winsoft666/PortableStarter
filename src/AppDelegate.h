#pragma once
#include <QStyledItemDelegate>

class AppDelegate : public QStyledItemDelegate {
    Q_OBJECT
   public:
    AppDelegate(QObject* parent = nullptr);

   protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
