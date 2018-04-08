#ifndef INCLUDED_WINDOWIF_HPP
#define INCLUDED_WINDOWIF_HPP

namespace window{

	class WindowIF {
	public:
		//デストラクタ
		virtual ~WindowIF() {}
		//ウィンドウ作成
		virtual int create() = 0;
		//ウィンドウ開始
		virtual int start() = 0;
	};
}

#endif //INCLUDED_WINDOWIF_HPP
