#include "EffectSystem.h"
#include "Components.h"
#include "Sound.h"
#include "Defines.h"
#include "GameData.h"
#include "SystemAssist.h"
#include "MathAssist.h"

using namespace Component;

void FadeUISystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<FadeUI>>();

	for (auto it : view)
	{

		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		ComponentHandle<FadeUI> fade = a_chunk.GetComponent<FadeUI>(it);

		float speed = fade.Look().fadeSpeed;

		fade->progress = std::clamp(fade.Look().progress + speed * a_context.effectStepTime, 0.0f, 1.0f);

		float target = 1.0f;
		float start = 0.0f;

		if (fade.Look().fadeType == FadeType::FADE_UP)
		{
			start = fade.Look().min;
			target = fade.Look().max;
		}
		else if(fade.Look().fadeType == FadeType::FADE_DOWN)
		{
			start = fade.Look().max;
			target = fade.Look().min;
		}

		ui->alpha = Lerp(start, target, fade.Look().progress);

		ComponentHandle<FadeChange> change = a_chunk.GetComponent<FadeChange>(it);
		if (!change.IsValid()) continue;

		if (change.Look().isWait) continue;

		if (fade.Look().progress < 1.0f) continue;
		fade->progress = 0.0f;
		switch (change.Look().type)
		{
		case FadeChungeType::FADE_CHANGE_FLICKER:
			if (fade->fadeType == FADE_DOWN) fade->fadeType = FADE_UP;
			else if (fade->fadeType == FADE_UP)	fade->fadeType = FADE_DOWN;
			break;
			
		}
	}
}

void CreateEffectSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView keyView = a_chunk.GetView<ComponentTypes<EffectKey>>();
	for (auto it : keyView)
	{
		const ComponentHandle<EffectKey> effectKey = a_chunk.GetComponent<EffectKey>(it);
		if (!a_context.input.IsRegisterTrigger(effectKey.Look().key)) continue;

		Entity createEffect = a_chunk.CreateNewEntity(
			CreateEffect(effectKey.Look().type)
		);

		if (!effectKey.Look().isRecycling)
		{
			a_chunk.DeleteChunkEntity(it);
		}
	}

	ComponentView createView = a_chunk.GetView<ComponentTypes<CreateEffect>>();

	for (auto it : createView)
	{
		ComponentHandle<CreateEffect> effect = a_chunk.GetComponent<CreateEffect>(it);

		float2 posOffset = effect.Look().posOffset;
		float angleOffset = effect.Look().angleOffset;

		if (effect.Look().duration < effect.Look().maxWaitTime)
		{
			effect->duration += a_context.deltaTime;
			continue;
		}

		switch (effect.Look().type)
		{
		case WHITEFADE_UP:
		{
			Entity whiteFade = a_chunk.CreateNewEntity(
				UIComponent("WhiteFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 0.0f),
				FadeUI(FADE_UP, 0.01f)
			);

			Entity whiteBack = a_chunk.CreateNewEntity(
				UIComponent("WhiteFadeBack", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 0.0f),
				FadeUI(FADE_UP, 0.006f)
			);


			break;
		}

		case WHITEFADE_CLEAR:
			Entity whiteFadeClear = a_chunk.CreateNewEntity(
				UIComponent("WhiteFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.015f)
			);

			Entity whiteBackClear = a_chunk.CreateNewEntity(
				UIComponent("WhiteFadeBack", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.009f)
			);

			break;

		case WHITEMINIFADE_UP:
		{
			Entity whiteMiniFade = a_chunk.CreateNewEntity(
				UIComponent("WhiteFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 0.0f),
				FadeUI(FADE_UP, 0.02f)
			);

			Entity whiteMiniBack = a_chunk.CreateNewEntity(
				UIComponent("WhiteFadeBack", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 0.0f),
				FadeUI(FADE_UP, 0.015f)
			);

			break;
		}

		case WHITEMINIFADE_CLEAR:
			Entity whiteMiniFadeClear = a_chunk.CreateNewEntity(
				UIComponent("WhiteFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.03f)
			);

			Entity whiteMiniBackClear = a_chunk.CreateNewEntity(
				UIComponent("WhiteFadeBack", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.04f)
			);

			break;


		case DARKFADE_UP:
			Entity darkFadeMini = a_chunk.CreateNewEntity(
				UIComponent("DarkFadeMini", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 0.0f),
				FadeUI(FADE_UP, 0.005f)
			);
			Entity darkFade = a_chunk.CreateNewEntity(
				UIComponent("DarkFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 0.0f),
				FadeUI(FADE_UP, 0.004f)
			);
			break;

		case DARKFADE_CLEAR:
			Entity darkFadeMiniClear = a_chunk.CreateNewEntity(
				UIComponent("DarkFadeMini", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.02f)
			);
			Entity darkFadeClear = a_chunk.CreateNewEntity(
				UIComponent("DarkFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.006f)
			);
			break;

		case SPEEDLINEFADE:
			Entity speedLine = a_chunk.CreateNewEntity(
				UIComponent("Speedline", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.05f)
			);
			break;

		case SHOTFLASH:
			Entity shotFlash = a_chunk.CreateNewEntity(
				UIComponent("WhiteFade", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.08f)
			);
			break;

		case DAMAGE_EFFECT:
			Entity damageEffect = a_chunk.CreateNewEntity(
				UIComponent("DamageEffect", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.04f)
			);

			Entity damageSpeedLine = a_chunk.CreateNewEntity(
				UIComponent("Speedline", float2(0.0f, 0.0f) + posOffset, float2(2.7f, 2.7f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.1f)
			);
			
			break;

		case DAMAGE_DIRECTION:
			Entity damageDirEffect = a_chunk.CreateNewEntity(
				UIComponent("DamageDirectionEffect", float2(0.0f, 0.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				FadeUI(FADE_DOWN, 0.04f)
			);
			break;

		case BATTLESTART:
			Entity battleStart = a_chunk.CreateNewEntity(
				UIComponent("BattleStart", float2(0.0f, 4.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				UIPosLerp(float2(0.0f, 2.0f) + posOffset, posOffset, 18.0f),
				UIScaleLerp(float2(0.6f * 5.8f, 0.48f * 5.8f), float2(0.6f, 0.48f), 18.0f),
				LifeTime(130.0f),
				DropUI({ "Assets/Sound/startui.mp3", "Assets/Sound/startui2.mp3" })

			);

			break;

		case GAMECLEAR:
			/*Entity gameClear = a_chunk.CreateNewEntity(
				UIComponent("GameEnd", float2(0.0f, 4.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				UIPosLerp(float2(0.0f, 2.0f) + posOffset, posOffset, 40.0f),
				UIScaleLerp(float2(0.6f * 5.8f, 0.48f * 5.8f), float2(0.6, 0.48f), 40.0f),
				LifeTime(130.0f),
				DropUI({ "Assets/Sound/gameclear.mp3", "Assets/Sound/shot.mp3" })
			);*/


			Entity gameClearBack = a_chunk.CreateNewEntity(
				UIComponent("ClearBack", float2(0.0f, 0.0f) + posOffset, float2(4.0f, 4.0f * (SCREEN_WIDTH / SCREEN_HEIGHT)), -10.0f + angleOffset, 0.3f, float2(), float2(7.0f, 7.0f)),
				UVMove(float2(0.02f, 0.0f)),
				LifeTime(100.0f)
			);


			Entity gameClearFream = a_chunk.CreateNewEntity(
				UIComponent("GameEndFrame", float2(0.0f, 0.2f) + posOffset, float2(1.5f, 1.5f), 0.0f + angleOffset, 1.0f),
				UIPosLerp(float2(0.0f, 2.0f) + posOffset, posOffset + 0.01f, 25.0f),
				UIScaleLerp(float2(2.1f * 5.8f, 1.5f * 5.8f), float2(2.1f, 1.5f), 25.0f),
				LifeTime(100.0f)
			);

			Entity gameClear = a_chunk.CreateNewEntity(
				UIComponent("GameEnd", float2(0.0f, 4.0f) + posOffset, float2(2.0f, 2.0f), 0.0f + angleOffset, 1.0f),
				UIPosLerp(float2(0.0f, 2.0f) + posOffset, posOffset, 25.0f),
				UIScaleLerp(float2(1.0f * 5.8f, 0.66f * 5.8f), float2(1.0, 0.66f), 25.0f),
				LifeTime(100.0f)
			);

			Entity clearSE = a_chunk.CreateNewEntity(
				SoundKey(false, 0.0f, "stageclear")
			);

			Entity clearSE2 = a_chunk.CreateNewEntity(
				SoundKey(false, 0.0f, "stageclear2")
			);

			break;

		case STAGE_CLEAR_MOVIE:
			Entity stageClear = a_chunk.CreateNewEntity(
				Name("StageClear"),
				MOVE_AND_TRANSFORM_COMPONENT(
					float3(0.0f, 0.0f, 0.0f),
					float3(-1.0f, 0.0f, 0.0f),
					float3(1.0f, 1.0f, 1.0f)
				),
				UIComponent("StageClear",float2(), float2(3.35f, 1.0f), 0.0f),
				SpriteComponent(float3(), float3(0.0f, 30.0f, 0.0f), true),
				LeapPosComponent(true, float3(0.0f, 1.6f, 20.0f) + kClearWorldPosition, float3(0.0f, 1.7f, 2.0f) + kClearWorldPosition, 45.0f),
				PosePosState(POSE_POS_LEAP),
				RailFly(0.01f, 0.01f),
				AngularVelocity(float3(0.1f, 0.0f, 0.0f))
			);
			break;

		}

		a_chunk.DeleteChunkEntity(it);
	}

}

void TrailSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	a_context.trailEffect->Update();

	ComponentView view = a_chunk.GetView<ComponentTypes<TrailComponent, Position>>();

	for (auto it : view)
	{
		float3 pos = reinterpret_cast<const float3&>(a_chunk.GetComponent<Position>(it).Look());
		ComponentHandle<TrailComponent> trailHandle = a_chunk.GetComponent<TrailComponent>(it);

		TrailUpdateData data(pos, trailHandle.Look().size, trailHandle.Look().shrinkageRate);

		a_context.trailEffect->AddUpdateLine(trailHandle.Look().id, data);
	}
}

void UIMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent, UIAngularSpeed>>();
	for (auto it : view)
	{
		const ComponentHandle<UIAngularSpeed> angleSpeed = a_chunk.GetComponent<UIAngularSpeed>(it);
		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);

		ui->uiRotation += angleSpeed.Look().speed * a_context.effectStepTime;
	}
}

void UVMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent, UVMove>>();
	for (auto it : view)
	{
		const ComponentHandle<UVMove> uvMove = a_chunk.GetComponent<UVMove>(it);
		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);

		ui->uvPos += uvMove.Look().moveSpeed * a_context.effectStepTime;
	}
}

void HPGaugeSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent, HPGaugeUI>>();
	for (auto it : view)
	{

		ComponentHandle<HPGaugeUI> gauge = a_chunk.GetComponent<HPGaugeUI>(it);
		Entity targetEntity = gauge.Look().target;
		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);

		// 対象のHPゲージがなかった場合ゲージ0にする
		ComponentHandle<HitPoint> targetHP = a_chunk.GetComponent<HitPoint>(targetEntity);
		if (!targetHP.IsValid())
		{
			ui->uiScale.x = 0.0f;
			continue;
		}
		
		// HP割合
		float hpRatio = targetHP.Look().currentHP / targetHP.Look().maxHP;
		ui->uvScale.x = hpRatio; 
		ui->uiScale.x = gauge.Look().gaugeScale.x * hpRatio;
		ui->uiPos.x = gauge.Look().gaugePos.x - gauge.Look().gaugeScale.x * (1 - hpRatio) / 2;
	}
}


void SpriteAnimationSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<SpriteAnimation>>();

	for (auto it : view)
	{
		ComponentHandle<SpriteAnimation> spriteAnim = a_chunk.GetComponent<SpriteAnimation>(it);

		int currentSprite = spriteAnim.Look().nextSprite;
		if (currentSprite == spriteAnim.Look().maxSprite)
		{
			if (!spriteAnim.Look().isLoop) continue;

			currentSprite = 0;
			spriteAnim->nextSprite = 0;
		}
		// 次使用予定のスプライトアニメ番号を更新する
		spriteAnim->nextSprite += 1;

		// 使用するアニメのUVを取得
		int2 spriteUVCount = spriteAnim.Look().spriteUVCount;
		float2 uvScale(
			1.0f / (float)spriteUVCount.x,
			1.0f / (float)spriteUVCount.y
		);
		int2 uvCount(
			currentSprite % spriteUVCount.x,
			currentSprite / spriteUVCount.y
		);
		float2 uvPos(
			uvScale.x * (float)uvCount.x,
			uvScale.y * (float)uvCount.y
		);

		// UVを更新
		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		if (ui.IsValid())
		{
			ui->uvPos = uvPos;
			ui->uvScale = uvScale;
		}
	}
}

void SpawnEfkEffectAreaSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EfkEffectArea, Position>>();
	for (auto it : view)
	{
		// コンポーネントを取得
		ComponentHandle<EfkEffectArea> area = a_chunk.GetComponent<EfkEffectArea>(it);

		// スポーン間隔を更新
		area->currentDuration += a_context.deltaTime;

		// スポーン間隔中だった場合は抜ける
		if (area.Look().currentDuration < area.Look().spawnInteval)
		{
			continue;
		}

		// 自身座標から見たの範囲内にランダムにエフェクトを生成
		if (area.Look().spawnEffectKeys.empty())
		{
			continue;
		}

		ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		const float u1 = (float)(std::rand() % 10001) / 10000.0f;
		const float u2 = (float)(std::rand() % 10001) / 10000.0f;
		const float u3 = (float)(std::rand() % 10001) / 10000.0f;
		const float r = area.Look().spawnRadius * powf(u1, 1.0f / 3.0f);
		const float theta = 2.0f * PI * u2;
		const float phi = acosf(2.0f * u3 - 1.0f);
		const float offsetX = r * sinf(phi) * cosf(theta);
		const float offsetY = r * cosf(phi);
		const float offsetZ = r * sinf(phi) * sinf(theta);
		const int effectKeyIndex = std::rand() % static_cast<int>(area.Look().spawnEffectKeys.size());
		
		const float3 spawnPos(
			pos.Look().x + offsetX,
			pos.Look().y + offsetY,
			pos.Look().z + offsetZ
		);
		a_chunk.CreateNewEntity(
			TRANSFORM_COMPONENT(spawnPos, float3(), float3(1.0f, 1.0f, 1.0f)),
			EfkEffectKey(area.Look().spawnEffectKeys[effectKeyIndex], false)
		);

		if (!area.Look().spawnSoundKeys.empty())
		{
			const int soundKeyIndex = std::rand() % static_cast<int>(area.Look().spawnSoundKeys.size());
			std::string key = kSoundAssetPath + area.Look().spawnSoundKeys.at(soundKeyIndex) + ".mp3";
			PlaySound(LoadSound(key.c_str()));
		}

		area->currentDuration = 0.0f;
		area->spawnInteval /= area.Look().spawnSpeedupRate;
	}
}

void SpawnEfkEffectSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EfkEffectKey, Position>, ComponentTypes<EfkEffectRuntime>>();

	for (auto it : view)
	{
		ComponentHandle<EfkEffectKey> key = a_chunk.GetComponent<EfkEffectKey>(it);
		if (key.Look().waitTime > 0.0f)
		{
			key->waitTime -= a_context.deltaTime;
			key->waitTime = std::max(key.Look().waitTime, 0.0f);
			continue;
		}

		ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		Effekseer::EffectRef ref;

		if (!a_context.effectCache.TryGetEffect(key.Look().handleId, ref)) continue;

		if(!key.Look().isLoop) a_chunk.DeleteChunkComponent(it, EfkEffectKey::kTypeId);
		a_chunk.AddComponent(it, EfkEffectRuntime(GetEffectManager()->Play(ref, pos.Look().x, pos.Look().y, pos.Look().z), key.Look().isLoop));
	}
}

void UpdateEfkEffectSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<EfkEffectRuntime, Position>>();
	
	for (auto it : view)
	{
		ComponentHandle<EfkEffectRuntime> runtime = a_chunk.GetComponent<EfkEffectRuntime>(it);
		const ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);

		if (!GetEffectManager()->Exists(runtime.Look().handle))
		{
			if (runtime.Look().isLoop) a_chunk.DeleteChunkComponent(it, EfkEffectRuntime::kTypeId);
			else a_chunk.DeleteChunkEntity(it); 
			continue;
		}


		GetEffectManager()->SetLocation(runtime.Look().handle, pos.Look().x, pos.Look().y, pos.Look().z);
		GetEffectManager()->SetRotation(runtime.Look().handle, rot.Look().pitch * RAD, rot.Look().yaw * RAD, rot.Look().roll * RAD);
		GetEffectManager()->UpdateHandle(runtime.Look().handle, a_context.deltaTime / 4);
	}
	static int time = 0;
	time++;

	Effekseer::Manager::UpdateParameter updateParameter;
	GetEffectManager()->Update(updateParameter);
}

void UILerpSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView posView = a_chunk.GetView<ComponentTypes<UIComponent, UIPosLerp>>();
	for (auto it : posView)
	{
		// 進行終了していたら抜ける
		ComponentHandle<UIPosLerp> posLerp = a_chunk.GetComponent<UIPosLerp>(it);
		if (posLerp.Look().progress >= 1.0f)
		{
			continue;
		}
		
		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		
		// 進行度を更新
		float newProgress = posLerp.Look().progress + a_context.effectStepTime / posLerp.Look().maxLerpTime;
		posLerp->progress = std::clamp(newProgress, 0.0f, 1.0f);
		// ui座標を更新
		ui->uiPos.x = Lerp(posLerp.Look().startPos.x, posLerp.Look().targetPos.x, posLerp.Look().progress);
		ui->uiPos.y = Lerp(posLerp.Look().startPos.y, posLerp.Look().targetPos.y, posLerp.Look().progress);
	}

	ComponentView scaleView = a_chunk.GetView<ComponentTypes<UIComponent, UIScaleLerp>>();
	for (auto it : scaleView)
	{
		// 進行終了していたら抜ける
		ComponentHandle<UIScaleLerp> scaleLerp = a_chunk.GetComponent<UIScaleLerp>(it);
		if (scaleLerp.Look().progress >= 1.0f) continue;

		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);

		// 進行度を更新
		float newProgress = scaleLerp.Look().progress + a_context.effectStepTime / scaleLerp.Look().maxLerpTime;
		scaleLerp->progress = std::clamp(newProgress, 0.0f, 1.0f);
		// ui座標を更新
		ui->uiScale.x = Lerp(scaleLerp.Look().startScale.x, scaleLerp.Look().targetScale.x, scaleLerp.Look().progress);
		ui->uiScale.y = Lerp(scaleLerp.Look().startScale.y, scaleLerp.Look().targetScale.y, scaleLerp.Look().progress);
	}
}

void StartUISystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<DropUI>>();

	for (auto it : view)
	{
		ComponentHandle<UIPosLerp> posLerp = a_chunk.GetComponent<UIPosLerp>(it);
		if (!posLerp.IsValid() || posLerp.Look().progress < 1.0f) continue;
		ComponentHandle<UIScaleLerp> scaleLerp = a_chunk.GetComponent<UIScaleLerp>(it);
		if (!scaleLerp.IsValid() || scaleLerp.Look().progress < 1.0f) continue;
		
		a_chunk.DeleteChunkComponent(it, UIPosLerp::kTypeId);
		a_chunk.DeleteChunkComponent(it, UIScaleLerp::kTypeId);

		ComponentHandle<DropUI> drop = a_chunk.GetComponent<DropUI>(it);
		a_chunk.AddComponent(it, Component::FadeUI(FADE_DOWN, 0.02f));
		for (auto soundKey : drop.Look().playSounds)
		{
			PlaySound(LoadSound(soundKey.c_str()));
		}

		
	}

}


void SoundSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<SoundKey>>();
	for (auto it : view)
	{
		ComponentHandle<SoundKey> sound = a_chunk.GetComponent<SoundKey>(it);
		std::string key = kSoundAssetPath + sound.Look().soundKey + ".mp3";

		if (!sound.IsValid()) continue;
		if(sound.Look().waitTime > 0.0f)
		{
			sound->waitTime -= a_context.deltaTime;
			continue;
		}

		PlaySound(LoadSound(key.c_str(), sound.Look().isLoop));
		a_chunk.DeleteChunkEntity(it);
	}
}

// !!!New!!!
void UITextBoxCursorSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<UITextBoxCursor, DeleteOnInput>>();

	for (auto it : view)
	{
		const ComponentHandle<DeleteOnInput> deleteOnInput = a_chunk.GetComponent<DeleteOnInput>(it);

		if (deleteOnInput.Look().elapsedTime + a_context.deltaTime < deleteOnInput.Look().maxWaitTime)
		{
			continue;
		}

		const ComponentHandle<UITextBoxCursor> cursor = a_chunk.GetComponent<UITextBoxCursor>(it);

		a_chunk.AddComponent(it, UIComponent(
			cursor.Look().uiKey,
			cursor.Look().uiPos,
			cursor.Look().uiScale,
			0.0f
		));
		a_chunk.AddComponent(it, FadeUI(
			FADE_DOWN,
			cursor.Look().fadeSpeed,
			cursor.Look().fadeMin,
			cursor.Look().fadeMax
		));
		a_chunk.AddComponent(it, FadeChange(FADE_CHANGE_FLICKER));

		if (!cursor.Look().soundKey.empty())
		{
			std::string soundPath = kSoundAssetPath + cursor.Look().soundKey + ".mp3";
			PlaySound(LoadSound(soundPath.c_str()));
		}

		a_chunk.DeleteChunkComponent(it, UITextBoxCursor::kTypeId);
	}
}

void CreateTutorialTextSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView createView = a_chunk.GetView<ComponentTypes<CreateTutorialWindow>>();

	for (auto it : createView)
	{
		ComponentHandle<CreateTutorialWindow> tutorial = a_chunk.GetComponent<CreateTutorialWindow>(it);
		// 開始まで待機
		if (tutorial.Look().waitTime > 0.0f)
		{
			tutorial->waitTime -= a_context.deltaTime;
			continue;
		}

		// UIを生成
		a_chunk.CreateNewEntity(
			UIComponent("TutorialWindow", float2(0.0f, -0.6f), float2(1.95f, 0.8f), 0.0f),
			DeleteOnInput("Select", 120.0f),
			ClearTarget(),
			AllActionStopper(),
			FadeUI(FADE_DOWN, 0.02f, 0.85f, 1.0f),
			FadeChange(FADE_CHANGE_FLICKER)

		);

		a_chunk.DeleteChunkEntity(it);

		// テキストを生成
		a_chunk.CreateNewEntity(
			UIComponent(tutorial.Look().textKey, float2(0.0f, -0.6f), float2(1.8f, 0.7f), 0.0f),
			DeleteOnInput("Select", 120.0f),
			ClearTarget(),
			AllActionStopper(),
			FadeUI(FADE_DOWN, 0.02f, 0.95f, 1.0f),
			FadeChange(FADE_CHANGE_FLICKER)
		);

		a_chunk.CreateNewEntity(
			UITextBoxCursor("LookOnMaker", float2(0.85f, -0.8f), float2(0.07f, 0.07f * 1.777f), 0.02f, 0.0f, 1.0f, ""),
			DeleteOnInput("Select", 120.0f),
			ClearTarget(),
			AllActionStopper()
		);

		
	}

}

bool IsTargetInInputDirection(int a_direction, float a_currentPos, float a_targetPos)
{
	if (a_direction == 0) return true;

	int vectorSign = Sign(a_targetPos - a_currentPos);
	if (vectorSign != a_direction) return false;
	return true;
}

// 毎回View作るとちょっと重いだろうから引数で受け取る
Entity GetNextTarget(Entity a_target, Chunk& a_chunk, ComponentView a_cursorView ,float2 a_direction)
{

	Entity bestEntity = a_target;
	ComponentHandle<SelectBox> bestBox = a_chunk.GetComponent<SelectBox>(a_target);
	if (std::abs(a_direction.x) < 0.3f)
	{
		a_direction.x = 0.0f;
	}
	if (std::abs(a_direction.y) < 0.3f)
	{
		a_direction.y = 0.0f;
	}

	int2 signDirection(Sign(a_direction.x), Sign(a_direction.y));

	for (auto it : a_cursorView)
	{
		if (it == a_target) continue;

		ComponentHandle<SelectBox> targetBox = a_chunk.GetComponent<SelectBox>(it);

		if (!IsTargetInInputDirection(signDirection.x, bestBox.Look().pos.x, targetBox.Look().pos.x))
		{
			continue;
		}

		if (!IsTargetInInputDirection(signDirection.y, bestBox.Look().pos.y, targetBox.Look().pos.y))
		{
			continue;
		}
		

		// 代入
		bestEntity = it;
		bestBox = targetBox;
	}
	return bestEntity;
}

void PlayCursor(Entity a_target, Chunk& a_chunk, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	// 選択先の選択時機能を取得する
	ComponentHandle<SelectPlayCommand> command = a_chunk.GetComponent<SelectPlayCommand>(a_target);
	// チャンクを変更する
	if (command.Look().flag & SELECT_CHANGESCENE)
	{
		a_chunk.CreateNewEntity(
			ChunkChange(true, command.Look().sceneName),
			DelayChunkChange(command.Look().waitChangeScene)
		);
	}

	// エフェクトを再生する
	if (command.Look().flag & SELECT_CREATEEFFECT)
	{
		a_chunk.CreateNewEntity(
			CreateEffect(command.Look().effectType)
		);
	}


	// 音を鳴らす
	if(command.Look().flag & SELECT_PLAYSOUND)
	{
		a_chunk.CreateNewEntity(
			SoundKey(false, 0.0f, command.Look().soundName)
		);
	}

	// 全てのカーソルや選択ボックスを削除
	if (command.Look().flag & SELECT_ALLSELECT_DELETE)
	{
		ComponentView cursorView = a_chunk.GetView<ComponentTypes<SelectCursor>>();
		for (auto it : cursorView)
		{
			a_chunk.DeleteChunkEntity(it);
		}

		ComponentView boxView = a_chunk.GetView<ComponentTypes<SelectBox>>();
		for (auto it : boxView)
		{
			a_chunk.DeleteChunkEntity(it);
		}
	}

	// Jsonを読み込む
	if (command.Look().flag & SELECT_LOAD)
	{
		NewSceneSpawn(a_serialize, a_chunk, a_aiManager, command.Look().loadName, float3());
	}

	// 回復を生成する
	if (command.Look().flag & SELECT_HPHEAL)
	{

	}
	
}

void CursorSelectSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	ComponentView cursorView = a_chunk.GetView<ComponentTypes<SelectCursor>>();
	ComponentView targetView = a_chunk.GetView<ComponentTypes<SelectBox>>();


	bool isUseleft = a_context.input.GetLeftAxis().magnitube >= a_context.input.GetKeyAxis().magnitube;
	const Axis& leftAxis = isUseleft
		? a_context.input.GetLeftAxis()
		: a_context.input.GetKeyAxis();

	if (isUseleft)
	{
		DebugConsole::SetDrawPos(15, 22);
		std::cout << "left!!" << std::endl;
	}

	bool isPlay = a_context.input.IsRegisterTrigger("Select");
	float2 direction = float2(leftAxis.x, leftAxis.y);

	for (auto cursorIt : cursorView)
	{
		ComponentHandle<SelectCursor> cursor = a_chunk.GetComponent<SelectCursor>(cursorIt);
		if (!cursor.Look().isActiv) continue;
		
		// カーソル選択
		if (cursor.Look().coolTime > 0.0f)
		{
			float newCoolTime = cursor.Look().coolTime - a_context.deltaTime;
			newCoolTime = std::max(newCoolTime, 0.0f);
			cursor->coolTime = newCoolTime;
		}
		else if (leftAxis.magnitube != 0.0f)
		{
			// ターゲットを更新
			Entity nextTarget = GetNextTarget(cursor.Look().selectEntity, a_chunk, targetView, direction);
			if (nextTarget != cursor->selectEntity)
			{
				cursor->coolTime = cursor.Look().maxCoolTime;
				cursor->selectEntity = nextTarget;
			}

			ComponentHandle<SelectBox> box = a_chunk.GetComponent<SelectBox>(nextTarget);
			ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(cursorIt);
			ui->uiPos = box.Look().pos;
			ui->uiScale = box.Look().cursorScale;
		}

		if (isPlay) PlayCursor(cursor.Look().selectEntity, a_chunk, a_aiManager, a_serialize);
	}


}
