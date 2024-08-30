#pragma once
#include <QSettings>

QSettings& GetSettings();

void SetStartWithOS(bool startAutomatically);

bool IsUrl(const QString& str);
