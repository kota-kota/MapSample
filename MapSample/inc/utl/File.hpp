#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace utl
{
	class File {
		//メンバ変数
		std::string		mFilePath;
		size_t			mFileSize;
		FILE*			mFp;

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
		bool read(const std::int32_t offset, const size_t size, std::vector<std::uint8_t>* data);
		//ファイルサイズ取得
		size_t getFileSize();
	};
}
