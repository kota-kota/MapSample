#include "DrawGL.hpp"
#include "Str.hpp"
#include <cstdio>
#include <vector>

namespace {
	//フォントファイル
	//const std::string fontFilePath = "./data/font/ipagp.ttf";
	//const std::string fontFilePath = "C:\\Windows\\Fonts\\meiryo.ttc";
	const std::string fontFilePath = "C:\\Windows\\Fonts\\msgothic.ttc";

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

	//テクスチャA バーテックスシェーダ
	const GLchar* texture_a_vert =
		"#version 100\n"
		"attribute highp vec3 attr_point;\n"
		"attribute lowp vec2 attr_uv;\n"
		"uniform highp mat4 unif_mvp;\n"
		"varying lowp vec2 vary_uv;\n"
		"void main() {\n"
		"    gl_Position = unif_mvp * vec4( attr_point, 1.0 );\n"
		"    vary_uv = attr_uv;\n"
		"}\n";

	//テクスチャA フラグメントシェーダ
	const GLchar* texture_a_frag =
		"#version 100\n"
		"uniform lowp sampler2D unif_texture;\n"
		"uniform lowp vec4 unif_texcolor;\n"
		"varying lowp vec2 vary_uv;\n"
		"void main() {\n"
		"    gl_FragColor = vec4(unif_texcolor.rgb, texture2D( unif_texture, vary_uv ).a * unif_texcolor.a);\n"
		"}\n";
}

namespace fw {

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
		switch (shaderType) {
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
		case EN_ShaderType::TEXTURE_A:
			printf("<Shader::create> TEXTURE_A\n");
			this->createProgram(texture_a_vert, texture_a_frag);
			this->setPara_TEXTURE_A();
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
		if (this->para.prog_id != 0) {
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
		if (retCompiled == GL_FALSE) {
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
		if (retCompiled == GL_FALSE) {
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
		if (retLinked == GL_FALSE) {
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
		if (vertId != 0) {
			glDeleteShader(vertId);
		}
		if (fragId != 0) {
			glDeleteShader(fragId);
		}
		if (progId != 0) {
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

	//シェーダパラメータ設定(テクスチャA用)
	void Shader::setPara_TEXTURE_A()
	{
		//attribute変数を取り出す
		this->para.attr_point = glGetAttribLocation(this->para.prog_id, "attr_point");
		this->para.attr_uv = glGetAttribLocation(this->para.prog_id, "attr_uv");
		printf("glGetAttribLocation %d %d\n", this->para.attr_point, this->para.attr_uv);

		//uniform変数を取り出す
		this->para.unif_mvp = glGetUniformLocation(this->para.prog_id, "unif_mvp");
		this->para.unif_texture = glGetUniformLocation(this->para.prog_id, "unif_texture");
		this->para.unif_texcolor = glGetUniformLocation(this->para.prog_id, "unif_texcolor");
		printf("glGetUniformLocation %d %d %d\n", this->para.unif_mvp, this->para.unif_texture, this->para.unif_texcolor);
	}



	/**
	* フォントクラス
	*/

	//コンストラクタ
	Font::Font(const std::string fontFilePath) :
		ftLibrary_(nullptr), ftFace_(nullptr), ftIsKerning_(0)
	{
		FT_Init_FreeType(&this->ftLibrary_);
		FT_New_Face(this->ftLibrary_, fontFilePath.c_str(), 0, &this->ftFace_);
		this->ftIsKerning_ = FT_HAS_KERNING(this->ftFace_);
	}

	//デストラクタ
	Font::~Font()
	{
		FT_Done_Face(this->ftFace_);
		FT_Done_FreeType(this->ftLibrary_);
	}


	/**
	 * OpenGL描画インターフェースクラス
	 */

	//コンストラクタ
	DrawGL::DrawGL() :
		shader_(), curShaderType_(EN_ShaderType::INVALID), proj_(), ftLibrary_(nullptr), ftFace_(nullptr), ftIsKerning_(0)
	{
		printf("<DrawGL::DrawGL>\n");
		printf("Vevdor: %s\n", glGetString(GL_VENDOR));
		printf("GPU: %s\n", glGetString(GL_RENDERER));
		printf("OpenGL: %s\n", glGetString(GL_VERSION));
		printf("Extension: %s\n", glGetString(GL_EXTENSIONS));

		//シェーダ作成
		this->shader_[EN_ShaderType::COLOR_RGBA].create(EN_ShaderType::COLOR_RGBA);
		this->shader_[EN_ShaderType::TEXTURE_RGBA].create(EN_ShaderType::TEXTURE_RGBA);
		this->shader_[EN_ShaderType::TEXTURE_A].create(EN_ShaderType::TEXTURE_A);

		//FreeType初期化
		FT_Init_FreeType(&this->ftLibrary_);
		FT_New_Face(this->ftLibrary_, fontFilePath.c_str(), 0, &this->ftFace_);
		this->ftIsKerning_ = FT_HAS_KERNING(this->ftFace_);
	}

	//デストラクタ
	DrawGL::~DrawGL()
	{
		//FreeType破棄
		FT_Done_Face(this->ftFace_);
		FT_Done_FreeType(this->ftLibrary_);
	}

	//セットアップ
	void DrawGL::setup(const AreaI& area)
	{
		std::int32_t width = area.xmax - area.xmin;
		std::int32_t height = area.ymax - area.ymin;
		std::float32_t widthF = static_cast<std::float32_t>(width);
		std::float32_t heightF = static_cast<std::float32_t>(height);

		//ビューポート
		glViewport(area.xmin, area.ymin, width, height);

		//プロジェクション行列生成
		this->proj_ = this->makeProjMatrix_Ortho(0.0F, widthF, 0.0F, heightF, -1.0F, 1.0F);
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

	//点描画
	void DrawGL::drawPoints(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors)
	{
#if 0	/*再考する。点の大きさとか、「■点」「○点」とか、別途シェーダ書く必要がありそう。*/
		//カラーRRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_COLOR_RGBA();

		//MVP変換行列をシェーダへ転送(OpenGLは行と列が逆なので転置する)
		MatrixF mvp = this->transposeMatrix(this->proj_);
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&mvp.mat[0]));

		//頂点データ転送
		glVertexAttribPointer(shaderPara.attr_point, 3, GL_FLOAT, GL_FALSE, 0, static_cast<GLfloat*>(points));
		glVertexAttribPointer(shaderPara.attr_color, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, static_cast<GLubyte*>(colors));

		//描画
		glDrawArrays(GL_POINTS, 0, pointNum);
#endif
	}

	//ライン描画
	void DrawGL::drawLines(const std::int32_t pointNum, std::float32_t* const points, std::uint8_t* colors, const std::float32_t width, const EN_LineType type)
	{
		//カラーRRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_COLOR_RGBA();

		//MVP変換行列をシェーダへ転送(OpenGLは行と列が逆なので転置する)
		MatrixF mvp = this->transposeMatrix(this->proj_);
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&mvp.mat[0]));

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
		MatrixF mvp = this->transposeMatrix(this->proj_);
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&mvp.mat[0]));

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
	void DrawGL::drawImage(std::float32_t* const point, const std::float32_t angle, std::uint8_t* const image, const ImageAttr& imgAttr)
	{
		//テクスチャRGBAシェーダを使用
		ShaderPara shaderPara = this->useShader_TEXTURE_RGBA();

		//angle分回転させるモデル変換行列を生成
		MatrixF model;
		VectorF trans, rotate;
		trans = { -point[0], -point[1], 0.0F };
		model = this->translateMatrix(trans);
		rotate = { 0.0F, 0.0F, angle };
		model = this->multiplyMatrix(this->rotateMatrix(rotate), model);
		trans = { point[0], point[1], 0.0F };
		model = this->multiplyMatrix(this->translateMatrix(trans), model);

		//MVP変換行列をシェーダへ転送
		MatrixF mvp;
		mvp = this->multiplyMatrix(this->proj_, model);
		mvp = this->transposeMatrix(mvp);
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&mvp.mat[0]));

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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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

	//テキスト描画(1バイト文字)
	void DrawGL::drawText(std::float32_t* const point, const StringU8& text, const TextAttr& textAttr)
	{
		//テキスト文字列をワイド文字(UTF16BE)に変換
		StringU16 utf16Text;
		StringIF::convert(text, EN_CharCode::UTF16BE, &utf16Text);

		//テキスト描画(UTF16BE)
		this->drawText_UTF16BE(point, utf16Text, textAttr);
	}

	//テキスト描画(2バイト文字)
	void DrawGL::drawText(std::float32_t* const point, const StringU16& text, const TextAttr& textAttr)
	{
		//テキスト文字列をワイド文字(UTF16BE)に変換
		StringU16 utf16Text;
		StringIF::convert(text, EN_CharCode::UTF16BE, &utf16Text);

		//テキスト描画(UTF16BE)
		this->drawText_UTF16BE(point, utf16Text, textAttr);
	}

	//テキスト描画(UTF16BE)
	void DrawGL::drawText_UTF16BE(std::float32_t* const point, const StringU16& utf16Text, const TextAttr& textAttr)
	{
		//テキスト文字列のフォントグリフ保持用
		const std::int32_t MAX_GLYPHS = 32;
		FontGlyph glyphs[MAX_GLYPHS];
		std::int32_t numGlyphs = 0;

		//前回のグリフインデックス
		FT_UInt preGlyphIndex = 0;

		//文字列のバウンディングボックス
		FT_BBox stringBBox;

		//フォントサイズ設定
		FT_Set_Char_Size(this->ftFace_, textAttr.size * 64, 0, 96, 0);

		//文字列の長さ分ループ
		const std::int32_t len = utf16Text.getLen();
		for (std::int32_t i = 0; i < len; i++) {
			//処理対象文字
			const std::uint16_t c = static_cast<std::uint16_t>(utf16Text[i]);

			//処理対象文字のグリフ格納用
			FontGlyph* glyph = &glyphs[numGlyphs];

			//グリフインデックスを取得
			glyph->index_ = FT_Get_Char_Index(this->ftFace_, c);

			//カーニング
			//TODO:現状未使用
			if ((this->ftIsKerning_ != 0) && (preGlyphIndex != 0) && (glyph->index_ != 0)) {
				//カーニングを取得
				FT_Vector delta;
				FT_Get_Kerning(this->ftFace_, preGlyphIndex, glyph->index_, FT_KERNING_DEFAULT, &delta);

				glyph->metrics_.kerningX_ = delta.x >> 6;
				glyph->metrics_.kerningY_ = delta.y >> 6;
			}

			//グリフをロード
			FT_Load_Glyph(this->ftFace_, glyph->index_, FT_LOAD_DEFAULT);

			//グリフを描画
			FT_Get_Glyph(this->ftFace_->glyph, &glyph->image_);
			FT_Glyph_To_Bitmap(&glyph->image_, FT_RENDER_MODE_NORMAL, nullptr, 1);

			//寸法情報を取得
			FT_BitmapGlyph bit = (FT_BitmapGlyph)glyph->image_;
			glyph->metrics_.width_ = bit->bitmap.width;
			glyph->metrics_.height_ = bit->bitmap.rows;
			glyph->metrics_.offsetX_ = bit->left;
			glyph->metrics_.offsetY_ = bit->top;
			glyph->metrics_.nextX_ = this->ftFace_->glyph->advance.x >> 6;
			glyph->metrics_.nextY_ = this->ftFace_->glyph->advance.y >> 6;

			//処理対象文字のバウンディングボックスを取得
			FT_BBox bbox;
			FT_Glyph_Get_CBox(glyph->image_, ft_glyph_bbox_pixels, &bbox);

			if (i == 0) {
				stringBBox.xMin = 0;
				stringBBox.xMax = glyph->metrics_.nextX_;
				stringBBox.yMin = bbox.yMin;
				stringBBox.yMax = bbox.yMax;
			}
			else {
				stringBBox.xMin = 0;
				stringBBox.xMax += glyph->metrics_.nextX_;
				if (bbox.yMin < stringBBox.yMin) { stringBBox.yMin = bbox.yMin; }
				if (bbox.yMax > stringBBox.yMax) { stringBBox.yMax = bbox.yMax; }
			}
			
			//次文字へ
			numGlyphs++;
		}

		//文字列の幅高さ
		const std::int32_t stringW = stringBBox.xMax - stringBBox.xMin;
		const std::int32_t stringH = stringBBox.yMax - stringBBox.yMin;
		const std::int32_t stringSize = stringW * stringH;

		//文字列画像の領域を確保
		std::uint8_t* tex = new std::uint8_t[stringSize];
		memset(tex, 0, stringSize);

		//文字列画像を生成
		FT_Vector pen = { 0, stringBBox.yMax };
		for (std::int32_t i = 0; i < numGlyphs; i++) {
			//処理対象文字のグリフを取得
			FontGlyph* glyph = &glyphs[i];
			FT_BitmapGlyph bit = (FT_BitmapGlyph)glyph->image_;

			const std::int32_t xoffset = pen.x + glyph->metrics_.offsetX_;
			const std::int32_t yoffset = pen.y - glyph->metrics_.offsetY_;
			std::int32_t readOffset = 0;
			std::int32_t writeOffset = xoffset + (yoffset * stringW);
			for (std::int32_t h = 0; h < glyph->metrics_.height_; h++) {
				memcpy_s(tex + writeOffset, stringW, bit->bitmap.buffer + readOffset, glyph->metrics_.width_);
				readOffset += glyph->metrics_.width_;
				writeOffset += stringW;
			}

			pen.x += glyph->metrics_.nextX_;

			//グリフイメージ破棄
			FT_Done_Glyph(glyph->image_);
		}

		//テクスチャAシェーダを使用
		ShaderPara shaderPara = this->useShader_TEXTURE_A();

		//MVP変換行列をシェーダへ転送
		MatrixF mvp = this->transposeMatrix(this->proj_);
		glUniformMatrix4fv(shaderPara.unif_mvp, 1, GL_FALSE, static_cast<GLfloat*>(&mvp.mat[0]));

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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, stringW, stringH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tex);

		//文字列画像の領域を解放
		delete[] tex;

		//テクスチャパラメータ設定
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//テクスチャユニット0を指定
		glUniform1i(shaderPara.unif_texture, 0);

		const std::float32_t xmin = point[0] - (static_cast<std::float32_t>(stringW) / 2.0F);
		const std::float32_t ymin = point[1] - (static_cast<std::float32_t>(stringH) / 2.0F);
		const std::float32_t xmax = point[0] + (static_cast<std::float32_t>(stringW) / 2.0F);
		const std::float32_t ymax = point[1] + (static_cast<std::float32_t>(stringH) / 2.0F);

		//頂点座標
		const GLint pointNum = 4;
		GLfloat p[pointNum * 3] = {
			xmin, ymax, 0.0F,
			xmax, ymax, 0.0F,
			xmin, ymin, 0.0F,
			xmax, ymin, 0.0F,
		};

		//UV座標
		GLfloat uv[pointNum * 2] = {
			0.0F, 0.0F,
			1.0F, 0.0F,
			0.0F, 1.0F,
			1.0F, 1.0F,
		};

		//テクスチャ色
		GLfloat color[4];
		color[0] = textAttr.bodyColor.r / 255.0F;
		color[1] = textAttr.bodyColor.g / 255.0F;
		color[2] = textAttr.bodyColor.b / 255.0F;
		color[3] = textAttr.bodyColor.a / 255.0F;
		glUniform4fv(shaderPara.unif_texcolor, 1, &color[0]);

		//頂点データ転送
		glVertexAttribPointer(shaderPara.attr_point, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*)&p[0]);
		glVertexAttribPointer(shaderPara.attr_uv, 2, GL_FLOAT, GL_FALSE, 0, (GLfloat*)&uv[0]);

		//描画
		glDrawArrays(GL_TRIANGLE_STRIP, 0, pointNum);

		//テクスチャアンバインド
		glBindTexture(GL_TEXTURE_2D, 0);

		//テクスチャ破棄
		glDeleteTextures(1, &texId);

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
	ShaderPara DrawGL::useShader_TEXTURE_A()
	{
		//テクスチャA
		const EN_ShaderType type = EN_ShaderType::TEXTURE_A;

		//シェーダパラメータ取得
		Shader* shader = &this->shader_[type];
		ShaderPara shaderPara = shader->getShaderPara();

		//現在使用中でなければ使用開始
		if (this->curShaderType_ != type) {
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
