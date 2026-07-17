#include <memory>
#include <vector>

#include "Main.h"
#include "Geometory.h"
#include "Sprite.h"
#include "Input.h"
#include "Defines.h"
#include "ShaderList.h"
#include "imGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"
#include "DebugConsole.h"
#include "Debug.h"

#include "GameScene.h"
#include "LoadScene.h"
#include "TestScene.h"

MainGame::MainGame()
	: scene(nullptr)
	, pRTV(nullptr)
	, pDSV(nullptr)
	, fpsOldTime(0.0f)
	, preExecTime(0.0f)
	, processingTime(0.0f)
	, fpsCount(0)
{
	uiCache = UICache();
	modelCache = ModelCache();
	input = Input();

}


// TODO ランダムになっていないので修正
void MainGame::Init()
{

	// DirectX初期化	
	pRTV = GetDefaultRTV();
	pDSV = GetDefaultDSV();

	// 他機能初期化
	Geometory::Init();
	Sprite::Init();
	ShaderList::Init();
	SetDepthTest(DEPTH_TEST_TRUE);

	// TODO 処理中にMain側が終了しDirectXがUninitされると壊れるため修正する
	runningLoadLoop = true;
	// Load用ループを開始
	loadWorker = std::thread(
		&MainGame::WorkerLoop,
		this,
		std::ref(modelJobQueue),
		std::ref(modelResultQueue),
		std::ref(textureJobQueue),
		std::ref(textureResultQueue),
		std::ref(soundJobQueue),
		std::ref(soundResultQueue),
		std::ref(runningLoadLoop));
	

	modelDatas =
	{
		{"bricktexture1", 0.005f},
		{"assaultRifle", 5.3f},
		{"ghost_FINAL", 0.13f},
		{"SM_SimpleTunnel", 1.0f},
		{"Box", 1.0f},
		{"Ball", 1.0f},
		{"Laser", 1.0f},
		{"LaserPoint", 1.0f},
		{"Dark", 20.0f},
		{"FireAnimation", 1.0f},
		{"Light", 20.0f},
		{"low_poly_treesNXT_5flat", 1.0f},
		{"bolita_pinchos_pintada", 0.01f},
		{"GreekPillar", 50.0f},
		{"track_txt", 7.0f},
		{"Minecart", 0.06f},
		{"CrackedTombstone", 5.0f}
	};

	textureDatas =
	{
		"TitleLogo",
		"Shadow",
		"Mask",
		"DarkMask",
		"Reticle",
		"DarkGameOver",
		"PleaseKey",
		"WhiteFade",
		"DarkFade",
		"DarkFadeMini",
		"WhiteFadeBack",
		"GameOver",
		"GameClear",
		"GameClearText",
		"GameStart",
		"Speedline",
		"UIGauge",
		"UIFrame",
		"UIBack",
		"ShotEffect",
		"DamageEffect",
		"DamageDirectionEffect",
		"ShotIcon",
		"WaitShot",
		"CanShotReady",
		"LeftClick",
		"HitEffectRay",
		"Blue",
		"Alpha",
		"Star",
		"Red",
		"Purple",
		"Green"
	};

	ChangeScene("Load");

	input.RegisterKey("GameEnd", VK_ESCAPE);
	input.RegisterKey("DebugConsole", VK_F1);
	

	// Soundの初期化
	InitSound();
	
	isInit = true;
}

void MainGame::Uninit()
{
	if (!isInit)
	{
		DebugMessage("初期化に失敗しているためメインループから抜けます。");
		return;
	}

	UninitSound();
	
	scene = nullptr;

	runningLoadLoop = false;
	loadWorker.detach();

	ShaderList::Uninit();
	Sprite::Uninit();
	Geometory::Uninit();
}

void MainGame::AdvanceUpdate(float deltaTime)
{
	input.UpdateInput();
	scene->AdvanceUpdate(deltaTime);
	LoadLoopUpdate();
}


void MainGame::MainLoop(HWND a_hwnd, MSG& message)
{
	if (!isInit)
	{
		DebugMessage("初期化に失敗しているためメインループから抜けます。");
		return;
	}

	// ランダム初期化
	std::srand((unsigned)time(NULL));

	//--- FPS制御
	timeBeginPeriod(1);
	ResetFPS();


	//--- ウィンドウの管理
	while (!IsEnd())
	{

		if (PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&message, NULL, 0, 0))
			{
				break;
			}
			else
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		else
		{
			DWORD nowTime = timeGetTime();
			float diff = static_cast<float>(nowTime - preExecTime);
			if (diff >= 1000.0f / fFPS)
			{

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				if (nowTime - fpsOldTime >= 1000)
				{
					processingTime = static_cast<float>(timeGetTime() - nowTime);
				}

				AdvanceUpdate(diff);
				if (true)
				{
					SetCursorPos(kMouseFixedPosX, kMouseFixedPosY);
				}
				ShowCursor(false);
				Update();
				Draw();
				PostFrameProcess();

				// FPSを更新
				preExecTime = nowTime;
				fpsCount++;
				// IMGUIを更新
				ImGui::EndFrame();

			}
			if (nowTime - fpsOldTime >= 1000)
			{
#ifdef _DEBUG
				DebugConsole::SetDrawPos(1, 2);
				std::cout << "FPS::" << fpsCount << std::endl;
				std::cout << "直前の処理時間::" << processingTime / 1000 << std::endl;
#endif
				fpsCount = 0;
				fpsOldTime = nowTime;
			}
		}
	}

}

// TODO 後に修正
bool MainGame::IsMouseLock()
{
	return false;
}

bool MainGame::IsEnd()
{
	return isEnd;
}


void MainGame::LoadLoopUpdate()
{
	// 各アセットの読み込み

	while (!modelResultQueue.IsEnpty())
	{
		ModelLoadResult result;
		if (modelResultQueue.TryPop(result));
		modelCache.RegisterModel(result.key, result.data);
	}

	while (!textureResultQueue.IsEnpty())
	{
		TextureLoadResult result;
		if (textureResultQueue.TryPop(result));
		uiCache.RegisterModel(result.key, result.data);
	}
	// TODO Soundを読み込めるようにする
}

void MainGame::Update()
{
#ifdef _DEBUG
	if (input.IsRegisterTrigger("DebugConsole")) DebugConsole::ToggleConsole();
#endif
	scene->Update();

}

void MainGame::Draw()
{
	BeginDrawDirectX();
	SetRenderTargets(1, &pRTV, pDSV);

	// 軸線の表示
#ifdef _DEBUG
	// グリッド
	DirectX::XMFLOAT4 lineColor(0.5f, 0.5f, 0.5f, 1.0f);
	float size = DEBUG_GRID_NUM * DEBUG_GRID_MARGIN;
	for (int i = 1; i <= DEBUG_GRID_NUM; ++i)
	{
		float grid = i * DEBUG_GRID_MARGIN;
		DirectX::XMFLOAT3 pos[2] = {
			DirectX::XMFLOAT3(grid, 0.0f, size),
			DirectX::XMFLOAT3(grid, 0.0f,-size),
		};
		Geometory::AddLine(pos[0], pos[1], lineColor);
		pos[0].x = pos[1].x = -grid;
		Geometory::AddLine(pos[0], pos[1], lineColor);
		pos[0].x = size;
		pos[1].x = -size;
		pos[0].z = pos[1].z = grid;
		Geometory::AddLine(pos[0], pos[1], lineColor);
		pos[0].z = pos[1].z = -grid;
		Geometory::AddLine(pos[0], pos[1], lineColor);
	}
	// 軸
	//Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(size, 0, 0), DirectX::XMFLOAT4(1, 0, 0, 1));
	//Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, size, 0), DirectX::XMFLOAT4(0, 1, 0, 1));
	//Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, size), DirectX::XMFLOAT4(0, 0, 1, 1));
	//Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(-size, 0, 0), DirectX::XMFLOAT4(0, 0, 0, 1));
	//Geometory::AddLine(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, -size), DirectX::XMFLOAT4(0, 0, 0, 1));

	Geometory::DrawLines();

	// カメラの値
	static bool camAutoSwitch = false;
	static bool camUpDownSwitch = true;
	static float camAutoRotate = 1.0f;
	/*if (input.IsRegisterTrigger(VK_RETURN)) {
		camAutoSwitch ^= true;
	}
	if (input.IsRegisterTrigger(VK_SPACE)) {
		camUpDownSwitch ^= true;
	}*/

	DirectX::XMVECTOR camPos;
	if (camAutoSwitch) {
		camAutoRotate += 0.01f;
	}
	camPos = DirectX::XMVectorSet(
		cosf(camAutoRotate) * 5.0f,
		3.5f * (camUpDownSwitch ? 1.0f : -1.0f),
		sinf(camAutoRotate) * 5.0f,
		0.0f);

	// ジオメトリ用カメラ初期化
	DirectX::XMFLOAT4X4 mat[2];
	DirectX::XMStoreFloat4x4(&mat[0], DirectX::XMMatrixTranspose(
		DirectX::XMMatrixLookAtLH(
			camPos,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		)));
	DirectX::XMStoreFloat4x4(&mat[1], DirectX::XMMatrixTranspose(
		DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(60.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f)
	));
	Geometory::SetView(mat[0]);
	Geometory::SetProjection(mat[1]);
#endif

	scene->Draw();

	scene->PostFrameProcess();
	EndDrawDirectX();


	// imGuiの描画
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

}

void MainGame::PostFrameProcess()
{
	// Sceneのフレーム終了処理
	scene->PostFrameProcess();

	// Sceneが変更されるまでリクエストを確認
	while (!sceneChangeQueue.empty())
	{
		// キーを取得
		std::string key = sceneChangeQueue.front();
		sceneChangeQueue.pop();
		// Sceneを変更出来たらBreak
		if (ChangeScene(key))
		{
			// Queueは初期化する
			std::queue<std::string> empty;
			sceneChangeQueue.swap(empty);
			ResetFPS();
			break;
		}
	}

}


void MainGame::WorkerLoop(
	ThreadSafeQueue<ModelLoadJob>& a_modelJobQueue,
	ThreadSafeQueue<ModelLoadResult>& a_modelResultQueue,
	ThreadSafeQueue<TextureLoadJob>& a_textureJobQueue,
	ThreadSafeQueue<TextureLoadResult>& a_textureResultQueue,
	ThreadSafeQueue<SoundLoadJob>& a_soundJobQueue,
	ThreadSafeQueue<SoundLoadResult>& a_soundResultQueue,
	std::atomic<bool>& a_running)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
#ifdef _DEBUG
		DebugMessage("LoadLoopの初期化に失敗しました！");
#endif // _DEBUG
		
		a_running = false;
		return;
	}
	while (a_running)
	{
		while (!modelJobQueue.IsEnpty())
		{
			// 登録されているJobを受け取る
			ModelLoadJob modelJob = a_modelJobQueue.Pop();

			// Loadして終了フラグを立てる
			ModelLoadResult modelResult = LoadModel(modelJob);
			a_modelResultQueue.Push(std::move(modelResult));
			if (modelJob.endFlagPointer)
			{
				modelJob.endFlagPointer->store(true);
			}

		}

		while (!textureJobQueue.IsEnpty())
		{
			// 上と同じ
			TextureLoadJob textureJob = a_textureJobQueue.Pop();

			TextureLoadResult textureResult = LoadTexture(textureJob);
			a_textureResultQueue.Push(std::move(textureResult));
			if (textureJob.endFlagPointer)
			{
				textureJob.endFlagPointer->store(true);
			}
		}

	}

	CoUninitialize();
}

bool MainGame::ChangeScene(std::string a_key)
{
	bool ret = true;
	if (a_key == "Load")
	{
		scene = make_unique<LoadScene>(
			[this](std::string a_key)
			{
				sceneChangeQueue.push(a_key);
			},
			modelJobQueue,
			textureJobQueue,
			modelDatas,
			textureDatas,
			modelCache,
			uiCache,
			input
		);

		modelDatas.clear();
		textureDatas.clear();
	}
	else if (a_key == "Game")
	{

		scene = make_unique<GameScene>(
			modelCache,
			uiCache,
			[this](std::string a_key)
			{
				sceneChangeQueue.push(a_key);
			},
			[this]()
			{
				isEnd = true;
			},
			[this]()
			{
				ResetFPS();
			},
			input
		);
	}
	else if (a_key == "End")
	{
	}
	else if (a_key == "Test")
	{
		scene = make_unique<TestScene>();
	}
	else
	{
		ret = false;
	}

	return ret;
}

void MainGame::ResetFPS()
{
	DWORD countStartTime = timeGetTime();
	preExecTime = countStartTime;
	fpsOldTime = countStartTime;
	float processingTime = 0.0f;
	int fpsCount = 0;
}

