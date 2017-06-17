#include <cstdint>

namespace win32
{
	void StartApplication();
}


//メイン処理
std::int32_t main()
{
	win32::StartApplication();

	return 0;
}