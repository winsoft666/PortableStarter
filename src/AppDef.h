#ifndef APP_DEF_H_
#define APP_DEF_H_
#pragma once
#include <QPixmap>

struct AppMeta {
    QString id;
    bool runAsAdmin = false;
    QString path;
    QString parameter;
    QString name;
    QString triggerKey;
    QPixmap icon;
};
#endif  // !APP_DEF_H_
