#ifndef QS_PINYIN_H__
#define QS_PINYIN_H__

#include <string>
#include <vector>

class Pinyin {
   public:
    static bool IsChinese(wchar_t chr);
    static bool GetPinyins(wchar_t chr, std::vector<std::wstring>& out);

    /// <summary>
    /// The minimum integer value of a valid Chinese character.
    /// The maximum integer value of a valid Chinese character.
    /// [12295, 12295]、 [19968, 40869]和 [59418, 59491]
    /// </summary>
   private:
    static const int32_t MinValue = 12295;
    static const int32_t Part1MinValue = 19968;
    static const int32_t Part1MaxValue = 40869;
    static const int32_t Part2MinValue = 59418;
    static const int32_t Part2MaxValue = 59491;

    static const wchar_t* PinyinTable[];
    static const int64_t PinyinCodes[];
    static const int64_t PinyinPart1[];
    static const int64_t PinyinPart2[];

    static const int64_t ling = 175;

    static std::vector<std::wstring> DecodePinyins(int64_t pinyinCode);
};

#endif  // !QS_PINYIN_H__
