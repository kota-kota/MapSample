# ---------------------------------------------------------
# 事前準備(Windows)
# ---------------------------------------------------------
・Visual Studio 2015


# ---------------------------------------------------------
# 事前準備(Android NDK)
# ---------------------------------------------------------
・ndk-build
  AndroidNDKのインストールパスを環境変数に設定しておく
  C:\Users\...\AppData\Local\Android\Sdk\ndk-bundle


# ---------------------------------------------------------
# zlib 1.2.8
#    https://zlib.net/
# ---------------------------------------------------------
1. zlibソースをダウンロードし展開しておく
   ※libpngのデフォルトがzlib1.2.8になっているため、1.2.8を使用する
   ※1.2.8以外を使用する場合は、libpngのビルド設定を弄る必要があるかも

2. libpngでビルドするため何もしない
   
   [ライブラリ]
   libpngのフォルダ
   [ヘッダ]
   zconf.h
   zlib.h


# ---------------------------------------------------------
# libpng 1.6.32
#    http://www.libpng.org/pub/png/libpng.html
# ---------------------------------------------------------
1. libpngソースをダウンロードし展開しておく
   ※zlibを展開した場所と同じフォルダ

2. projects\vstudio\vstduio.slnを開く
   VisualStudio2015を使用すると、アップグレードするか聞かれるため、「OK」とする

   ※zlib1.2.8以外のバージョンを使用する場合は、projects\vstudio\zlib.propsを修正する？
     <ZLibSrcDir>..\..\..\..\zlib-1.2.8</ZLibSrcDir>

3. x64ビルドを有効にする
   ソリューションを選択し「プロパティ」→「構成プロパティ」→「構成マネージャー」
   →アクティブソリューションプラットフォームのプルダウンから「新規作成」
   →新しいプラットフォームでx64を選択し「OK」

4. ReleseLibraryでWin32,x64をビルドする
   ※ビルドエラーが出るが、libpngとzlibのライブラリは成功しているので無視
   
   [ライブラリ]
   projects\vstudio\
      - x64\Release Library\
           - libpng.lib
           - zlib.lib
      - Release Library\
           - libpng.lib
           - zlib.lib
   [ヘッダ]
   png.h
   pngconf.h
   pngdebug.h
   pnginfo.h
   pnglibconf.h
   pngpriv.h
   pngstruct.h


# ---------------------------------------------------------
# freetype 2.8.0
#    https://www.freetype.org/
# ---------------------------------------------------------
1. freetypeソースをダウンロードし展開しておく

2. builds\windows\vc2010\freetype.slnを開く
   VisualStudio2015を使用すると、アップグレードするか聞かれるため、「OK」とする

3. Relese MultithreadedでWin32,x64をビルドする
   
   [ライブラリ]
   objs\vc2010\Win32\freetype28MT.lib
   objs\vc2010\x64\freetype28MT.lib
   [ヘッダ]
   include\以下全て


# ---------------------------------------------------------
# libxml2 
#    ftp://xmlsoft.org/libxml2/
# ---------------------------------------------------------
1. libxml2ソースをダウンロードする


# ---------------------------------------------------------
# ANGLE
#    https://github.com/Microsoft/angle
# ---------------------------------------------------------
1. Visual Studio 2015の変更で、Windows 10 SDK(10.0.14393)をインストールする
   「設定」→「アプリ」→「Microssft Visual Studio 2015」→「変更」→「変更」
    →「ユニバーサルWindowsアプリ開発ツール」にチェック
    →「次へ」→「更新」

2. ANGLEソースをチェックアウトする

3. winrt/10/src/angle.slnを開いてビルドする




1. 以下リポジトリからチェックアウトする
   https://github.com/Microsoft/angle
   
   本家はGoogleの以下リポジトリとなる
   https://github.com/google/angle
   しかし、WindowsでビルドするにはGypを利用する必要がある
   そのため、Microsoftのリポジトリから利用する

2. src/angle.slnを開いてビルドする
   
   <ビルドエラー>
   エラー内容:
     error MSB8036: The Windows SDK version 10.0.14393.0 was not found.
   対処法:
     ソリューションを右クリック→「ショリューションの再ターゲット」→「8.1」を選択
   
   エラー内容:
     include ファイルを開けません。'd3d11_3.h':No such file or directory
   対処法:
     「設定」→「アプリ」→「Microssft Visual Studio 2015」→「変更」→「変更」
      →ユニバーサルWindowsアプリ開発ツールの「Windows 10 SDK (10.0.10240)」にチェック
      →「次へ」→「更新」


2. winrt/10/src/angle.slnを開いてビルドする

   VisualStudioを開くと、見つからない機能のインストール画面が出てくるので、
   「ユニバーサルWindowsアプリケーション」
   「Windows 10 SDK 10.0.10240」
   をインストールする（VisualStudioが指示する流れに身を任せて）
   
   Windows10以外は、src/angle.slnを使用するらしい

2. [Windows10 + VisualStudio2015]
   src/angle.slnを開いてビルドする

   <ビルドエラー>
   エラー内容:
     error MSB8036: The Windows SDK version 10.0.14393.0 was not found.
   「設定」→「アプリ」→「Microssft Visual Studio 2015」→「変更」→「変更」
    →ユニバーサルWindowsアプリ開発ツールの「Windows 10 SDK (10.0.10240)」にチェック
    →「次へ」

   <ビルドエラー>
   エラー内容:
     d3d11_3.h: No Such File or Directory
   対処法
     「Windows 10 SDK 10.0.10240」をインストールする


