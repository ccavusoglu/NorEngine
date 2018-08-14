#include <tinyxml2.h>
#include "AssetManager.h"
#include "SignalManager.h"

AssetManager::AssetManager() {
#ifdef WIN32
    auto basePath = SDL_GetBasePath();
    basePathStr = std::string(SDL_GetBasePath()) + std::string("../assets/");
    SDL_free(basePath);
#endif

    assets = std::unordered_map<std::string, Texture *>();
}

AssetManager::~AssetManager() {
    for (auto asset : assets) {
        if (asset.second == nullptr) continue;

        delete asset.second;
        asset.second = nullptr;
    }
}

Texture *AssetManager::loadImage(std::string path) {
    const auto fullPath = basePathStr + path;
    auto temp = GPU_LoadImage(fullPath.c_str());

    if (temp == nullptr) {
        GPU_LogInfo("Error: %d\n", GPU_PopErrorCode().error);
        return nullptr;
        // TODO: render green solid image instead of breaking the game!
    }

    auto tex = new Texture(temp, path, 0, 0, temp->w, temp->h);
    assets[path] = tex;
    return tex;
}

void AssetManager::addAsset(std::string path) {
    if (assets.count(path) == 0) {
        assets[path] = nullptr;
    }
}

void AssetManager::loadAssetsAsync() {
    loaded = false;

    for (auto a : assets) {
        if (a.second == nullptr)
            assets[a.first] = loadImage(a.first);
    }

    for (auto s : sheets) {
        loadSheet(s);
    }

    SDL_Log("Assets Loaded: %d", assets.size());
    loaded = true;
}

bool AssetManager::isAssetsLoaded() {
    return loaded;
}

void AssetManager::unloadAssets(const char *holder) {
    // TODO: cuz of dumb seg fault on windows. load once never unload!
//    assets.erase(assets.begin(), assets.end());
}

Texture *AssetManager::getAsset(std::string path) {
    path = path + ".png";

    if (assets.count(path) == 0) {
        for (auto a : assets) {
            std::cout << "Path: " << path << " Added: " << (a.second->name == path) << std::endl;
        }
    }

    return assets[path];
}

void AssetManager::update(double dt) {

}

void AssetManager::addAssets() {
    addSheet("sprites");
}

NFont *AssetManager::getDefaultFont() {
#ifndef WIN32
    return new NFont("fonts/SHOWG.ttf", 72);
#else
    return new NFont("../assets/fonts/SHOWG.ttf", 72);
#endif
}

void AssetManager::addSheet(std::string path) {
    addAsset(path + ".png");
    sheets.push_back(path + ".xml");
}

void AssetManager::loadSheet(std::string sheet) {
    const char *ATTR_NAME = "name";
    const char *ATTR_X = "x";
    const char *ATTR_Y = "y";
    const char *ATTR_WIDTH = "width";
    const char *ATTR_HEIGHT = "height";
    const char *ATTR_IMAGE_PATH = "imagePath";

    char *data = nullptr;

    if (loadFile(sheet, data) < 1) return;

    tinyxml2::XMLDocument document;

    auto res = document.Parse(data);

    auto root = document.FirstChild();

    const char *imagePath = root->ToElement()->Attribute(ATTR_IMAGE_PATH);

    auto first = root->FirstChild();

    while (first != nullptr) {
        auto element = first->ToElement();
        int x, y, width, height;

        const char *name = element->Attribute(ATTR_NAME);
        element->QueryAttribute(ATTR_X, &x);
        element->QueryAttribute(ATTR_Y, &y);
        element->QueryAttribute(ATTR_WIDTH, &width);
        element->QueryAttribute(ATTR_HEIGHT, &height);

        auto tex = new Texture(assets[imagePath]->rawImage, std::string(name), x, y, width, height);
        assets[name] = tex;

        first = first->NextSibling();
    }
}

int AssetManager::loadFile(std::string path, char *&data) {
    auto fullPath = AM->basePathStr + path;

    auto file = SDL_RWFromFile(fullPath.c_str(), "r");

    if (file != nullptr) {
        auto length = (size_t) SDL_RWseek(file, 0, RW_SEEK_END);
        data = new char[length];
        SDL_RWseek(file, 0, RW_SEEK_SET);

        if (!SDL_RWread(file, data, 1, length)) {
            SDL_Log("File empty or broken: %s", path);
            return -1;
        };

        // NULL terminate array
        data[length] = '\0';
    } else {
        SDL_Log("Cannot read: %s", path);
        return 0;
    }

    SDL_RWclose(file);

    return 1;
}
