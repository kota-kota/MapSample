#include "UiScreen.hpp"
#include "draw/DrawIF.hpp"
#include "image/LocalImage.hpp"


//----------------------------------------------------------
//
// UI画面クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiScreen::UiScreen(fw::DrawIF* const drawIF, fw::LocalImage* const localImage, const std::CoordI mapPos) :
	status_(), touchPos_(), dragPos_(), buttonEvent_(ui::EN_ButtonEvent::INVALID)
{
	//画面中心座標
	std::WH screenwh = drawIF->getScreenWH();
	std::Position screenPos = { screenwh.width / 2, screenwh.height / 2 };

	//地図エリア
	std::Area mapArea = { mapPos.x - screenPos.x, mapPos.y - screenPos.y, mapPos.x + screenPos.x, mapPos.y + screenPos.y };

	//ステータスを初期化
	this->status_.isUpdate_ = std::EN_OffOn::ON;
	this->status_.drawIF_ = drawIF;
	this->status_.localImage_ = localImage;
	this->status_.mapPos_ = mapPos;
	this->status_.mapArea_ = mapArea;
	this->status_.screenPos_ = screenPos;
}

//デストラクタ
ui::UiScreen::~UiScreen()
{
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
	fw::LocalImage* localImage = this->status_.localImage_;
	std::CoordI mapPos = this->status_.mapPos_;
	std::Area mapArea = this->status_.mapArea_;
	std::Position screenPos = this->status_.screenPos_;

	drawIF->makeCurrent(true);
	drawIF->setup(mapPos);

	//画面クリア
	{
		std::Color defColor = { 230, 231, 232, 255 };
		drawIF->clear(defColor);
	}

	//イメージ描画
	{
		std::CoordI texBasePos = { mapArea.xmin, mapArea.ymax };

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

			drawIF->drawImage(texBasePos, image);

			texBasePos.x += xOffset;
		}
	}

	//点描画
	{
		std::vector<std::Color> colors(1);
		colors[0] = { 255, 0, 0, 255 };
		std::vector<std::CoordI> coords(1);
		coords[0] = { 63230028, 16092608, 0 };
		drawIF->drawPoints(coords, colors, 10.0f);
	}

	//ライン描画
	{
		std::vector<std::Color> colors(1);
		colors[0] = { 255, 0, 0, 255 };
		std::vector<std::CoordI> coords(5);
		coords[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coords[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coords[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coords[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coords[4] = { 63230028 - 100, 16092608 - 100, 0 };
		drawIF->drawLines(coords, colors, 2.0f);
	}

	//文字描画
	{
		std::CoordI coord = { 63230028, 16092608, 0 };
		//this->drawIF_->drawString(coord, L"hijkl,HIJKL<ASCII>/OpenGL. ^-^");
		drawIF->drawString(coord, L"1234567890");
		coord.y -= 100;
		drawIF->drawString(coord, L"東京ドームアトラクション、ですわよ。");
	}

	drawIF->swapBuffers();
	drawIF->makeCurrent(false);

	//描画完了したので、描画更新不要
	this->status_.isUpdate_ = std::EN_OffOn::OFF;
}

//ボタンイベント処理:LEFT_DOWN
void ui::UiScreen::procButtonLeftDown(const std::CoordI buttonPos)
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
		std::CoordI diffPos;
		diffPos.x = this->status_.screenPos_.x - this->touchPos_.x;
		diffPos.y = this->status_.screenPos_.y - this->touchPos_.y;

		//地図座標に反映
		this->status_.mapPos_.x -= diffPos.x;
		this->status_.mapPos_.y += diffPos.y;

		//描画更新必要
		this->status_.isUpdate_ = std::EN_OffOn::ON;
	}
	else {
		//何もしない
	}

	this->buttonEvent_ = INVALID;
}

//ボタンイベント処理:MOVE
void ui::UiScreen::procButtonMove(const std::CoordI buttonPos)
{
	//ドラッグスクロール
	if ((this->buttonEvent_ == LEFT_DOWN) || (this->buttonEvent_ == LEFT_MOVE)) {
		//差分座標を計算
		std::CoordI diffPos;
		diffPos.x = this->touchPos_.x - buttonPos.x;
		diffPos.y = this->touchPos_.y - buttonPos.y;

		//地図座標に反映
		this->status_.mapPos_.x += diffPos.x;
		this->status_.mapPos_.y -= diffPos.y;

		this->touchPos_ = buttonPos;

		//描画更新必要
		this->status_.isUpdate_ = std::EN_OffOn::ON;

		this->buttonEvent_ = LEFT_MOVE;
	}
}