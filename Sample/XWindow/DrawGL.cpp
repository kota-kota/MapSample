﻿#include "DrawGL.hpp"
#include <cstdio>

namespace {
	//カラーRGBA バーテックスシェーダ
	const GLchar* color_rgba_vert = 
		"#version 100\n"
		"attribute highp vec3 attr_point;\n"
		"attribute lowp vec4 attr_color;\n"
		"uniform highp mat4 unif_mvp;\n"
		"varying lowp vec4 vary_color;\n"
		"void main() {\n"
		"    gl_Position = unif_mvp * vec4( attr_point, 1.0 );\n"
		"    vary_color = attr_color;\n"
		"}\n";
	//カラーRGBA フラグメントシェーダ
	const GLchar* color_rgba_frag = 
		"#version 100\n"
		"varying lowp vec4 vary_color;\n"
		"void main() {\n"
		"    gl_FragColor = vary_color / 255.0;\n"
		"}\n";

	//テクスチャRGBA バーテックスシェーダ
	const GLchar* texture_rgba_vert = 
		"#version 100\n"
		"attribute highp vec3 attr_point;\n"
		"attribute lowp vec2 attr_uv;\n"
		"uniform highp mat4 unif_mvp;\n"
		"varying lowp vec2 vary_uv;\n"
		"void main() {\n"
		"    gl_Position = unif_mvp * vec4( attr_point, 1.0 );\n"
		"    vary_uv = attr_uv;\n"
		"}\n";

	//テクスチャRGBA フラグメントシェーダ
	const GLchar* texture_rgba_frag = 
		"#version 100\n"
		"uniform lowp sampler2D unif_texture;\n"
		"varying lowp vec2 vary_uv;\n"
		"void main() {\n"
		"    gl_FragColor = texture2D( unif_texture, vary_uv );\n"
		"}\n";
}

namespace draw {

	/**
	 * シェーダクラス
	 */

	//コンストラクタ
	Shader::Shader() :
		type(EN_ShaderType::INVALID), para()
	{
		this->para.prog_id = 0;
		this->para.attr_point = -1;
		this->para.attr_color = -1;
		this->para.attr_uv = -1;
		this->para.unif_mvp = -1;
		this->para.unif_texture = -1;
	}

	//デストラクタ
	Shader::~Shader()
	{
		this->destroy();
	}

	//シェーダ作成
	void Shader::create(const EN_ShaderType shaderType)
	{
		//シェーダ種別毎に作成
		switch(shaderType) {
		case EN_ShaderType::COLOR_RGBA:
			printf("<Shader::create> COLOR_RGBA\n");
			this->createProgram(color_rgba_vert, color_rgba_frag);
			this->setPara_COLOR_RGBA();
			break;
		case EN_ShaderType::TEXTURE_RGBA:
			printf("<Shader::create> TEXTURE_RGBA\n");
			this->createProgram(texture_rgba_vert, texture_rgba_frag);
			this->setPara_TEXTURE_RGBA();
			break;
		default:
			break;
		}
	}

	//シェーダ破棄
	void Shader::destroy()
	{
		printf("<Shader::destroy> %d\n", this->para.prog_id);

		//シェーダを未使用にする
		glUseProgram(0);

		//シェーダ削除
		if(this->para.prog_id != 0) {
			glDeleteProgram(this->para.prog_id);
			this->para.prog_id = 0;
		}

		this->type = EN_ShaderType::INVALID;
		this->para.attr_point = -1;
		this->para.attr_color = -1;
		this->para.attr_uv = -1;
		this->para.unif_mvp = -1;
		this->para.unif_texture = -1;
	}

	//シェーダパラメータ取得
	ShaderPara Shader::getShaderPara()
	{
		return this->para;
	}

	//シェーダプログラム作成
	void Shader::createProgram(const GLchar* vert, const GLchar* frag)
	{
		GLuint	vertId = 0;
		GLuint	fragId = 0;
		GLuint	progId = 0;
		GLint	retCompiled = 0;
		GLint	retLinked = 0;

		//バーテックスシェーダオブジェクト作成しコンパイル
		vertId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertId, 1, (const GLchar * const *)&vert, nullptr);
		glCompileShader(vertId);
		glGetShaderiv(vertId, GL_COMPILE_STATUS, &retCompiled);
		if(retCompiled == GL_FALSE) {
			//失敗
			printf("[ERROR] %s:%d glCompileShader vertId:%d\n", __FUNCTION__, __LINE__, vertId);
			goto ERR;
		}
		printf("glCompileShader vertId:%d\n", vertId);

		//フラグメントシェーダオブジェクト作成
		fragId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragId, 1, (const GLchar * const *)&frag, nullptr);
		glCompileShader(fragId);
		glGetShaderiv(fragId, GL_COMPILE_STATUS, &retCompiled);
		if(retCompiled == GL_FALSE) {
			//失敗
			printf("[ERROR] %s:%d glCompileShader fragId:%d\n", __FUNCTION__, __LINE__, fragId);
			goto ERR;
		}
		printf("glCompileShader fragId:%d\n", fragId);

		//プログラムオブジェクト作成しリンク
		progId = glCreateProgram();
		glAttachShader(progId, vertId);
		glAttachShader(progId, fragId);
		glLinkProgram(progId);
		glGetProgramiv(progId, GL_LINK_STATUS, &retLinked);
		if(retLinked == GL_FALSE) {
			//失敗
			printf("[ERROR] %s:%d glLinkProgram progId:%d\n", __FUNCTION__, __LINE__, progId);
			goto ERR;
		}
		printf("glLinkProgram progId:%d\n", progId);

		//プログラムオブジェクトを保持
		this->para.prog_id = progId;

		//シェーダオブジェクト解放
		glDeleteShader(vertId);
		glDeleteShader(fragId);
		return;

	ERR:
		if(vertId != 0) {
			glDeleteShader(vertId);
		}
		if(fragId != 0) {
			glDeleteShader(fragId);
		}
		if(progId != 0) {
			glDeleteProgram(progId);
		}
		return;
	}

	//シェーダパラメータ設定・有効化(カラーRGBA用)
	void Shader::setPara_COLOR_RGBA()
	{
		//attribute変数を取り出す
		this->para.attr_point = glGetAttribLocation(this->para.prog_id, "attr_point");
		this->para.attr_color = glGetAttribLocation(this->para.prog_id, "attr_color");
		printf("glGetAttribLocation %d %d\n", this->para.attr_point, this->para.attr_color);

		//uniform変数を取り出す
		this->para.unif_mvp = glGetUniformLocation(this->para.prog_id, "unif_mvp");
		printf("glGetUniformLocation %d\n", this->para.unif_mvp);
	}

	//シェーダパラメータ設定(テクスチャRGBA)
	void Shader::setPara_TEXTURE_RGBA()
	{
		//attribute変数を取り出す
		this->para.attr_point = glGetAttribLocation(this->para.prog_id, "attr_point");
		this->para.attr_uv = glGetAttribLocation(this->para.prog_id, "attr_uv");
		printf("glGetAttribLocation %d %d\n", this->para.attr_point, this->para.attr_uv);

		//uniform変数を取り出す
		this->para.unif_mvp = glGetUniformLocation(this->para.prog_id, "unif_mvp");
		this->para.unif_texture = glGetUniformLocation(this->para.prog_id, "unif_texture");
		printf("glGetUniformLocation %d %d\n", this->para.unif_mvp, this->para.unif_texture);
	}


	/**
	 * OpenGL描画インターフェースクラス
	 */

	//コンストラクタ
	DrawGL::DrawGL() :
		shader_(), curShaderType_(EN_ShaderType::INVALID), proj_()
	{
		printf("<DrawGL::DrawGL>\n");
		printf("Vevdor: %s\n", glGetString(GL_VENDOR));
		printf("GPU: %s\n", glGetString(GL_RENDERER));
		printf("OpenGL: %s\n", glGetString(GL_VERSION));

		//シェーダ作成
		this->shader_[EN_ShaderType::COLOR_RGBA].create(EN_ShaderType::COLOR_RGBA);
		this->shader_[EN_ShaderType::TEXTURE_RGBA].create(EN_ShaderType::TEXTURE_RGBA);
	}

	//デストラクタ
	DrawGL::~DrawGL()
	{
	}

	//セットアップ
	void DrawGL::setup(const AreaI& area)
	{
		std::int32_t width = area.xmax - area.xmin;
		std::int32_t height = area.ymax - area.ymin;

		//ビューポート
		glViewport(area.xmin, area.ymin, width, height);

		//プロジェクション行列生成(OpenGLは行と列が逆なので転置する)
		this->proj_ = this->makeProjMatrix_Ortho(0.0F, width, 0.0F, height, -1.0F, 1.0F);
		this->proj_ = this->transMatrix(this->proj_);
	}

	//クリア
	void DrawGL::clear(const ColorUB& color)
	{
		std::float32_t r = std::float32_t(color.r) / 255.0F;
		std::float32_t g = std::float32_t(color.g) / 255.0F;
		std::float32_t b = std::float32_t(color.b) / 255.0F;
		std::float32_t a = std::float32_t(color.a) / 255.0F;

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//ライン描画
	void DrawGL::drawLines(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const std::float32_t width, const EN_LineType type)
	{
		//カラーRRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_COLOR_RGBA();

		//MVP変換行列をシェーダへ転送
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&this->proj_.mat[0]));

		//頂点データ転送
		glVertexAttribPointer(shaderPara.attr_point, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLfloat*>(points));
		glVertexAttribPointer(shaderPara.attr_color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, static_cast<GLubyte*>(colors));

		//ライン描画種別からモードを取得
		GLenum mode = 0;
		if(type == EN_LineType::LINES) { mode = GL_LINES; }
		else if(type == EN_LineType::LINE_STRIP) { mode = GL_LINE_STRIP; }
		else if(type == EN_LineType::LINE_LOOP) { mode = GL_LINE_LOOP; }
		else { /* 未サポート */ };

		//描画
		glLineWidth(width);
		glDrawArrays(mode, 0, pointNum);
	}

	//ポリゴン描画
	void DrawGL::drawPolygons(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const EN_PolygonType type)
	{
		//カラーRRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_COLOR_RGBA();

		//MVP変換行列をシェーダへ転送
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&this->proj_.mat[0]));

		//頂点データ転送
		glVertexAttribPointer(shaderPara.attr_point, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLfloat*>(points));
		glVertexAttribPointer(shaderPara.attr_color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, static_cast<GLubyte*>(colors));

		//ポリゴン描画種別からモードを取得
		GLenum mode = 0;
		if(type == EN_PolygonType::TRIANGLE_STRIP) { mode = GL_TRIANGLE_STRIP; }
		else if(type == EN_PolygonType::TRIANGLE_FAN) { mode = GL_TRIANGLE_FAN; }
		else { /* 未サポート */ };

		//描画
		glDrawArrays(mode, 0, pointNum);
	}

	//画像描画
	void DrawGL::drawImage(std::float32_t* const point, std::uint8_t* const image, const ImageAttr& imgAttr)
	{
		//テクスチャRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_TEXTURE_RGBA();

		//MVP変換行列をシェーダへ転送
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&this->proj_.mat[0]));

		//GL描画設定
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		//テクスチャ生成
		GLuint texId;
		glGenTextures(1, &texId);

		//テクスチャバインド
		glBindTexture(GL_TEXTURE_2D, texId);

		//テクスチャロード
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgAttr.width, imgAttr.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		//テクスチャパラメータ設定
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//テクスチャユニット0を指定
		glUniform1i(shaderPara.unif_texture, 0);

		//頂点座標
		const GLint pointNum = 4;
		GLfloat p[pointNum * 3] = {
			point[0] - (imgAttr.width / 2), point[1] + (imgAttr.height / 2), point[2],
			point[0] + (imgAttr.width / 2), point[1] + (imgAttr.height / 2), point[2],
			point[0] - (imgAttr.width / 2), point[1] - (imgAttr.height / 2), point[2],
			point[0] + (imgAttr.width / 2), point[1] - (imgAttr.height / 2), point[2],
		};

		//UV座標
		GLfloat uv[pointNum * 2] = {
			0.0F, 0.0F,
			1.0F, 0.0F,
			0.0F, 1.0F,
			1.0F, 1.0F,
		};

		//頂点データ転送
		glVertexAttribPointer(shaderPara.attr_point, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*)&p[0]);
		glVertexAttribPointer(shaderPara.attr_uv, 2, GL_FLOAT, GL_FALSE, 0, (GLfloat*)&uv[0]);

		//描画
		glDrawArrays(GL_TRIANGLE_STRIP, 0, pointNum);

		//テクスチャアンバインド
		glBindTexture(GL_TEXTURE_2D, 0);

		//テクスチャ破棄
		glDeleteTextures(1, &texId);

		//GL描画設定解除
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	//シェーダパラメータ使用開始
	ShaderPara DrawGL::useShader_COLOR_RGBA()
	{
		//カラーRGBA
		const EN_ShaderType type = EN_ShaderType::COLOR_RGBA;

		//シェーダパラメータ取得
		Shader* shader = &this->shader_[type];
		ShaderPara shaderPara = shader->getShaderPara();

		//現在使用中でなければ使用開始
		if(this->curShaderType_ != type) {
			//シェーダプログラムをバインド
			glUseProgram(shaderPara.prog_id);

			//attribute変数有効化
			glEnableVertexAttribArray(shaderPara.attr_point);
			glEnableVertexAttribArray(shaderPara.attr_color);

			//使用中のシェーダ種別に設定
			this->curShaderType_ = type;
		}

		return shaderPara;
	}
	ShaderPara DrawGL::useShader_TEXTURE_RGBA()
	{
		//テクスチャRGBA
		const EN_ShaderType type = EN_ShaderType::TEXTURE_RGBA;

		//シェーダパラメータ取得
		Shader* shader = &this->shader_[type];
		ShaderPara shaderPara = shader->getShaderPara();

		//現在使用中でなければ使用開始
		if(this->curShaderType_ != type) {
			//シェーダプログラムをバインド
			glUseProgram(shaderPara.prog_id);

			//attribute変数有効化
			glEnableVertexAttribArray(shaderPara.attr_point);
			glEnableVertexAttribArray(shaderPara.attr_uv);

			//使用中のシェーダ種別に設定
			this->curShaderType_ = type;
		}

		return shaderPara;
	}
}
