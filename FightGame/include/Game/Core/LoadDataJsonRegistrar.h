#pragma once

#include <string>

#include "Loading.h"

class LoadDataJsonRegistrar
{
public:
	LoadDataJsonRegistrar(LoadDatas& a_loadDatas);

	// Json‚ğ“Ç‚İ‚ñ‚ÅLoadDatas‚É•Û‘¶
	bool RegisterFromManifest(const LoadManifestPaths& a_paths);

private:
	LoadDatas& loadDatas;
};
