#include "AppDelegate.h"
#include <QPainter>
#include "Helper.h"

AppDelegate::AppDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent) {
}

void AppDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSettings& settings = GetSettings();
    //QPalette palette = option.widget->palette();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::LosslessImageRendering, true);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor(204, 232, 255, 255));
    }
    else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor(232, 232, 232, 255));
    }
    else {
        //painter->fillRect(option.rect, palette.color(QPalette::Window));
    }

    QRect contentRect = option.rect.marginsRemoved(QMargins(2, 0, 2, 0));

    QRect iconRect(contentRect.left(), contentRect.top(), contentRect.height(), contentRect.height());
    QPixmap icon = index.data(Qt::UserRole + 1).value<QPixmap>();
    painter->drawPixmap(iconRect, icon, QRect(QPoint(0, 0), icon.size()));

    QRect txtRect = contentRect.marginsRemoved(QMargins(contentRect.left() + contentRect.height() + 4, 0, 0, 0));
    QString text = index.data(Qt::UserRole + 2).toString();

    QRect boundingRect;
    painter->drawText(txtRect, Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::UserRole + 2).toString(), &boundingRect);

    if (GetSettings().value("ShowPathAndParameter", SETTING_DEFAULT_SHOW_PATH_AND_PARAMETER).toInt() == 1) {
        txtRect = txtRect.marginsRemoved(QMargins(boundingRect.width() + 10, 0,0,0));

        painter->save();

        QFont font = painter->font();
        if (font.pixelSize() != -1) {
            font.setPixelSize(font.pixelSize() * 0.8);
        }
        else {
            font.setPointSizeF(font.pointSizeF() * 0.8);
        }
        painter->setFont(font);

        QPen pen = painter->pen();
        QColor txtColor = pen.color();
        txtColor.setAlpha(160);
        pen.setColor(txtColor);
        painter->setPen(pen);

        painter->drawText(txtRect,  Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::UserRole + 3).toString());

        painter->restore();
    }
}

QSize AppDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QSize(option.rect.width(), 22);
}
