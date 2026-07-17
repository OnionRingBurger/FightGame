#ifndef __MAIN_H__
#define __MAIN_H__

#include <memory>
#include <Windows.h>

#include "DirectX.h"
#include "ThreadSafeQueue.h"
#include "Loading.h"
#include "World.h"
#include "ModelCache.h"
#include "UICache.h"
#include "IScene.h"

class MainGame
{
public:

	MainGame();
	~MainGame() = default;
	void Init();
	void Uninit();

	void MainLoop(HWND a_hwnd, MSG& message);
	bool IsMouseLock();
	bool IsEnd();

private:
	/**
	* @name   LoadLoopUpdate
	* @brief  読み込まれたモデルをCacheに登録する
	* @detail 一度呼べば読み込まれている全てのモデル及びテクスチャを登録可能
	*/
	void LoadLoopUpdate();

	/**
	* @name   AdvanceUpdate
	* @brief  事前処理
	*/
	void AdvanceUpdate(float delatTime);

	/**
	* @name   Update
	* @brief  更新処理
	*/
	void Update();

	/**
	* @name   Draw
	* @brief  描画処理
	*/
	void Draw();

	/**
	* @name   PostFrameProcess
	* @brief  フレーム終了処理処理
	*/
	void PostFrameProcess();

	//! 現在のScene
	std::unique_ptr<IScene> scene;
	//! DXのレンダーターゲットビュー
	RenderTarget* pRTV;
	DepthStencil* pDSV;
	//! 更新処理が完了しているかどうか
	bool isInit = false;
	//! 終了処理が完了しているかどうか
	bool isEnd = false;

	//! 読み込みリクエストとリザルト
	ThreadSafeQueue<ModelLoadJob> modelJobQueue;
	ThreadSafeQueue<ModelLoadResult> modelResultQueue;
	ThreadSafeQueue<TextureLoadJob> textureJobQueue;
	ThreadSafeQueue<TextureLoadResult> textureResultQueue;
	ThreadSafeQueue<SoundLoadJob> soundJobQueue;
	ThreadSafeQueue<SoundLoadResult> soundResultQueue;

	//! 読み込み処理を回しているかどうか 
	std::atomic<bool> runningLoadLoop = true;
	
	void WorkerLoop(
		ThreadSafeQueue<ModelLoadJob>& a_modelJobQueue,
		ThreadSafeQueue<ModelLoadResult>& a_modelResultQueue,
		ThreadSafeQueue<TextureLoadJob>& a_textureJobQueue,
		ThreadSafeQueue<TextureLoadResult>& a_textureResultQueue,
		ThreadSafeQueue<SoundLoadJob>& a_soundJobQueue,
		ThreadSafeQueue<SoundLoadResult>& a_soundResultQueue,
		std::atomic<bool>& a_running);

	bool ChangeScene(std::string);

	void ResetFPS();

	ModelCache modelCache;
	UICache uiCache;
	Input input;
	std::queue<std::string> sceneChangeQueue;
	std::thread loadWorker;

	std::vector<modelLoadData> modelDatas;
	std::vector<std::string> textureDatas;

	// FPS処理用
	DWORD preExecTime;
	DWORD fpsOldTime;
	float processingTime;
	int fpsCount;
};

#endif // __MAIN_H__