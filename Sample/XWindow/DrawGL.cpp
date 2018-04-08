#include "DrawGL.hpp"
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
		"    gl_FragColor = vary_color;\n"
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
	void DrawGL::drawLines(const int32_t pointNum, PointF* const points, ColorUB* colors, const std::float32_t width, const EN_LineType type)
	{
		//カラーRRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_COLOR_RGBA();

		//MVP変換行列をシェーダへ転送
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, (GLfloat*)this->proj_.mat);

		//頂点データ転送
		glVertexAttribPointer(shaderPara.attr_point, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*)points);
		glVertexAttribPointer(shaderPara.attr_color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLubyte*)colors);

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

	//カラーRGBA使用開始
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
}
