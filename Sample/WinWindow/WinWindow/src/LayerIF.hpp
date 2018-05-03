#ifndef INCLUDED_LAYERIF_HPP
#define INCLUDED_LAYERIF_HPP

#include "Type.hpp"
#include "DrawIF.hpp"

namespace fw {

	//最新レイヤーID
	static std::int32_t lastLayerId = 0;

	/**
	 * レイヤー操作インターフェースクラス
	 */
	class LayerIF {
		std::int32_t	id_;
		std::int32_t	width_;
		std::int32_t	height_;
		DrawIF*			drawIF_;

	public:
		//コンストラクタ
		LayerIF(const std::int32_t width, const std::int32_t height, DrawIF* const drawIF)
			: id_(0), width_(width), height_(height), drawIF_(drawIF)
		{
			lastLayerId++;
			this->id_ = lastLayerId;
		}
		//デストラクタ
		virtual ~LayerIF()
		{
		}
		//レイヤーID取得
		std::int32_t getId()
		{
			return this->id_;
		}
		//レイヤーサイズ取得
		void getSize(std::int32_t* const width, std::int32_t* const height)
		{
			if (width != nullptr) { *width = this->width_; }
			if (height != nullptr) { *height = this->height_; }
		}
		//描画インターフェース取得
		DrawIF* getDrawIF()
		{
			return this->drawIF_;
		}
		//描画開始
		virtual void beginDraw() = 0;
		//描画終了
		virtual void endDraw() = 0;

	private:
		//コピーコンストラクタ(禁止)
		LayerIF(const LayerIF& org) = delete;
		//コピー代入演算子(禁止)
		void operator=(const LayerIF& org) = delete;
	};
}

#endif //INCLUDED_LAYERIF_HPP
