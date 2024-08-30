#pragma once
#include <QSettings>
#include <QList>

QSettings& GetSettings();

void SetStartWithOS(bool startAutomatically);

bool IsUrl(const QString& str);

bool GetStringLetters(const QString& string, QList<QString>& allLetters);