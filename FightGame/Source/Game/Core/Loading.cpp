#include "Loading.h"
#include "Debug.h"
#include <codecvt>
#include <cstdio>
#include "DirectX.h"
#include <dwrite_3.h>

namespace
{
	// !!!New!!!
	void DebugFontLoadHr(const char* step, HRESULT hr)
	{
		char msg[128];
		sprintf_s(msg, "FontLoad[%s] hr=0x%08X", step, static_cast<unsigned>(hr));
		DebugMessage(msg);
	}

	// !!!New!!!
	void DebugFontLoadPath(const wchar_t* path)
	{
		char pathMsg[512];
		if (WideCharToMultiByte(CP_UTF8, 0, path, -1, pathMsg, sizeof(pathMsg), nullptr, nullptr) <= 0)
		{
			DebugMessage("FontLoad[Path] convert failed");
			return;
		}
	}

	// !!!New!!!
	bool ResolveFontFileFullPath(const std::wstring& fontFilePath, wchar_t* fullPath, DWORD fullPathCount)
	{
		DWORD pathLen = GetFullPathNameW(fontFilePath.c_str(), fullPathCount, fullPath, nullptr);
		if (pathLen == 0 || pathLen >= fullPathCount)
		{
			DebugMessage("FontLoad[GetFullPathNameW] failed");
			return false;
		}
		DebugFontLoadPath(fullPath);
		return true;
	}

	// !!!New!!!
	bool CanOpenFontFile(const wchar_t* fullPath)
	{
		HANDLE fileHandle = CreateFileW(
			fullPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);
		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			DebugFontLoadHr("CreateFileW", HRESULT_FROM_WIN32(GetLastError()));
			return false;
		}
		CloseHandle(fileHandle);
		return true;
	}

	// !!!New!!!
	bool CreateFontFileReferenceWithLog(IDWriteFactory* factory, const wchar_t* fullPath, ComPtr<IDWriteFontFile>& outFontFile)
	{
		HRESULT hr = factory->CreateFontFileReference(
			fullPath,
			nullptr,
			outFontFile.GetAddressOf());
		if (FAILED(hr))
		{
			DebugFontLoadHr("CreateFontFileReference", hr);
			return false;
		}
		return true;
	}

	// !!!New!!!
	bool AddFontFileWithLog(IDWriteFontSetBuilder1* builder1, IDWriteFontFile* fontFile)
	{
		HRESULT hr = builder1->AddFontFile(fontFile);
		if (FAILED(hr))
		{
			DebugFontLoadHr("AddFontFile", hr);
			return false;
		}
		return true;
	}
}

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

TextFormatLoadResult LoadFormat(const TextFormatLoadJob& a_job, const IUICacheAcquisition& a_cache)
{
	IDWriteFactory* factory = GetDWriteFactory();

	HRESULT hr;

	ComPtr<IDWriteFontCollection1> customCollection;
	IDWriteFontCollection* fontCollection = nullptr;

	if(a_job.useFile)
	{
		wchar_t fullPath[MAX_PATH] = {};
		if (!ResolveFontFileFullPath(a_job.fontFile, fullPath, MAX_PATH))
		{
			return TextFormatLoadResult();
		}

		ComPtr<IDWriteFactory5> factory5;
		hr = factory->QueryInterface(IID_PPV_ARGS(&factory5));
		if(FAILED(hr))
		{
			DebugFontLoadHr("QueryInterface(IDWriteFactory5)", hr);
			return TextFormatLoadResult();
		}
		ComPtr<IDWriteFontSetBuilder> builder;
		hr = factory5->CreateFontSetBuilder(&builder);
		if(FAILED(hr))
		{
			DebugFontLoadHr("CreateFontSetBuilder", hr);
			return TextFormatLoadResult();
		}
		ComPtr<IDWriteFontSetBuilder1> builder1;
		hr = builder.As(&builder1);
		if(FAILED(hr))
		{
			DebugFontLoadHr("QueryInterface(IDWriteFontSetBuilder1)", hr);
			return TextFormatLoadResult();
		}

		ComPtr<IDWriteFontFile> fontFile = a_cache.GetTextFontFile(a_job.fontFile);
		if (fontFile == nullptr)
		{
			if (!CanOpenFontFile(fullPath))
			{
				return TextFormatLoadResult();
			}
			if (!CreateFontFileReferenceWithLog(factory, fullPath, fontFile))
			{
				return TextFormatLoadResult();
			}
		}
		else
		{
			DebugMessage("FontLoad[Cache] hit");
		}
		if (!AddFontFileWithLog(builder1.Get(), fontFile.Get()))
		{
			return TextFormatLoadResult();
		}
		ComPtr<IDWriteFontSet> fontSet;
		hr = builder1->CreateFontSet(&fontSet);
		if(FAILED(hr))
		{
			DebugFontLoadHr("CreateFontSet", hr);
			return TextFormatLoadResult();
		}
	
		hr = factory5->CreateFontCollectionFromFontSet(
		fontSet.Get(),
		customCollection.GetAddressOf());
	
		if (FAILED(hr))
		{
			DebugFontLoadHr("CreateFontCollectionFromFontSet", hr);
			return TextFormatLoadResult();
		}

		// フォントが存在しているか確認
		UINT32 index = 0;
		BOOL exists = FALSE;
		customCollection->FindFamilyName(a_job.font.c_str(), &index, &exists);
		if (!exists)
		{
			DebugMessage("FontLoad[FindFamilyName] not found");
			return TextFormatLoadResult();
		}

		fontCollection = customCollection.Get();
	}



	ComPtr<IDWriteTextFormat> textFormat;

	hr = factory->CreateTextFormat(
		a_job.font.c_str(),
		fontCollection,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		a_job.size,
		L"ja-jp",
		textFormat.GetAddressOf()
	);

	if (FAILED(hr))
	{
		DebugFontLoadHr("CreateTextFormat", hr);
		return TextFormatLoadResult();
	}

	hr = textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if (FAILED(hr))
	{
		DebugMessage("テキストフォーマットの読み込みに失敗しました。");
		return TextFormatLoadResult();
	}
	hr = textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	if (FAILED(hr))
	{
		DebugMessage("テキストフォーマットの読み込みに失敗しました。");
		return TextFormatLoadResult();
	}

	return TextFormatLoadResult(
		a_job.key,
		textFormat
	);
}

TextUILoadResult LoadTextUI(const TextUILoadJob& a_job, const IUICacheAcquisition& a_cache)
{
	auto* writeFactory = GetDWriteFactory();
	ComPtr<IDWriteTextFormat> format = a_cache.GetTextFormat(a_job.formatKey).Get();
	HRESULT hr;

	ComPtr<IDWriteTextLayout> textLayout;
	hr = writeFactory->CreateTextLayout(
		a_job.text.c_str(),
		wcslen(a_job.text.c_str()),
		format.Get(),
		a_job.width,   // 最大幅
		a_job.height,   // 最大高さ
		textLayout.GetAddressOf());

	if (FAILED(hr))
	{
		return TextUILoadResult();
	}

	auto* d2dFactory = GetD2DFactory();
	auto* wicFactory = GetWICFactory();
	ComPtr<ID2D1RenderTarget> renderTarget;

	// 書き込む用のBitmapを作成
	ComPtr<IWICBitmap> wicBitmap;
	hr = wicFactory->CreateBitmap(a_job.width, a_job.height, GUID_WICPixelFormat32bppPBGRA,
		WICBitmapCacheOnLoad, wicBitmap.GetAddressOf());

	if (FAILED(hr))
	{
		return TextUILoadResult();
	}

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	hr = d2dFactory->CreateWicBitmapRenderTarget(wicBitmap.Get(), props, renderTarget.GetAddressOf());

	if (FAILED(hr))
	{
		return TextUILoadResult();
	}



	// 実際に与えられた文字を描画する
	ComPtr<ID2D1SolidColorBrush> brush;
	hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brush.GetAddressOf());
	if (FAILED(hr))
	{
		return TextUILoadResult();
	}
	renderTarget->BeginDraw();
	renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)); // 背景を透過
	renderTarget->DrawTextLayout(D2D1::Point2F(0.0f, 0.0f), textLayout.Get(), brush.Get());
	hr = renderTarget->EndDraw();
	if (FAILED(hr))
	{
		return TextUILoadResult();
	}

	// 内容をロックしてデータを生成
	WICRect rect = { 0.0f, 0.0f, (INT)a_job.width, (INT)a_job.height };
	ComPtr<IWICBitmapLock> lock;
	wicBitmap->Lock(&rect, WICBitmapLockRead, lock.GetAddressOf());

	UINT size;
	BYTE* data;
	UINT stride;
	hr = lock->GetDataPointer(&size, &data);
	lock->GetStride(&stride);
	if (FAILED(hr))
	{
		return TextUILoadResult();
	}

	// テクスチャを生成
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	hr = texture->Create(DXGI_FORMAT_B8G8R8A8_UNORM, a_job.width, a_job.height, data);


	lock->Release();

	if (FAILED(hr))
	{
		return TextUILoadResult();
	}

	return TextUILoadResult(
		a_job.key,
		texture
	);
}
