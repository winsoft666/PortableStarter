#ifndef APP_DELEGATE_H_
#define APP_DELEGATE_H_

#include <QStyledItemDelegate>

class AppDelegate : public QStyledItemDelegate {
    Q_OBJECT
   public:
    AppDelegate(QObject* parent = nullptr);

   protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
#endif  // !APP_DELEGATE_H_
