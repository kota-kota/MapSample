#ifndef INCLUDED_DRAWGL_HPP
#define INCLUDED_DRAWGL_HPP

#include "DrawIF.hpp"
#include "Font.hpp"
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>

namespace draw {

	//シェーダ種別
	enum EN_ShaderType {
		INVALID = -1,		//無効値
		COLOR_RGBA,			//カラーRGBA
		TEXTURE_RGBA,		//テクスチャRGBA
		TEXTURE_A,			//テクスチャA
		NUM,				//数
	};

	//シェーダパラメータ構造体
	struct ShaderPara {
		GLuint	prog_id;		//シェーダプログラムID
		GLint	attr_point;		//attribute変数point
		GLint	attr_color;		//attribute変数color
		GLint	attr_uv;		//attribute変数uv
		GLint	unif_mvp;		//uniform変数mvp
		GLint	unif_texture;	//uniform変数texture
		GLint	unif_texcolor;	//uniform変数texcolor
	};

	/**
	 * シェーダクラス
	 */
	class Shader {
		//メンバ変数
		EN_ShaderType	type;			//シェーダタイプ
		ShaderPara		para;			//シェーダパラメータ

	public:
		//コンストラクタ
		Shader();
		//デストラクタ
		~Shader();
		//シェーダ作成
		void create(const EN_ShaderType shaderType);
		//シェーダ破棄
		void destroy();
		//シェーダパラメータ取得
		ShaderPara getShaderPara();

	private:
		//シェーダプログラム作成
		void createProgram(const GLchar* vert, const GLchar* frag);
		//シェーダパラメータ設定(カラーRGBA用)
		void setPara_COLOR_RGBA();
		//シェーダパラメータ設定(テクスチャRGBA用)
		void setPara_TEXTURE_RGBA();
		//シェーダパラメータ設定(テクスチャA用)
		void setPara_TEXTURE_A();
	};

	/**
	 * OpenGL描画インターフェースクラス
	 */
	class DrawGL : public DrawIF {
		//メンバ変数
		Shader			shader_[EN_ShaderType::NUM];
		Font			font_;
		EN_ShaderType	curShaderType_;
		MatrixF			proj_;

	public:
		//コンストラクタ
		DrawGL();
		//デストラクタ
		virtual ~DrawGL();
		//セットアップ
		virtual void setup(const AreaI& area);
		//クリア
		virtual void clear(const ColorUB& color);
		//ライン描画
		virtual void drawLines(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const std::float32_t width, const EN_LineType type);
		//ポリゴン描画
		virtual void drawPolygons(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const EN_PolygonType type);
		//画像描画
		virtual void drawImage(std::float32_t* const point, const std::float32_t angle, std::uint8_t* const image, const ImageAttr& imgAttr);
		//テキスト描画
		virtual void drawText(std::float32_t* const point, const std::char8_t* const text, const TextAttr& textAttr);

	private:
		//シェーダパラメータ使用開始
		ShaderPara useShader_COLOR_RGBA();
		ShaderPara useShader_TEXTURE_RGBA();
		ShaderPara useShader_TEXTURE_A();

		//コピーコンストラクタ(禁止)
		DrawGL(const DrawGL& org) = delete;
		//コピー代入演算子(禁止)
		void operator=(const DrawGL& org) = delete;
	};
};


#endif //INCLUDED_DRAWGL_HPP
