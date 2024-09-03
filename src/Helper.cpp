#include "Helper.h"
#include <QDir>
#include <QCoreApplication>
#include "Pinyin.h"
#include <QBuffer>

namespace {
#ifdef Q_OS_MAC
QString macOSXAppBundlePath() {
    QDir dir = QDir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    QString absolutePath = dir.absolutePath();
    // absolutePath will contain a "/" at the end,
    // but we want the clean path to the .app bundle
    if (absolutePath.length() > 0 && absolutePath.right(1) == "/") {
        absolutePath.chop(1);
    }
    return absolutePath;
}

QString macOSXAppBundleName() {
    QString bundlePath = macOSXAppBundlePath();
    QFileInfo fileInfo(bundlePath);
    return fileInfo.baseName();
}
#endif  // Q_OS_MAC
}  // namespace

QSettings& GetSettings() {
    QString iniPath = QCoreApplication::applicationDirPath() + "/config.ini";
    static QSettings settings(iniPath, QSettings::IniFormat);
    return settings;
}

void SetStartWithOS(bool startAutomatically) {
#if defined(Q_OS_MAC)

    // Remove any existing login entry for this app first, in case there was one
    // from a previous installation, that may be under a different launch path.
    {
        QStringList args;
        args << "-e tell application \"System Events\" to delete login item\"" + macOSXAppBundleName() + "\"";

        QProcess::execute("osascript", args);
    }

    // Now install the login item, if needed.
    if (startAutomatically) {
        QStringList args;
        args << "-e tell application \"System Events\" to make login item at end " +
                    "with properties {path:\"" + macOSXAppBundlePath() + "\", hidden:false}";

        QProcess::execute("osascript", args);
    }

#elif defined(Q_OS_WINDOWS)

    QString key = "PortableStarter";

    QSettings registrySettings(
        "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        QSettings::NativeFormat);

    registrySettings.remove(key);

    if (startAutomatically) {
        registrySettings.setValue(key, QString("\"" + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + "\""));
    }

    registrySettings.sync();

#endif
}

bool IsUrl(const QString& str) {
    return str.startsWith("http://", Qt::CaseInsensitive) || str.startsWith("https://", Qt::CaseInsensitive);
}

bool GetStringLetters(const QString& string, QList<QString>& allLetters) {
    allLetters.clear();

    if (string.isEmpty()) {
        return true;
    }

    std::wstring str = string.toStdWString();
    std::vector<std::vector<std::wstring>> vAllFullPinYin;
    std::vector<std::vector<std::wstring>> vLetters;

    wchar_t preChar = L'\0';
    for (size_t i = 0; i < str.size(); i++) {
        const wchar_t wc = str[i];

        if (Pinyin::IsChinese(wc)) {  // chinese
            std::vector<std::wstring> py;
            if (!Pinyin::GetPinyins(wc, py)) {
                return false;
            }
            vAllFullPinYin.push_back(py);

            std::vector<std::wstring> letters;
            for (const auto& s : py) {
                letters.push_back(s.substr(0, 1));
            }
            vLetters.push_back(letters);
        }
        else {
            std::wstring strwc(1, wc);
            vAllFullPinYin.push_back({strwc});

            if (wc <= L'Z' && wc >= L'A') {
                vLetters.push_back({strwc});
            }
            else if (wc <= L'9' && wc >= L'0') {
                vLetters.push_back({strwc});
            }
            else if (preChar == L'\0' || preChar == L' ' || preChar == L',' ||
                     preChar == L'-' || preChar == L'_' || preChar == L'+' ||
                     preChar == L'=' || preChar == L'&' || preChar == L'#' ||
                     preChar == L'@' || preChar == L'!' || preChar == L'~' ||
                     preChar == L'.') {
                vLetters.push_back({strwc});
            }
        }

        preChar = wc;
    }

    std::wstring fullPY;
    for (const auto& it1 : vAllFullPinYin) {
        fullPY += it1[0];
    }
    allLetters.push_back(QString::fromStdWString(fullPY));

    if (vLetters.size() > 1) {
        std::wstring letters;
        for (const auto& it1 : vLetters) {
            letters += it1[0];
        }
        allLetters.push_back(QString::fromStdWString(letters));
    }

    return true;
}

QByteArray PixmapToByteArray(const QPixmap& pix) {
    QByteArray arr;
    if (!pix.isNull()) {
        QBuffer iconBuffer(&arr);
        iconBuffer.open(QIODevice::WriteOnly);
        if (!pix.save(&iconBuffer, "png")) {
            pix.save(&iconBuffer, "bmp");
        }
    }

    return arr;
}

QPixmap ByteArrayToPixmap(const QByteArray& arr) {
    QPixmap pix;
    if (!pix.loadFromData(arr, "png")) {
        pix.loadFromData(arr, "bmp");
    }
    return pix;
}
