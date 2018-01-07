#include "UiScreen.hpp"
#include "draw/DrawIF.hpp"
#include "image/LocalImage.hpp"


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
	std::Position mapPos = this->status_.mapPos_;

	//描画
	this->viewData_.draw(drawIF, mapPos);

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
void ui::UiScreen::procButtonMove(const std::Position& buttonPos)
{
	//ドラッグスクロール
	if ((this->buttonEvent_ == LEFT_DOWN) || (this->buttonEvent_ == LEFT_MOVE)) {
		//差分座標を計算
		std::Position diffPos = { 0 };
		diffPos.x = this->touchPos_.x - buttonPos.x;
		diffPos.y = this->touchPos_.y - buttonPos.y;

		//地図座標に反映
		this->status_.mapPos_.x += diffPos.x;
		this->status_.mapPos_.y -= diffPos.y;

		//描画更新必要
		this->status_.isUpdate_ = std::EN_OffOn::ON;

		//状態を更新
		this->touchPos_ = buttonPos;
		this->buttonEvent_ = LEFT_MOVE;
	}
}
