#include "FontFileLoadCache.h"


// !!!New!!!
ComPtr<IDWriteFontFile> FontFileLoadCache::GetFontFile(const std::wstring& a_key) const
{
	if (m_fontFileCache.find(a_key) == m_fontFileCache.end())
	{
		return nullptr;
	}

	return m_fontFileCache.at(a_key);
}

// !!!New!!!
void FontFileLoadCache::RegisterFontFile(const std::wstring& a_key, ComPtr<IDWriteFontFile> a_fontFile)
{
	if (!a_fontFile) return;

	m_fontFileCache.insert({ a_key, a_fontFile });
}
