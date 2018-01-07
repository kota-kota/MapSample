#include "DrawWEGL.hpp"

#ifdef DRAWIF_WEGL
#include "image/Image.hpp"
#include "image/Font.hpp"

#include <fstream>
#include <EGL/egl.h>
#include <GLES3/gl3.h>


#pragma comment(lib, "libEGL.lib")
#pragma comment(lib, "libGLES_CM.lib")
#pragma comment(lib, "libGLESv2.lib")


namespace {

	//EGLコンフィグ属性
	const EGLint eglConfigAttribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_NONE
	};

	//EGLコンテキスト属性
	const EGLint eglContextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};

	//シェーダ
	static const std::string color_rgba_vert = "/shader/color_rgba.vert";
	static const std::string color_rgba_frag = "/shader/color_rgba.frag";


	//シェーダソースファイル読み込み
	void readShaderSource(const fw::EN_ShaderType shaderType, std::string* const vert, std::string* const frag)
	{
		//シェーダタイプからシェーダのファイルパスを取得
		std::string vertFilePath;
		std::string fragFilePath;
		switch (shaderType) {
		case fw::EN_ShaderType::COLOR_RGBA:
			vertFilePath = std::D_DATA_PATH + color_rgba_vert;
			fragFilePath = std::D_DATA_PATH + color_rgba_frag;
		default:
			break;
		}

		//バーテックスシェーダ読み込み
		std::ifstream vfile(vertFilePath.c_str());
		std::istreambuf_iterator<GLchar> vbeginFile(vfile);
		std::istreambuf_iterator<GLchar> vendFile;
		*vert = std::string(vbeginFile, vendFile);

		//フラグメントシェーダ読み込み
		std::ifstream ffile(fragFilePath.c_str());
		std::istreambuf_iterator<GLchar> fbeginFile(ffile);
		std::istreambuf_iterator<GLchar> fendFile;
		*frag = std::string(fbeginFile, fendFile);
	}

	//シェーダコンパイルエラーログ出力
	void printCompileErrorLog(const GLuint shaderId)
	{
		//シェーダコンパイル時のログサイズを取得
		GLsizei bufSize = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &bufSize);
		if (bufSize > 1) {
			GLchar* log = new GLchar[bufSize];

			//ログ取得
			GLsizei length = 0;
			glGetShaderInfoLog(shaderId, bufSize, &length, log);

			//ログ出力
			printf("!!!ERROR!!!\n%s\n", log);

			delete[] log;
		}
		else {
			printf("!!!ERROR!!!\nNot Messages\n");
		}
	}

	//シェーダリンクエラーログ出力
	void printLinkErrorLog(const GLuint progId)
	{
		//シェーダリンク時のログサイズを取得
		GLsizei bufSize = 0;
		glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &bufSize);
		if (bufSize > 1) {
			GLchar* log = new GLchar[bufSize];

			//ログ取得
			GLsizei length = 0;
			glGetProgramInfoLog(progId, bufSize, &length, log);

			//ログ出力
			printf("!!!ERROR!!!\n%s\n", log);

			delete[] log;
		}
		else {
			printf("!!!ERROR!!!\nNot Messages\n");
		}
	}

	//シェーダコンパイル
	bool compileShader(const GLuint shaderId, const GLchar* const source)
	{
		bool success = false;

		//シェーダオブジェクト作成
		if (shaderId > 0) {
			//シェーダオブジェクト作成成功

			//シェーダソース読み込んでコンパイル実行
			glShaderSource(shaderId, 1, &source, nullptr);
			glCompileShader(shaderId);

			//コンパイル結果
			GLint result = GL_FALSE;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
			if (result == GL_TRUE) {
				//成功
				success = true;
			}
			else {
				//失敗(ログ出力)
				printCompileErrorLog(shaderId);
			}
		}

		return success;
	}

	//シェーダリンク
	bool linkShader(const GLuint progId, const GLuint vertId, const GLuint fragId)
	{
		bool success = false;

		if (progId > 0) {
			//シェーダオブジェクトをプログラムへ登録
			glAttachShader(progId, vertId);
			glAttachShader(progId, fragId);

			//リンクを実行
			glLinkProgram(progId);

			//リンク結果
			GLint result = GL_FALSE;
			glGetProgramiv(progId, GL_LINK_STATUS, &result);
			if (result == GL_TRUE) {
				//成功
				success = true;
			}
			else {
				//失敗(ログ出力)
				printLinkErrorLog(progId);
			}
		}

		return success;
	}

	//シェーダプログラム作成
	GLuint createShaderProgram(const fw::EN_ShaderType shaderType)
	{
		bool result = true;

		//シェーダソース読み込み
		std::string vert;
		std::string frag;
		readShaderSource(shaderType, &vert, &frag);

		//シェーダオブジェクト作成
		GLuint vertId = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragId = glCreateShader(GL_FRAGMENT_SHADER);

		//プログラムオブジェクト作成
		GLuint progId = glCreateProgram();

		//バーテックスシェーダのコンパイル
		printf("shaderType:%d vertId:%d\n", shaderType, vertId);
		result = compileShader(vertId, vert.c_str());
		if (!result) {
			goto END;
		}

		//フラグメントシェーダのコンパイル
		printf("shaderType:%d fragId:%d\n", shaderType, fragId);
		result = compileShader(fragId, frag.c_str());
		if (!result) {
			goto END;
		}

		//シェーダのリンク
		printf("shaderType:%d progId:%d\n", shaderType, progId);
		result = linkShader(progId, vertId, fragId);
		if (!result) {
			goto END;
		}

	END:
		if (vertId > 0) {
			//シェーダオブジェクト解放
			glDeleteShader(vertId);
		}
		if (fragId > 0) {
			//シェーダオブジェクト解放
			glDeleteShader(fragId);
		}
		if ((!result) && (progId > 0)) {
			//失敗時のみプログラムオブジェクト解放
			glDeleteProgram(progId);
			progId = 0;
		}

		return progId;
	}

	//シェーダプログラム解放
	void deleteShaderProgram(const GLuint progId)
	{
		if (progId > 0) {
			glDeleteProgram(progId);
		}
	}
}

//----------------------------------------------------------
//
// WEGL描画クラス
//
//----------------------------------------------------------

//コンストラクタ
fw::DrawWEGL::DrawWEGL(const HWND hWnd) :
	hWnd_(hWnd), display_(EGL_NO_DISPLAY), config_(0), surface_(EGL_NO_SURFACE), context_(EGL_NO_CONTEXT),
	font_(nullptr), model_(), view_(), proj_(), shaderPara_()
{
}

//デストラクタ
fw::DrawWEGL::~DrawWEGL()
{
	//カレント設定
	(void)::eglMakeCurrent(this->display_, this->surface_, this->surface_, this->context_);

	//シェーダプログラム解放
	deleteShaderProgram(this->shaderPara_.colorRGBA_.progId_);

	//カレント解除
	(void)::eglMakeCurrent(this->display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	if (this->font_ != nullptr) {
		//フォントオブジェクト解放
		delete this->font_;
	}

	//EGL資源破棄
	::eglTerminate(this->display_);
}

//作成
void fw::DrawWEGL::create()
{
	//EGLディスプレイを取得
	this->display_ = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (this->display_ == EGL_NO_DISPLAY) {
		printf("!!!Error!!! eglGetDisplay\n");
		return;
	}
	printf("EGLDisplay:0x%p\n", this->display_);

	//EGL初期化
	if (!::eglInitialize(this->display_, nullptr, nullptr)) {
		printf("!!!Error!!! eglInitialize\n");
		return;
	}

	//EGLコンフィグを取得
	EGLint numConfigs = 0;
	if (!::eglChooseConfig(this->display_, eglConfigAttribs, &this->config_, 1, &numConfigs)) {
		printf("!!!Error!!! eglChooseConfig\n");
		return;
	}
	if (numConfigs < 1) {
		printf("!!!Error!!! eglChooseConfig numConfigs:%d\n", numConfigs);
		return;
	}
	printf("EGLConfig:0x%p\n", this->config_);

	//EGLサーフェイスを作成
	this->surface_ = ::eglCreateWindowSurface(this->display_, this->config_, this->hWnd_, nullptr);
	if (this->surface_ == EGL_NO_SURFACE) {
		printf("!!!Error!!! eglCreateWindowSurface\n");
		return;
	}
	printf("EGLSurface:0x%p\n", this->surface_);

	// EGLコンテキストを作成
	this->context_ = ::eglCreateContext(this->display_, this->config_, EGL_NO_CONTEXT, eglContextAttribs);
	if (this->context_ == EGL_NO_CONTEXT) {
		printf("!!!Error!!! eglCreateContext\n");
		return;
	}
	printf("EGLContext:0x%p\n", this->context_);

	//カレント設定
	(void)::eglMakeCurrent(this->display_, this->surface_, this->surface_, this->context_);

	//カラーRGBA用のシェーダパラメータ作成
	fw::ShaderPara_ColorRGBA* const colorRGBA = &this->shaderPara_.colorRGBA_;
	colorRGBA->progId_ = createShaderProgram(fw::EN_ShaderType::COLOR_RGBA);
	colorRGBA->attr_point_ = glGetAttribLocation(colorRGBA->progId_, "attr_point");
	colorRGBA->attr_color_ = glGetAttribLocation(colorRGBA->progId_, "attr_color");
	colorRGBA->unif_model_ = glGetUniformLocation(colorRGBA->progId_, "unif_model");
	colorRGBA->unif_view_ = glGetUniformLocation(colorRGBA->progId_, "unif_view");
	colorRGBA->unif_proj_ = glGetUniformLocation(colorRGBA->progId_, "unif_proj");

	//カレント解除
	(void)::eglMakeCurrent(this->display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	//MVP変換行列を初期化
	this->model_ = fw::Math::identifyMatrixF();
	this->view_ = fw::Math::identifyMatrixF();
	this->proj_ = fw::Math::identifyMatrixF();

	//フォントオブジェクトを作成
	this->font_ = new image::Font();
}

//描画セットアップ
void fw::DrawWEGL::setup(const std::Position& mapPos)
{
	PAINTSTRUCT ps;
	::BeginPaint(this->hWnd_, &ps);
	::EndPaint(this->hWnd_, &ps);

	//画面幅高さを取得
	std::WH screenwh = this->getScreenWH();

	//ビューポート設定
	glViewport(0, 0, screenwh.width, screenwh.height);

	//プロジェクション設定
	std::double_t left = mapPos.x - screenwh.width / 2;
	std::double_t right = left + screenwh.width;
	std::double_t top = mapPos.y + screenwh.height / 2;
	std::double_t bottom = top - screenwh.height;
	fw::MatrixD proj = fw::Math::orthoMatrix(left, right, bottom, top, -1.0, 1.0);
	proj = fw::Math::transposeMatrix(proj);
	this->proj_ = fw::Math::convMatrixD2MatrixF(proj);

	//モデルビュー設定
	this->model_ = fw::Math::identifyMatrixF();
}

//描画カレント
void fw::DrawWEGL::makeCurrent(const bool current)
{
	if (current) {
		//カレント設定
		(void)::eglMakeCurrent(this->display_, this->surface_, this->surface_, this->context_);
	}
	else {
		//カレント解除
		(void)::eglMakeCurrent(this->display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	}
}

//描画更新
void fw::DrawWEGL::swapBuffers()
{
	(void)::eglSwapBuffers(this->display_, this->surface_);
}

//クリア
void fw::DrawWEGL::clear(const std::ColorUB& color)
{
	std::float_t r = std::float_t(color.r) / 255.0F;
	std::float_t g = std::float_t(color.g) / 255.0F;
	std::float_t b = std::float_t(color.b) / 255.0F;
	std::float_t a = std::float_t(color.a) / 255.0F;

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//点描画
void fw::DrawWEGL::drawPoints(const DrawCoords& coords, const DrawColors& colors, const std::float_t size)
{
}

//ライン描画
void fw::DrawWEGL::drawLines(const DrawCoords& coords, const DrawColors& colors, const std::float_t width)
{
	//シェーダプログラムを選択しバインド
	const fw::ShaderPara_ColorRGBA& colorRGBA = this->shaderPara_.colorRGBA_;
	glUseProgram(colorRGBA.progId_);

	//attribute変数有効化
	glEnableVertexAttribArray(colorRGBA.attr_point_);
	glEnableVertexAttribArray(colorRGBA.attr_color_);

	this->proj_ = fw::Math::identifyMatrixF();
	glUniformMatrix4fv(colorRGBA.unif_model_, 1, GL_FALSE, (GLfloat*)this->model_.mat);
	glUniformMatrix4fv(colorRGBA.unif_view_, 1, GL_FALSE, (GLfloat*)this->view_.mat);
	glUniformMatrix4fv(colorRGBA.unif_proj_, 1, GL_FALSE, (GLfloat*)this->proj_.mat);

	//頂点データ転送
	std::CoordF coord[4];
	coord[0] = { -0.7f, -0.7f, 1.0f };
	coord[1] = { -0.7f, 0.7f, 1.0f };
	coord[2] = { 0.7f, 0.7f, 1.0f };
	coord[3] = { 0.7f, -0.7f, 1.0f };
	std::ColorUB color[4];
	color[0] = { 255, 0, 0, 255 };
	color[1] = { 255, 0, 0, 255 };
	color[2] = { 255, 0, 0, 255 };
	color[3] = { 255, 0, 0, 255 };
	glVertexAttribPointer(colorRGBA.attr_point_, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*)coord);
	glVertexAttribPointer(colorRGBA.attr_color_, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, (GLubyte*)color);

	//描画
	glLineWidth(width);
	glDrawArrays(GL_LINE_STRIP, 0, 4);

	/*
	//MVP変換行列をシェーダへ転送(OpenGLは行と列が逆なので転置する)
	glUniformMatrix4fv(colorRGBA.unif_model_, 1, GL_FALSE, (GLfloat*)this->model_.mat);
	glUniformMatrix4fv(colorRGBA.unif_view_, 1, GL_FALSE, (GLfloat*)this->view_.mat);
	glUniformMatrix4fv(colorRGBA.unif_proj_, 1, GL_FALSE, (GLfloat*)this->proj_.mat);

	//頂点データ転送
	glVertexAttribPointer(colorRGBA.attr_point_, 3, GL_INT, GL_FALSE, 0, (GLint*)&coords[0]);
	glVertexAttribPointer(colorRGBA.attr_color_, 4, GL_INT, GL_FALSE, 0, (GLint*)&colors[0]);

	//描画
	glLineWidth(width);
	glDrawArrays(GL_LINE_LOOP, 0, coords.size());
	*/

	//attribute変数無効化
	glDisableVertexAttribArray(colorRGBA.attr_point_);
	glDisableVertexAttribArray(colorRGBA.attr_color_);
}

//ポリゴン描画
void fw::DrawWEGL::drawPolygons(const DrawCoords& coords, const DrawColors& colors)
{
}

//イメージ描画
void fw::DrawWEGL::drawImage(const std::CoordI& coord, const fw::Image& image)
{
}

//文字描画
void fw::DrawWEGL::drawString(const std::CoordI& coord, const wchar_t* const str)
{
}

//画面幅高さを取得
std::WH fw::DrawWEGL::getScreenWH()
{
	RECT rect = { 0 };
	(void)::GetClientRect(this->hWnd_, &rect);

	std::WH wh = { 0 };
	wh.width = std::int32_t(rect.right - rect.left);
	wh.height = std::int32_t(rect.bottom - rect.top);

	return wh;
}

#endif //DRAWIF_WEGL
