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
        //シェーダ作成チェック
        bool isCreated() const;
        //シェーダを作成する
        ReturnCode create(const Char8* const vertex, const Char8* const fragment);
        //シェーダを破棄する
        void destroy();
        //プログラムIDを取得する
        UInt32 getProgramId() const;
        //Attribute変数を取得する
        UInt32 getAttrLocation(const Char8* const attr) const;
        //Uniform変数を取得する
        UInt32 getUniformLocation(const Char8* const uniform) const;
    };
}

#endif //INCLUDED_SHADER_HPP
