#ifndef NORENGINE_ASSETMANAGER_H
#define NORENGINE_ASSETMANAGER_H


#include <SDL_gpu.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Texture.h"
#include "NFont.h"

class AssetManager {
    bool loaded;
    std::unordered_map<std::string, Texture *> assets;
    std::vector<std::string> sheets;

    // TODO: screen based asset loading. Not the brightest idea tho...
    std::unordered_map<const char *, std::unordered_map<std::string, Texture *>> assetsGroup;

public:
    std::string basePathStr = "";

    Texture *loadImage(std::string);
    Texture *getAsset(std::string path);

    void addAssets();
    void addAsset(std::string path);
    void loadAssetsAsync();
    bool isAssetsLoaded();

    void unloadAssets(const char *holder);

    AssetManager();

    void update(double dt);

    ~AssetManager();

    NFont *getDefaultFont();

    void loadSheet(std::string sheet);

    int loadFile(std::string file, char *&data);

private:
    void addSheet(std::string path);

};


#endif //NORENGINE_ASSETMANAGER_H
