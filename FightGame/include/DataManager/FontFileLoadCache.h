#pragma once

#include <unordered_map>
#include <string>
#include <wrl/client.h>
#include <dwrite.h>

using Microsoft::WRL::ComPtr;

class FontFileLoadCache
{
public:
	FontFileLoadCache() = default;
	~FontFileLoadCache() = default;

	// !!!New!!!
	ComPtr<IDWriteFontFile> GetFontFile(const std::wstring& a_key) const;
	void RegisterFontFile(const std::wstring& a_key, ComPtr<IDWriteFontFile> a_fontFile);

private:

	std::unordered_map<std::wstring, ComPtr<IDWriteFontFile>> m_fontFileCache;
};
