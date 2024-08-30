#pragma once
#include <QPixmap>

struct AppMeta {
    bool runAsAdmin = false;
    QString path;
    QString parameter;
    QString name;
    QString triggerKey;
    QPixmap icon;
};
