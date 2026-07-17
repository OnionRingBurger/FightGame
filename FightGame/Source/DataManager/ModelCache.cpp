#include "ModelCache.h"
#include "Debug.h"


ModelCache::ModelCache()
{
}

std::shared_ptr<Model> ModelCache::GetModel(std::string key) const
{
    // cache‚ÉModel‚ª‘¶İ‚µ‚Ä‚¢‚È‚¢‚È‚ç”²‚¯‚é
    if (m_cache.find(key) == m_cache.end())
    {
        DebugMessage(key + "ƒ‚ƒfƒ‹‚ª‘¶İ‚µ‚Ä‚¢‚Ü‚¹‚ñ");
        return nullptr;   
    }
    // ‘Î‰‚·‚éModel‚ğ•Ô‚·
    return m_cache.at(key);
}

void ModelCache::RegisterModel(std::string a_string, std::shared_ptr<Model> a_model)
{
    if (a_model)
    {
        m_cache.insert({ a_string, a_model });
    }
}


