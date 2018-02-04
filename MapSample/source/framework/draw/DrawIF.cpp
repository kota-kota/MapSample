#include "DrawIF.hpp"
#include "image/Font.hpp"


//----------------------------------------------------------
//
// 描画I/Fクラス
//
//----------------------------------------------------------

//コンストラクタ
fw::DrawIF::DrawIF() :
	font_(nullptr)
{
	//フォントオブジェクトを作成
	this->font_ = new fw::Font();
}

//デストラクタ
fw::DrawIF::~DrawIF()
{
	if (this->font_ != nullptr) {
		//フォントオブジェクトを解放
		delete this->font_;
	}
}
