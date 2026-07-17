#include "UICache.h"
#include "Debug.h"

UICache::UICache()
{
}


void UICache::RegisterModel(std::string a_key, std::shared_ptr<Texture> a_texture)
{
    if (!a_texture) return;
    // cache‚É“o˜^
    m_textureCache.insert({ a_key, a_texture });
    
}

std::shared_ptr<Texture> UICache::GetTexture(std::string a_key) const
{

    // cache‚ÉModel‚ª‘¶İ‚µ‚Ä‚¢‚È‚¢‚È‚ç“Ç‚İ‚Ş
    if (m_textureCache.find(a_key) == m_textureCache.end())
    {
        // “Ç‚İ‚İ‚É¸”s‚µ‚½‚È‚çnullptr‚ğ•Ô‚·
        return nullptr;
        
    }
    // ‘Î‰‚·‚éModel‚ğ•Ô‚·
    return m_textureCache.at(a_key);
}

