#include "Shader.hpp"
#include "Logger.hpp"

#include <GLES3/gl3.h>

namespace app {

    //コンストラクタ
    Shader::Shader() :
            programId_(0)
    {
        LOGI("%s\n", __FUNCTION__);
    }

    //デストラクタ
    Shader::~Shader()
    {
        LOGI("%s\n", __FUNCTION__);
        this->destroy();
    }

    //シェーダ作成チェック
    bool Shader::isCreated()
    {
        bool ret = false;
        if (this->programId_ != 0) { ret = true; }
        return ret;
    }

    //シェーダを作成する
    ReturnCode Shader::create(const Char8* const vertex, const Char8* const fragment)
    {
        LOGI("%s\n", __FUNCTION__);
        ReturnCode retCode = NG_ERROR;
        GLuint	vertexId = 0;
        GLuint	fragmentId = 0;
        GLuint	programId = 0;
        GLint	retCompiled = 0;
        GLint	retLinked = 0;

        //バーテックスシェーダオブジェクト作成しコンパイル
        vertexId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexId, 1, &vertex, nullptr);
        glCompileShader(vertexId);
        glGetShaderiv(vertexId, GL_COMPILE_STATUS, &retCompiled);
        LOGD("%s glCompileShader vertexId:%d (ret:%d)\n", __FUNCTION__, vertexId, retCompiled);
        if (retCompiled == GL_FALSE) {
            goto END;
        }

        //フラグメントシェーダオブジェクト作成
        fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentId, 1, &fragment, nullptr);
        glCompileShader(fragmentId);
        glGetShaderiv(fragmentId, GL_COMPILE_STATUS, &retCompiled);
        LOGD("%s glCompileShader fragmentId:%d (ret:%d)\n", __FUNCTION__, fragmentId, retCompiled);
        if (retCompiled == GL_FALSE) {
            goto END;
        }

        //プログラムオブジェクト作成しリンク
        programId = glCreateProgram();
        glAttachShader(programId, vertexId);
        glAttachShader(programId, fragmentId);
        glLinkProgram(programId);
        glGetProgramiv(programId, GL_LINK_STATUS, &retLinked);
        LOGD("%s glLinkProgram programId:%d (ret:%d)\n", __FUNCTION__, programId, retLinked);
        if (retLinked == GL_FALSE) {
            goto END;
        }

        //成功
        //プログラムIDを保持
        this->programId_ = programId;
        retCode = OK;

        END:
        if (vertexId != 0) {
            glDeleteShader(vertexId);
        }
        if (fragmentId != 0) {
            glDeleteShader(fragmentId);
        }
        if ((retCode != OK) && (programId != 0)) {
            glDeleteProgram(programId);
        }
        return retCode;
    }

    //シェーダを破棄する
    void Shader::destroy()
    {
        LOGI("%s programId:%d\n", __FUNCTION__, this->programId_);
        if (this->programId_ != 0) {
            glDeleteProgram(this->programId_);
        }
    }

    //プログラムIDを取得する
    UInt32 Shader::getProgramId()
    {
        return this->programId_;
    }

    //Attribute変数を取得する
    UInt32 Shader::getAttrLocation(const Char8* const attr)
    {
        return GLuint(glGetAttribLocation(this->programId_, attr));
    }

    //Uniform変数を取得する
    UInt32 Shader::getUniformLocation(const Char8* const uniform)
    {
        return GLuint(glGetUniformLocation(this->programId_, uniform));
    }
}