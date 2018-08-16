#ifndef INCLUDED_TYPE_HPP
#define INCLUDED_TYPE_HPP

#include <cstdint>

namespace app {
    //型定義
    typedef char Char8;
    typedef char16_t Char16;
    typedef char32_t Char32;
    typedef std::int8_t Int8;
    typedef std::uint8_t UInt8;
    typedef std::int16_t Int16;
    typedef std::uint16_t UInt16;
    typedef std::int32_t Int32;
    typedef std::uint32_t UInt32;
    typedef std::int64_t Int64;
    typedef std::uint64_t UInt64;
    typedef float Float;
    typedef double Double;

    //リターンコード
    enum ReturnCode : Int8 {
        NG_ERROR = -1,
        OK = 0,
    };
}

#endif //INCLUDED_TYPE_HPP
