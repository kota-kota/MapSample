#include "utl/File.hpp"

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::string;
using std::vector;
using utl::File;

//コンストラクタ
File::File()
	: mFilePath(), mFileSize(0), mFp(nullptr)
{
}

//デストラクタ
File::~File()
{
	this->close();
	this->mFilePath.clear();
}

//作成
bool File::create(const string& filePath)
{
	if (!this->mFilePath.empty()) {
		//作成済み
		return false;
	}

	this->mFilePath = filePath;

	return true;
}

//ファイルオープン
bool File::open(const string& mode)
{
	if (this->mFilePath.empty()) {
		//未作成
		return false;
	}
	if (this->mFp != nullptr) {
		//オープン済み
		return false;
	}

	//ファイルオープン
	(void)fopen_s(&this->mFp, this->mFilePath.c_str(), mode.c_str());
	if (this->mFp == nullptr) {
		//オープン失敗
		return false;
	}

	//ファイルサイズ取得
	fpos_t pos;
	(void)fseek(this->mFp, 0, SEEK_END);
	(void)fgetpos(this->mFp, &pos);
	(void)fseek(this->mFp, 0, SEEK_SET);
	this->mFileSize = static_cast<size_t>(pos);

	return true;
}

//ファイルクローズ
void File::close()
{
	if (this->mFp != nullptr) {
		//ファイルクローズ
		(void)fclose(this->mFp);
		this->mFp = nullptr;
	}
	this->mFileSize = 0;
}

//ファイル読み込み
bool File::read(const int32_t offset, const size_t size, vector<uint8_t>* data)
{
	if (this->mFilePath.empty()) {
		//未作成
		return false;
	}
	if (this->mFp == nullptr) {
		//未オープン
		return false;
	}
	if (this->mFileSize < (offset + size)) {
		//読み込み範囲がファイルサイズを超える
		return false;
	}
	if (data->size() < size) {
		//格納バッファのサイズが足りない
		return false;
	}

	FILE* fp = this->mFp;

	//読み込み位置を移動
	(void)fseek(this->mFp, offset, SEEK_SET);

	//ファイル読み込み
	size_t readSize = fread_s(&(*data)[0], size, 1, size, fp);
	if (readSize != size) {
		//読み込み失敗
		return false;
	}

	return true;
}

//ファイルサイズ取得
size_t File::getFileSize()
{
	return this->mFileSize;
}
