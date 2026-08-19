#include "OtherSystem.h"

#include "DebugConsole.h"



void ResetConsoleSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Firework>>();
	
	for (auto it : view)
	{
		// コンポーネントを取得
		ComponentHandle<Firework> firework = a_chunk.GetComponent<Firework>(it);

		DebugConsole::ClearConsole();
		//DebugConsole::SetDrawPos(1, 5);
		//std::string mode = (firework.Look().fireworkType & FIREWORK_BURN) ? "ON " : "OFF";
		//std::cout << "爆発移動" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_DROP) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 6);
		//std::cout << "落下" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_FLY) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 7);
		//std::cout << "ふわふわ" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_SHAKE) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 8);
		//std::cout << "振動" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_RETRO) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 9);
		//std::cout << "レトロゲーム" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_INPUT_MOVE) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 10);
		//std::cout << "キー操作" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_INPUT_ANGLE) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 11);
		//std::cout << "マウス操作" << mode << std::endl;
		//mode = (firework.Look().fireworkType & FIREWORK_FISH) ? "ON " : "OFF";
		//DebugConsole::SetDrawPos(1, 12);
		//std::cout << "魚群" << mode << std::endl;
	}

}

void SetFireTypeSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<Firework>>();

	for (auto it : view)
	{
		// コンポーネントを取得
		ComponentHandle<Firework> firework = a_chunk.GetComponent<Firework>(it);

		bool isResetConsole = false;

		// リセット
		if (a_context.input.IsRegisterKeyTrigger("Default"))
		{
			// 完全にリセットする
			firework->fireworkType = FIREWORK_NONE;
		}
		// 爆発
		if(a_context.input.IsRegisterKeyTrigger("Burn"))
		{
			firework->fireworkType ^= FIREWORK_BURN;
			isResetConsole = true;
		}
		// ふわふわ
		if (a_context.input.IsRegisterKeyTrigger("Fly"))
		{
			firework->fireworkType ^= FIREWORK_FLY;
			isResetConsole = true;
		}
		// 振動
		if (a_context.input.IsRegisterKeyTrigger("Shake"))
		{
			firework->fireworkType ^= FIREWORK_SHAKE;
			firework->fireworkType &= ~FIREWORK_RETRO;
			isResetConsole = true;
		}
		// レトロゲーム
		else if (a_context.input.IsRegisterKeyTrigger("Retro"))
		{
			firework->fireworkType ^= FIREWORK_RETRO;
			firework->fireworkType &= ~FIREWORK_SHAKE;
			isResetConsole = true;
		}
		// キー操作
		if (a_context.input.IsRegisterKeyTrigger("InputMove"))
		{
			firework->fireworkType ^= FIREWORK_INPUT_MOVE;
			isResetConsole = true;
		}
		// マウス操作
		if (a_context.input.IsRegisterKeyTrigger("InputAngle"))
		{
			firework->fireworkType ^= FIREWORK_INPUT_ANGLE;
			isResetConsole = true;
		}
		if (a_context.input.IsRegisterKeyTrigger("Drop"))
		{
			firework->fireworkType ^= FIREWORK_DROP;
			isResetConsole = true;
		}
		if (a_context.input.IsRegisterKeyTrigger("Fish"))
		{
			firework->fireworkType ^= FIREWORK_FISH;
			isResetConsole = true;
		}

		if (isResetConsole)
		{
			DebugConsole::ClearConsole();
			DebugConsole::SetDrawPos(1, 5);
			std::string mode = (firework.Look().fireworkType & FIREWORK_BURN) ? "ON " : "OFF";
			std::cout << "爆発移動" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_DROP) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 6);
			std::cout << "落下" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_FLY) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 7);
			std::cout << "ふわふわ" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_SHAKE) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 8);
			std::cout << "振動" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_RETRO) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 9);
			std::cout << "レトロゲーム" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_INPUT_MOVE) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 10);
			std::cout << "キー操作" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_INPUT_ANGLE) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 11);
			std::cout << "マウス操作" << mode << std::endl;
			mode = (firework.Look().fireworkType & FIREWORK_FISH) ? "ON " : "OFF";
			DebugConsole::SetDrawPos(1, 12);
			std::cout << "魚群" << mode << std::endl;

		}
	}
}

void SpawnObjectSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// 撃つボタンが押されていなかったら撃たない

	bool isShotKey = a_context.input.IsRegisterTrigger("Shot");

	ComponentView view = a_chunk.GetView<ComponentTypes<ShooterComponent, Position, Rotation, RayInfomation>>();

	// 全てのShootterを捜査
	for (auto it : view)
	{

		ComponentHandle<ShooterComponent> shooter = a_chunk.GetComponent<ShooterComponent>(it);
		bool isShot = false;

		if (!shooter.Look().canShot)
		{
			shooter->shotTimer -= a_context.deltaTime;
			if (shooter.Look().shotTimer > 0.0f) continue;
			shooter->canShot = true;

			continue;
		}

		if (!isShotKey) continue;

		PlaySound(LoadSound("Assets/Sound/shot.mp3"));
		isShot = true;

		// 射撃処理
		shooter->canShot = false;

		// 撃ったかどうかの判定
		ComponentView aview = a_chunk.GetView<ComponentTypes<PlayerTag, RayInfomation>>();
	
		std::vector<std::string> colors = {
			"Blue",
			"Red",
			"Purple",
			"Green"
		};

		for (auto it : aview)
		{
			// Rayの当たっている位置にオブジェクトを出す
			const ComponentHandle<RayInfomation> info = a_chunk.GetComponent<RayInfomation>(it);
			const ComponentHandle<Firework> firework = a_chunk.GetComponent<Firework>(it);
			int rand = std::rand();

			int colorIndex = rand % colors.size();
			float fish = ((rand % 1) - 0.5f) / 5580.0f;
			
			for (int i = 0; i < 80; i++)
			{
				float3 rot(
					(float)(std::rand() % 180 - 90),
					(float)(std::rand() % 360),
					0.0f
				);

				

				Entity fire = a_chunk.CreateNewEntity(
					MOVE_AND_TRANSFORM_COMPONENT(
						info.Look().rayHitPosition,
						rot,
						float3(0.012f, 0.012f, 0.125f)
					),
					ModelKey("Box"),
					Velocity(0.0f, 0.0f, 0.0f),
					LifeTime(300.0f),
					AlphaBlendComponent(colors[colorIndex], "Alpha"),
					BoxCollider(float3(), float3(0.012f, 0.012, 0.012)),
					OBBCollider(OBB_PushOutLocked)
				);
				// 爆発移動
				if (firework.Look().fireworkType & FIREWORK_BURN)
				{
					a_chunk.AddComponent(fire, MoveForward(0.0012f, 1.035f));
				}
				// 前方移動
				else
				{
					a_chunk.AddComponent(fire, MoveForward(0.018f, 0.993f));
				}
				// 落下
				if (firework.Look().fireworkType & FIREWORK_DROP)
				{
					a_chunk.AddComponent(fire, Force(float3(0.0f, -0.000025f, 0.0f), 1.0f));
				}
				// ふわふわ
				if(firework.Look().fireworkType & FIREWORK_FLY)
				{
					a_chunk.AddComponent(fire, RailFly(0.2f, 0.125f));
				}
				// 振動
				if(firework.Look().fireworkType & FIREWORK_SHAKE)
				{
					a_chunk.AddComponent(fire, ShakeComponent(float3(0.004f, 0.004f, 0.004f), float3(0.7f, 0.7f, 0.7f), 300.0f));
				}
				// レトロゲーム
				else if (firework.Look().fireworkType & FIREWORK_RETRO)
				{
					a_chunk.AddComponent(fire, ShakeComponent(float3(0.025f, 0.025f, 0.025f), float3(0.05f, 0.05f, 0.05f), 300.0f));
				}
				// キーボード操作
				if(firework.Look().fireworkType & FIREWORK_INPUT_MOVE)
				{
					a_chunk.AddComponent(fire, InputMove(float2(0.1f, 0.1f)));
				}
				// マウス操作
				if (firework.Look().fireworkType & FIREWORK_INPUT_ANGLE)
				{
					a_chunk.AddComponent(fire, InputRotato(float2(10.0f, 0.0f), float2(100.0f, 100.0f)));
				}
				// 魚群
				if (firework.Look().fireworkType & FIREWORK_FISH)
				{
					a_chunk.AddComponent(fire, AngularVelocity(float3(2.0f + fish , 1.5f + fish * 0.3f, 0.0f)));
				}

			}

		}
		shooter->shotTimer = shooter.Look().maxShotTimer;

	}
}

void TheWorldSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse)
{
	bool isStandKey = a_context.input.IsRegisterPress("TheWorld");

	if (!isStandKey) return;
	a_systemResponse.AddStopTime(0.5f, 0.5f, 0.1f);
	
}