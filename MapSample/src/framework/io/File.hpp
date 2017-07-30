#ifndef INCLUDED_FILE
#define INCLUDED_FILE

#include "Common.hpp"
#include <string>

namespace io
{
	class File {
		//メンバ変数
		std::string		filePath_;
		size_t			fileSize_;
		FILE*			fp_;

	public:
		//コンストラクタ
		File();
		//デストラクタ
		~File();
		//作成
		bool create(const std::string& filePath);
		//ファイルオープン
		bool open(const std::string& mode);
		//ファイルクローズ
		void close();
		//ファイル読み込み
		bool read(const cmn::Int32_t offset, const size_t size, cmn::Binary& data);
		//ファイルサイズ取得
		size_t getFileSize();
	};
}

#endif //INCLUDED_FILE
