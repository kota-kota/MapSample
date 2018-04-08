#ifndef INCLUDED_WINDOWCREATER_HPP
#define INCLUDED_WINDOWCREATER_HPP

namespace window{

	class WindowCreater {
	public:
		//デストラクタ
		virtual ~WindowCreater() {}
		//ウィンドウ作成
		virtual int create() = 0;
		//ウィンドウ開始
		virtual int start() = 0;
	};
}

#endif //INCLUDED_WINDOWCREATER_HPP
