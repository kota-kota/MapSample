#ifndef INCLUDED_TYPE_HPP
#define INCLUDED_TYPE_HPP

#include <cstdint>

namespace app {
    //型定義
    using Char8 = char;
    using Char16 = char16_t;
    using Char32 = char32_t;
    using Int8 = std::int8_t;
    using UInt8 = std::uint8_t;
    using Int16 = std::int16_t;
    using UInt16 = std::uint16_t;
    using Int32 = std::int32_t;
    using UInt32 = std::uint32_t;
    using Int64 = std::int64_t;
    using UInt64 = std::uint64_t;
    using Float = float;
    using Double = double;

    //リターンコード
    enum ReturnCode : Int8 {
        NG_ERROR = -1,
        OK = 0,
    };
}

#endif //INCLUDED_TYPE_HPP
