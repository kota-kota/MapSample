#include "UiScreen.hpp"
#include "draw/DrawIF.hpp"
#include "image/LocalImage.hpp"


namespace {
	//定数定義
	const std::double_t	VIEW_EYE_Z = 10.0;
	const std::double_t	PROJ_NEAR = 1.0;
	const std::double_t PROJ_FAR = 1000.0;
}


//----------------------------------------------------------
//
// UI画面クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiScreen::UiScreen(fw::DrawIF* const drawIF, fw::LocalImage* const localImage, const std::Position& mapPos) :
	status_(), touchPos_(), dragPos_(), buttonEvent_(ui::EN_ButtonEvent::INVALID)
{
	//画面中心座標
	std::WH screenwh = drawIF->getScreenWH();
	std::Position screenPos = { screenwh.width / 2, screenwh.height / 2 };

	//地図エリア
	std::AreaI mapArea = { mapPos.x - screenPos.x, mapPos.y - screenPos.y, mapPos.x + screenPos.x, mapPos.y + screenPos.y };

	//ステータスを初期化
	this->status_.isUpdate_ = std::EN_OffOn::ON;
	this->status_.drawIF_ = drawIF;
	this->status_.localImage_ = localImage;
	this->status_.screenPos_ = screenPos;
	this->status_.mapPos_ = mapPos;
	this->status_.mapArea_ = mapArea;
	this->initDrawStatus();
}

//デストラクタ
ui::UiScreen::~UiScreen()
{
}

//表示データ作成
void ui::UiScreen::makeViewData()
{
	fw::LocalImage* localImage = this->status_.localImage_;
	std::AreaI mapArea = this->status_.mapArea_;

	//背景色
	//std::ColorUB backColor = { 230, 231, 232, 255 };
	std::ColorUB backColor = { 255, 150, 150, 255 };
	this->viewData_.setBackColor(backColor);

	//イメージ
	{
		std::CoordI texBasePos = { mapArea.xmin, mapArea.ymax, 0 };

		std::int32_t xOffset = 200;
		for (std::uint16_t id = fw::D_IMAGEID_BMP_01; id <= fw::D_IMAGEID_BMP_0A; id++) {
			//std::int32_t xOffset = 512;
			//for (std::uint16_t id = fw::D_IMAGEID_PNG_01; id <= fw::D_IMAGEID_PNG_09; id++) {
			//std::int32_t xOffset = 227;
			//for (std::uint16_t id = fw::D_IMAGEID_JPEG_01; id <= fw::D_IMAGEID_JPEG_02; id++) {

			//画像データを取得
			fw::LocalImageData imageData;
			localImage->getImage(id, &imageData);

			fw::Image image;
			image.id_ = imageData.id_;
			image.format_ = imageData.format_;
			image.type_ = fw::EN_ImageType::D_IMAGETYPE_LOCAL;
			image.isFlip_ = 0;
			image.body_.data_ = imageData.body_;
			image.body_.dataSize_ = imageData.bodySize_;
			image.body_.width_ = 0;
			image.body_.height_ = 0;
			image.body_.isPixelFlip_ = 0;
			image.body_.isChgPallete_ = 0;
			image.body_.palleteNum_ = 0;
			image.body_.pallete_ = nullptr;
			image.isBlend_ = 0;

			this->viewData_.setDrawParts(new ViewImage(texBasePos, image));

			texBasePos.x += xOffset;
		}
	}

	//点
	{
		const std::int32_t coordNum = 3;
		const std::ColorUB pointColor = { 255, 0, 0, 255 };
		const std::CoordI pointCoord = { std::D_MAP_POSITION.x - 350, std::D_MAP_POSITION.y, 0 };
		const std::CoordI offset[coordNum] = {
			{ 0, 0, 0 },
			{ 100, 0, 0 },
			{ 200, 0, 0 },
		};

		fw::DrawColors colors(coordNum);
		fw::DrawCoords coords(coordNum);
		for (std::int32_t i = 0; i < coordNum; i++) {
			colors[i] = pointColor;
			coords[i] = { pointCoord.x + offset[i].x, pointCoord.y + offset[i].y, pointCoord.z + offset[i].z };
		}
		this->viewData_.setDrawParts(new ViewPoint(coords, colors, 10.0F));
	}

	//ライン
	{
		const std::int32_t coordNum = 5;
		const std::ColorUB lineColor = { 255, 0, 0, 255 };
		const std::CoordI lineCoord = { std::D_MAP_POSITION.x - 350, std::D_MAP_POSITION.y - 30, 0 };
		const std::CoordI offset[coordNum] = {
			{ 0, 0, 0 },
			{ 30, 5, 0 },
			{ 60, -15, 0 },
			{ 90, 10, 0 },
			{ 120, 0, 0 },
		};

		fw::DrawColors colors(coordNum);
		fw::DrawCoords coords(coordNum);
		for (std::int32_t i = 0; i < coordNum; i++) {
			colors[i] = lineColor;
			coords[i] = { lineCoord.x + offset[i].x, lineCoord.y + offset[i].y, lineCoord.z + offset[i].z };
		}
		this->viewData_.setDrawParts(new ViewLine(coords, colors, 2.0F));
	}

	//ポリゴン
	{
		const std::int32_t coordNum = 6;
		const std::ColorUB polyColor = { 255, 0, 0, 255 };
		const std::CoordI polyCoord = { std::D_MAP_POSITION.x - 350, std::D_MAP_POSITION.y - 60, 0 };
		const std::CoordI offset[coordNum] = {
			{ 0, 0, 0 },
			{ 0, -15, 0 },
			{ 200, -10, 0 },
			{ 200, -30, 0 },
			{ 500, 10, 0 },
			{ 500, 0, 0 },
		};

		fw::DrawColors colors(coordNum);
		fw::DrawCoords coords(coordNum);
		for (std::int32_t i = 0; i < coordNum; i++) {
			colors[i] = polyColor;
			coords[i] = { polyCoord.x + offset[i].x, polyCoord.y + offset[i].y, polyCoord.z + offset[i].z };
		}
		this->viewData_.setDrawParts(new ViewPolygon(coords, colors));
	}

	//文字
	{
		std::CoordI coord = { 63230028, 16092608, 0 };
		this->viewData_.setDrawParts(new ViewString(coord, L"hijkl,HIJKL<ASCII>/OpenGL. ^-^"));
		coord.y -= 100;
		this->viewData_.setDrawParts(new ViewString(coord, L"東京ドームアトラクション、ですわよ。"));
	}
}

//描画更新必要有無
bool ui::UiScreen::isUpdateDraw()
{
	return (this->status_.isUpdate_ == std::EN_OffOn::ON) ? true : false;
}

//描画
void ui::UiScreen::draw()
{
	fw::DrawIF* drawIF = this->status_.drawIF_;

	//描画
	drawIF->makeCurrent(true);
	drawIF->setup(this->status_.drawStatus_);
	this->viewData_.draw(drawIF);
	drawIF->swapBuffers();
	drawIF->makeCurrent(false);

	//描画完了したので、描画更新不要
	this->status_.isUpdate_ = std::EN_OffOn::OFF;
}

//ボタンイベント処理:LEFT_DOWN
void ui::UiScreen::procButtonLeftDown(const std::Position& buttonPos)
{
	//タッチ座標を保持
	this->touchPos_ = buttonPos;

	this->buttonEvent_ = LEFT_DOWN;
}

//ボタンイベント処理:LEFT_UP
void ui::UiScreen::procButtonLeftUp()
{
	if (this->buttonEvent_ == LEFT_DOWN) {
		//タッチスクロール

		//差分座標を計算
		std::Position diffPos = { 0 };
		diffPos.x = this->status_.screenPos_.x - this->touchPos_.x;
		diffPos.y = this->status_.screenPos_.y - this->touchPos_.y;

		//中心座標を移動
		this->moveMapPosition(-diffPos.x, diffPos.y);

		//描画更新必要
		this->status_.isUpdate_ = std::EN_OffOn::ON;
	}
	else {
		//何もしない
	}

	this->buttonEvent_ = INVALID;
}

//ホイールイベント処理:WHEEL_FORWARD
void ui::UiScreen::procButtonWheelForward()
{
	this->status_.drawStatus_.viewEye_.z += 1.0;
	this->updateViewMatrix();

	//描画更新必要
	this->status_.isUpdate_ = std::EN_OffOn::ON;
}

//ホイールイベント処理:WHEEL_BACKWARD
void ui::UiScreen::procButtonWheelBackward()
{
	this->status_.drawStatus_.viewEye_.z -= 1.0;
	this->updateViewMatrix();

	//描画更新必要
	this->status_.isUpdate_ = std::EN_OffOn::ON;
}

//ボタンイベント処理:MOVE
void ui::UiScreen::procButtonMove(const std::Position& buttonPos)
{
	//ドラッグスクロール
	if ((this->buttonEvent_ == LEFT_DOWN) || (this->buttonEvent_ == LEFT_MOVE)) {
		//差分座標を計算
		std::Position diffPos = { 0 };
		diffPos.x = this->touchPos_.x - buttonPos.x;
		diffPos.y = this->touchPos_.y - buttonPos.y;

		//中心座標を移動
		this->moveMapPosition(diffPos.x, -diffPos.y);

		//描画更新必要
		this->status_.isUpdate_ = std::EN_OffOn::ON;

		//状態を更新
		this->touchPos_ = buttonPos;
		this->buttonEvent_ = LEFT_MOVE;
	}
}

//描画ステータス初期化
void ui::UiScreen::initDrawStatus()
{
	const std::WH wh = this->status_.drawIF_->getScreenWH();
	const std::Position mapPos = this->status_.mapPos_;

	//ビューポート
	this->status_.drawStatus_.viewport_ = { 0, 0, wh.width, wh.height };

	//ビュー変換行列
	this->status_.drawStatus_.viewEye_ = { std::double_t(mapPos.x), std::double_t(mapPos.y), VIEW_EYE_Z };
	this->status_.drawStatus_.viewLook_ = { std::double_t(mapPos.x), std::double_t(mapPos.y), 0.0 };
	this->status_.drawStatus_.viewUp_ = { 0.0, 1.0, 0.0 };
	this->updateViewMatrix();

	//プロジェクション変換行列(視野空間を計算)
	const std::double_t correct = VIEW_EYE_Z / PROJ_NEAR;
	const std::double_t width = (std::double_t(wh.width) / 2.0) / correct;
	const std::double_t height = (std::double_t(wh.height) / 2.0) / correct;
	std::double_t left = -width;
	std::double_t right = width;
	std::double_t bottom = -height;
	std::double_t top = height;
	std::double_t znear = PROJ_NEAR;
	std::double_t zfar = PROJ_FAR;
	this->status_.drawStatus_.projMat_ = fw::Math::frustumMatrix(left, right, bottom, top, znear, zfar);
}

//中心座標を移動
void ui::UiScreen::moveMapPosition(const std::int32_t dx, const std::int32_t dy)
{
	//地図座標に反映
	this->status_.mapPos_.x += dx;
	this->status_.mapPos_.y += dy;

	//ビュー変換行列の更新
	this->status_.drawStatus_.viewEye_.x += std::double_t(dx);
	this->status_.drawStatus_.viewEye_.y += std::double_t(dy);
	this->status_.drawStatus_.viewLook_.x += std::double_t(dx);
	this->status_.drawStatus_.viewLook_.y += std::double_t(dy);
	this->updateViewMatrix();
}

//ビュー変換行列を更新
void ui::UiScreen::updateViewMatrix()
{
	const fw::VectorD& eye = this->status_.drawStatus_.viewEye_;
	const fw::VectorD& look = this->status_.drawStatus_.viewLook_;
	const fw::VectorD& up = this->status_.drawStatus_.viewUp_;
	this->status_.drawStatus_.viewMat_ = fw::Math::lookMatrix(eye, look, up);
}
