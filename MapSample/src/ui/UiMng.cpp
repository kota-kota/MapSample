#include "UiMng.hpp"

#include "LocalImage.hpp"
#include "draw/DrawIF.hpp"
#include "image/Image.hpp"
#include "io/File.hpp"



//----------------------------------------------------------
//
// UI管理クラス
//
//----------------------------------------------------------

//コンストラクタ
ui::UiMng::UiMng(fw::DrawIF* drawIF) :
	drawIF_(drawIF), mapPos_(), screenPos_(), touchPos_(), dragPos_(), isTouchOn_(false), isDragOn_(false),
	texBasePos_(), localImage(nullptr)
{
	printf("[%s] DrawIF:0x%p\n", __FUNCTION__, drawIF);
	if (this->drawIF_ != nullptr) {
		//地図中心座標
		this->mapPos_ = { 63230028, 16092608, 0 };

		//画面中心座標
		std::WH screenwh = this->drawIF_->getScreenWH();
		this->screenPos_ = { screenwh.width / 2, screenwh.height / 2, 0 };

		//テクスチャ基準座標(画面左上になるように)
		this->texBasePos_ = { this->mapPos_.x - this->screenPos_.x, this->mapPos_.y + this->screenPos_.y, 0 };

		//ソフト持ち画像作成
		this->localImage = new LocalImage();
		this->localImage->create();
	}
}

//デストラクタ
ui::UiMng::~UiMng()
{
	if (this->localImage != nullptr) {
		delete this->localImage;
	}
}

//タッチON
void ui::UiMng::setTouchOn(std::CoordI touchPos)
{
	//タッチ座標を保持
	this->touchPos_ = touchPos;

	//タッチスクロールをONとする
	this->isTouchOn_ = true;
	this->isDragOn_ = false;
}

//タッチOFF
void ui::UiMng::setTouchOff()
{
	if (this->isTouchOn_) {
		//タッチスクロール

		//差分座標を計算
		std::CoordI diffPos;
		diffPos.x = this->screenPos_.x - this->touchPos_.x;
		diffPos.y = this->screenPos_.y - this->touchPos_.y;

		//地図座標に反映
		this->mapPos_.x -= diffPos.x;
		this->mapPos_.y += diffPos.y;
	}
	else {
		//何もしない
	}

	this->isTouchOn_ = false;
	this->isDragOn_ = false;
}

//ドラッグ
void ui::UiMng::setDrag(std::CoordI dragPos)
{
	//ドラッグスクロールをONとする
	this->isTouchOn_ = false;
	this->isDragOn_ = true;

	//差分座標を計算
	std::CoordI diffPos;
	diffPos.x = this->touchPos_.x - dragPos.x;
	diffPos.y = this->touchPos_.y - dragPos.y;

	//地図座標に反映
	this->mapPos_.x += diffPos.x;
	this->mapPos_.y -= diffPos.y;

	this->touchPos_ = dragPos;
}

//描画
void ui::UiMng::draw()
{
	this->drawIF_->makeCurrent(true);
	this->drawIF_->setup(this->mapPos_);
	{
		std::Color defColor = { 230, 231, 232, 255 };
		this->drawIF_->clear(defColor);
	}
	{
		std::CoordI texBasePos = this->texBasePos_;
		//std::int32_t xOffset = 200;
		//for (EN_LocalImageID id = EN_LocalImageID::BMP01; id <= EN_LocalImageID::BMP10; id = EN_LocalImageID(std::int32_t(id) + 1)) {
		//std::int32_t xOffset = 512;
		//for (EN_LocalImageID id = EN_LocalImageID::PNG01; id <= EN_LocalImageID::PNG09; id = EN_LocalImageID(std::int32_t(id) + 1)) {
		std::int32_t xOffset = 227;
		for (EN_LocalImageID id = EN_LocalImageID::JPEG01; id <= EN_LocalImageID::JPEG02; id = EN_LocalImageID(std::int32_t(id) + 1)) {

			//画像データを取得
			LocalImageData imageData;
			this->localImage->getImage(id, &imageData);

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

			this->drawIF_->drawImage(texBasePos, image);

			texBasePos.x += xOffset;
		}
	}
	{
		std::vector<std::Color> colors(1);
		colors[0] = { 255, 0, 0, 255 };
		std::vector<std::CoordI> coords(1);
		coords[0] = { 63230028, 16092608, 0 };
		this->drawIF_->drawPoints(coords, colors, 10.0f);
	}
	{
		std::vector<std::Color> colors(1);
		colors[0] = { 255, 0, 0, 255 };
		std::vector<std::CoordI> coords(5);
		coords[0] = { 63230028 - 100, 16092608 - 100, 0 };
		coords[1] = { 63230028 - 100, 16092608 + 100, 0 };
		coords[2] = { 63230028 + 100, 16092608 + 100, 0 };
		coords[3] = { 63230028 + 100, 16092608 - 100, 0 };
		coords[4] = { 63230028 - 100, 16092608 - 100, 0 };
		this->drawIF_->drawLines(coords, colors, 2.0f);
	}

	this->drawIF_->swapBuffers();
	this->drawIF_->makeCurrent(false);
}
