#include "utfstring.hpp"
#include "format.hpp"
#include "stdout.hpp"
#include "utfstringview.hpp"
#include <iostream>

int main()
{

    // Japanese: "こんにちは世界"
    auto japanese = u8"こんにちは世界"; // Byte length: 21, Code point length: 7

    // Chinese: "你好，世界"
    auto chinese = u8"你好，世界"; // Byte length: 15, Code point length: 5

    // Russian: "Привет, мир"
    auto russian = u8"Привет, мир"; // Byte length: 21, Code point length: 11

    // Arabic: "مرحبا بالعالم"
    auto arabic = u8"مرحبا بالعالم"; // Byte length: 25, Code point length: 13

    // Spanish: "¡Hola, mundo!"
    auto spanish = u8"¡Hola, mundo!"; // Byte length: 13, Code point length: 13

    // Emoji: "😀🌍"
    auto emoji = U"😀🌍"; // Byte length: 8, Code point length: 2

    // Emoji: "😀🌍"
    auto emoji1 = u"😀🌍"; // Byte length: 8, Code point length: 2

    // Emoji: "😀🌍"
    auto emoji2 = u8"😀🌍"; // Byte length: 8, Code point length: 2

    hls::PrinterSink sink(hls::Encoding::UTF8);
    // English: "Hello, World!"
    auto english = u8"The string is '{:x}'"; // Byte length: 13, Code point length: 13
    const char *str = "Test string.";
    unsigned int v = 16;
    hls::format_string_to_sink(hls::UTFStringView(english), sink, v);
}