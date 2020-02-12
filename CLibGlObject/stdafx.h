// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_NO_WARNINGS 1

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。

///// メモリ・リーク
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。

#include "LibChImage.h"
#include "LibChData.h"
#include "LibCAlgebra.h"

#include "../Lib/LibCGlObject.h"

//#pragma comment(lib , "glew32.lib")