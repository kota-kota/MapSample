#include "File.hpp"

using std::string;
using cmn::Int32_t;
using cmn::Binary;
using io::File;

//コンストラクタ
File::File()
	: filePath_(), fileSize_(0), fp_(nullptr)
{
}

//デストラクタ
File::~File()
{
	this->close();
	this->filePath_.clear();
}

//作成
bool File::create(const string& filePath)
{
	if (!this->filePath_.empty()) {
		//作成済み
		return false;
	}

	this->filePath_ = filePath;

	return true;
}

//ファイルオープン
bool File::open(const string& mode)
{
	if (this->filePath_.empty()) {
		//未作成
		return false;
	}
	if (this->fp_ != nullptr) {
		//オープン済み
		return false;
	}

	//ファイルオープン
	(void)fopen_s(&this->fp_, this->filePath_.c_str(), mode.c_str());
	if (this->fp_ == nullptr) {
		//オープン失敗
		return false;
	}

	//ファイルサイズ取得
	fpos_t pos;
	(void)fseek(this->fp_, 0, SEEK_END);
	(void)fgetpos(this->fp_, &pos);
	(void)fseek(this->fp_, 0, SEEK_SET);
	this->fileSize_ = static_cast<size_t>(pos);

	return true;
}

//ファイルクローズ
void File::close()
{
	if (this->fp_ != nullptr) {
		//ファイルクローズ
		(void)fclose(this->fp_);
		this->fp_ = nullptr;
	}
	this->fileSize_ = 0;
}

//ファイル読み込み
bool File::read(const Int32_t offset, const size_t size, Binary& data)
{
	if (this->filePath_.empty()) {
		//未作成
		return false;
	}
	if (this->fp_ == nullptr) {
		//未オープン
		return false;
	}
	if (this->fileSize_ < (offset + size)) {
		//読み込み範囲がファイルサイズを超える
		return false;
	}
	if (data.size() < size) {
		//格納バッファのサイズが足りない
		return false;
	}

	FILE* fp = this->fp_;

	//読み込み位置を移動
	(void)fseek(this->fp_, offset, SEEK_SET);

	//ファイル読み込み
	size_t readSize = fread_s(&data[0], size, 1, size, fp);
	if (readSize != size) {
		//読み込み失敗
		return false;
	}

	return true;
}

//ファイルサイズ取得
size_t File::getFileSize()
{
	return this->fileSize_;
}
