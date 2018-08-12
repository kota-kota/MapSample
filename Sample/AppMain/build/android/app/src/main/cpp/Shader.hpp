#ifndef INCLUDED_SHADER_HPP
#define INCLUDED_SHADER_HPP

#include "Type.hpp"

namespace app {

    class Shader {
        //メンバ変数
        UInt32 programId_;

    public:
        //コンストラクタ
        Shader();
        //デストラクタ
        ~Shader();
        //シェーダを作成する
        ReturnCode create(const Char8* const vertex, const Char8* const fragment);
        //プログラムIDを取得する
        UInt32 getProgramId();
        //Attribute変数を取得する
        UInt32 getAttrLocation(const Char8* const attr);
        //Uniform変数を取得する
        UInt32 getUniformLocation(const Char8* const uniform);
    };
}

#endif //INCLUDED_SHADER_HPP
