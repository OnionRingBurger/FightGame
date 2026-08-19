#include "EffectSystem.h"
#include "Components.h"
#include "Sound.h"

using namespace Component;

void FadeUISystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<FadeUI>>();

	for (auto it : view)
	{

		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		ComponentHandle<FadeUI> fade = a_chunk.GetComponent<FadeUI>(it);

		float speed = fade.Look().fadeSpeed;

		if (fade.Look().fadeType == FADE_DOWN)
		{
			speed *= -1.0f;
		}

		if(ui.IsValid()) ui->alpha = std::clamp(ui.Look().alpha + speed * a_context.effectStepTime, 0.0f, 1.0f);

		ComponentHandle<FadeChange> change = a_chunk.GetComponent<FadeChange>(it);
		if (!change.IsValid()) continue;

		if (change.Look().isWait) continue;

		switch (change.Look().type)
		{
		case FadeChungeType::FADE_CHANGE_FLICKER:
			if (ui.Look().alpha <= 0.0f)
			{
				fade->fadeType = FADE_UP;
			}
			else if (ui.Look().alpha >= 1.0f)
			{
				fade->fadeType = FADE_DOWN;
			}
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
		const ComponentHandle<CreateEffect> effect = a_chunk.GetComponent<CreateEffect>(it);

		float2 posOffset = effect.Look().posOffset;
		float angleOffset = effect.Look().angleOffset;

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

			PlaySound(LoadSound("Assets/Sound/goal.mp3"));

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

			PlaySound(LoadSound("Assets/Sound/minifade.mp3"));
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
		}
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

		ui->uiRotation += angleSpeed.Look().speed * a_context.deltaTime;
	}
}

void UVMoveSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent, UVMove>>();
	for (auto it : view)
	{
		const ComponentHandle<UVMove> uvMove = a_chunk.GetComponent<UVMove>(it);
		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);

		ui->uvPos += uvMove.Look().moveSpeed * a_context.deltaTime;
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


