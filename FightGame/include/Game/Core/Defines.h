#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <assert.h>
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>

static const int FPS = 60;
static const float fFPS = static_cast<float>(FPS);

// リソースパス
#define ASSET(path)	"Assets/"path

// 3D空間定義
#define CMETER(value) (value * 0.01f)
#define METER(value) (value * 1.0f)
#define MSEC(value) (value / fFPS)
#define CMSEC(value) MSEC(CMETER(value))
static const float GRAVITY = 0.98f;

// タイトル
static const char* APP_TITLE = "Fight";

// 画面サイズ
static const int SCREEN_WIDTH	= 1280;
static const int SCREEN_HEIGHT	= 720;

constexpr int kMouseFixedPosX = 800;
constexpr int kMouseFixedPosY = 400;

// シェーダー
constexpr int kMaxArea = 100;

constexpr const char* kDataPath = "Assets/Data/EntityData.json";
constexpr const char* kDataTestPath = "Assets/Data/EntityDataTest.json";
constexpr const char* kAttackDataPath = "Assets/Data/AttackData.json";

constexpr const char* kTestEffect = "Freeze.efk";

constexpr const char* kSoundAssetPath = "Assets/Sound/";
constexpr const char* kFontAssetPath = "Assets/Font/";

// #define CREATEGAME

// グリッドサイズ(デバッグ用
static const int	DEBUG_GRID_NUM		= 10;			// グリッド中心から端までの線の本数
static const float	DEBUG_GRID_MARGIN	= METER(1.0f);	// グリッド配置幅

#endif // __DEFINES_H__