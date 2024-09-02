#include "AppDelegate.h"
#include <QPainter>
#include "Helper.h"

AppDelegate::AppDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent) {
}

void AppDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSettings& settings = GetSettings();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::LosslessImageRendering, true);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor(204, 232, 255, 255));
    }
    else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor(232, 232, 232, 255));
    }

    QRect contentRect = option.rect.marginsRemoved(QMargins(2, 2, 2, 2));

    QRect iconRect(contentRect.left(), contentRect.top(), contentRect.height(), contentRect.height());
    QPixmap icon = index.data(Qt::UserRole + 1).value<QPixmap>();
    painter->drawPixmap(iconRect, icon, QRect(QPoint(0, 0), icon.size()));

    QRect txtRect = contentRect.marginsRemoved(QMargins(contentRect.left() + contentRect.height() + 4, 0, 0, 0));
    QString text = index.data(Qt::UserRole + 2).toString();

    if (GetSettings().value("ShowPathAndParameter").toInt() == 1) {
        QString pathAndParam = index.data(Qt::UserRole + 3).toString();
        if (!pathAndParam.isEmpty()) {
            text.append(" ");
            text.append(pathAndParam);
        }
    }
    painter->drawText(txtRect, Qt::AlignVCenter | Qt::AlignLeft, text);
}

QSize AppDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QSize(option.rect.width(), 22);
}
