#include "Loading.h"
#include "Debug.h"
#include <codecvt>

constexpr const char* kModelFilePath = "Assets/Model/";
constexpr const char* kTextureFilePath = "Assets/Texture/";
constexpr const char* kEffectFilePath = "Assets/Effect/";

ModelLoadResult LoadModel(const ModelLoadJob& job)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();

	std::string pathCmp = kModelFilePath + job.key;
	std::string path = pathCmp + ".fbx";
	if (model->Load(path.c_str(), job.scale))
	{
		std::cout << job.key << "読み込み完了" << std::endl;;
	}
	else
	{
		DebugMessage(job.key + "のモデルの読み込みに失敗");
	}


	return ModelLoadResult(job.key, model);
}


TextureLoadResult LoadTexture(const TextureLoadJob& job)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	
	std::string pathCmp = kTextureFilePath + job.key;
	std::string path = pathCmp + ".png";
	if (!FAILED(texture->Create(path.c_str())))
	{
		std::cout << job.key << "読み込み完了" << std::endl;
	}
	else
	{
		DebugMessage(job.key + "テクスチャの読み込みに失敗");
	}
	

	return TextureLoadResult(job.key, texture);
}

EffectLoadResult LoadEffect(const EffectLoadJob& job)
{
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
	std::u16string convertedKey = converter.from_bytes(kEffectFilePath + job.key);

	Effekseer::EffectRef ref = Effekseer::Effect::Create(GetEffectManager(), convertedKey.c_str(), job.magnificent);

	return EffectLoadResult(job.key, ref);
}

void LoadSound()
{
}
