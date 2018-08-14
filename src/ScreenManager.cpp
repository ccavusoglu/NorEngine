#include <SDL_rwops.h>
#include <SDL_log.h>
#include "ScreenManager.h"
#include <string>
#include <Definitions.h>
#include <Game.h>
#include <EntityManager.h>

#ifdef WIN32

#include <windows.h>

#endif

const char *ATTR_NAME = "name";
const char *ATTR_X = "x";
const char *ATTR_Y = "y";
const char *ATTR_W = "w";
const char *ATTR_H = "h";
const char *ATTR_ANCHOR_X = "anchorX";
const char *ATTR_ANCHOR_Y = "anchorY";
const char *ATTR_SCALE_X = "scaleX";
const char *ATTR_SCALE_Y = "scaleY";
const char *ATTR_TAG = "tag";
const char *ATTR_TEXTURE = "texture";
const char *ATTR_COMPONENTS = "components";
const char *ATTR_ZORDER = "zOrder";
const char *ATTR_COLOR = "color";
const char *ATTR_TEXT = "text";
const char *ATTR_BODYOFFSET = "bodyOffset";
const char *ATTR_INPUTOFFSET = "inputOffset";
const char *ATTR_TEXTURES = "textures";
const char *ATTR_INTERVAL = "interval";

const char *TYPE_SPRITE = "sprite";
const char *TYPE_TEXT = "text";
const char *TYPE_ANIMATION = "animation";

long fileLastModificationTimeHigh = 0;
long fileLastModificationTimeLow = 0;

std::vector<ComponentType> parseComponents(std::vector<std::string> components) {
    std::vector<ComponentType> temp;

    for (auto comp : components) {
        if (comp == "BODY") temp.push_back(BODY);
        else if (comp == "ENEMY") temp.push_back(ENEMY);
        else if (comp == "INPUT_C") temp.push_back(INPUT_C);
        else if (comp == "MOTION") temp.push_back(MOTION);
        else if (comp == "SPRITE") temp.push_back(SPRITE);
        else if (comp == "TEXT") temp.push_back(TEXT);
        else if (comp == "TRANSFORM") temp.push_back(TRANSFORM);
        else SDL_Log("Component Type not defined. %s", comp);
    }

    return temp;
}

ZOrder parseZOrder(const char *zOrder) {
    if (zOrder == nullptr) return DEFAULT;

    auto z = std::string(zOrder);

    if (z == "BACKGROUND") return BACKGROUND;
    if (z == "BACK") return BACK;
    if (z == "TOP") return TOP;
    if (z == "POPUP") return POPUP;

    return DEFAULT;
}

glm::vec4 parseVec4(const char *vec4) {
    if (vec4 == nullptr) return {0, 0, 0, 0};

    auto parts = Util::splitString(vec4, ",");

    return {Util::toFloat(parts[0]), Util::toFloat(parts[1]), Util::toFloat(parts[2]), Util::toFloat(parts[3])};
}

void ScreenManager::prepareScreen(ScreenType type) {
    std::string screen = "";

    switch (type) {
        case SPLASH:
            screen = "SplashScreen";
            break;
        case MENU:
            screen = "MenuScreen";
            break;
        case GAME:
            screen = "GameScreen";
            break;
    }

    char *buffer = nullptr;

    if (AM->loadFile(screensFile, buffer) < 1) return;

    tinyxml2::XMLDocument document;

    auto res = document.Parse(buffer);

    auto root = document.FirstChild();
    auto screenRoot = root->FirstChildElement(screen.c_str());

    if (screenRoot != nullptr) {
        auto firstElement = screenRoot->FirstChild();

        while (firstElement != nullptr) {
            createEntity(firstElement->ToElement());

            firstElement = firstElement->NextSibling();
        }
    } else {
        SDL_Log("Not found in screen.xml");
    }
}

void ScreenManager::createEntity(tinyxml2::XMLElement *pElement) {
    const char *type;
    const char *name;
    float x, y;
    float w = -1, h = -1;
    float anchorX = 0.5f, anchorY = 0.5f;
    float scaleX = 1.0f, scaleY = 1.0f;
    ZOrder zOrder;
    const char *tag;
    const char *texture = nullptr;
    const char *components = nullptr;
    const char *bodyOffset = nullptr;
    const char *inputOffset = nullptr;

    type = pElement->Value();
    name = pElement->Attribute(ATTR_NAME);
    pElement->QueryAttribute(ATTR_X, &x);
    pElement->QueryAttribute(ATTR_Y, &y);
    pElement->QueryAttribute(ATTR_W, &w);
    pElement->QueryAttribute(ATTR_H, &h);
    pElement->QueryAttribute(ATTR_ANCHOR_X, &anchorX);
    pElement->QueryAttribute(ATTR_ANCHOR_Y, &anchorY);
    tag = pElement->Attribute(ATTR_TAG);
    texture = pElement->Attribute(ATTR_TEXTURE);
    components = pElement->Attribute(ATTR_COMPONENTS);
    zOrder = parseZOrder(pElement->Attribute(ATTR_ZORDER));
    bodyOffset = pElement->Attribute(ATTR_BODYOFFSET);
    inputOffset = pElement->Attribute(ATTR_INPUTOFFSET);

    auto entityBuilder = EM->create(name, {x, y, w, h}, (Tag) Util::getTag(tag));

    if (texture != nullptr) entityBuilder.texture(texture, {anchorX, anchorY}, zOrder);

    if (components != nullptr) {
        auto comps = parseComponents(Util::splitString(components, ","));

        for (auto comp : comps) {
            entityBuilder.component(comp);
        }
    }

    if (bodyOffset != nullptr) {
        entityBuilder.bodyOffset(parseVec4(bodyOffset));
    }

    if (inputOffset != nullptr) {
        entityBuilder.inputOffset(parseVec4(inputOffset));
    }

    if (!strcmp(type, TYPE_TEXT)) {
        const char *color = nullptr;
        const char *text = "";

        color = pElement->Attribute(ATTR_COLOR);
        text = pElement->Attribute(ATTR_TEXT);
        pElement->QueryAttribute(ATTR_SCALE_X, &scaleX);
        pElement->QueryAttribute(ATTR_SCALE_Y, &scaleY);

        entityBuilder.text(text, Util::getSDLColor(color), {scaleX, scaleY});
    }

    if (!strcmp(type, TYPE_ANIMATION)) {
        const char *textures = nullptr;
        int interval = 100;

        textures = pElement->Attribute(ATTR_TEXTURES);
        pElement->QueryAttribute(ATTR_INTERVAL, &interval);

        entityBuilder.animation(Util::splitString(textures, ","), interval, {anchorX, anchorY}, zOrder);
    }
}

bool ScreenManager::checkScreenFileChange(ScreenType type) {
    FILETIME ftCreate, ftAccess, ftWrite;
    auto fullPath = AM->basePathStr + std::string(screensFile);
    HANDLE hFile = CreateFile(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) return false;

    if (fileLastModificationTimeHigh == 0) {
        fileLastModificationTimeHigh = ftWrite.dwHighDateTime;
        fileLastModificationTimeLow = ftWrite.dwLowDateTime;
    }

    CloseHandle(hFile);

    bool val = (fileLastModificationTimeHigh < ftWrite.dwHighDateTime) ||
               (fileLastModificationTimeLow < ftWrite.dwLowDateTime);

    fileLastModificationTimeHigh = ftWrite.dwHighDateTime;
    fileLastModificationTimeLow = ftWrite.dwLowDateTime;

    return val;
}
