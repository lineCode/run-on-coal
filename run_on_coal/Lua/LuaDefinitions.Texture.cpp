#include "stdafx.h"
#include "Core/Core.h"
#include "Managers/ElementManager.h"
#include "Managers/LuaManager.h"
#include "Managers/RenderManager.h"
#include "Scene/Texture.h"
#include "Lua/ArgReader.h"
#include "Lua/LuaDefinitions.Texture.h"
#include "Utils/Utils.h"

namespace ROC
{
namespace Lua
{

int textureCreate(lua_State *f_vm)
{
    std::string l_type;
    bool l_compress;
    ArgReader argStream(f_vm,LuaManager::m_corePointer);
    argStream.ReadText(l_type);
    argStream.ReadBoolean(l_compress);
    if(argStream.HasErrors() || !l_type.length())
    {
        lua_pushboolean(f_vm,0);
        return 1;
    }
    Texture *l_tex = NULL;
    int l_iType = Utils::ReadEnumString(l_type,"rgb,rgba,cube");
    switch(l_iType)
    {
    case 0: case 1:
    {
        std::string l_path;
        argStream.ReadText(l_path);
        if(argStream.HasErrors() || !l_path.length())
        {
            lua_pushboolean(f_vm,0);
            return 1;
        }
        l_tex = LuaManager::m_corePointer->GetElementManager()->CreateTexture(l_path,TEXTURE_TYPE_RGB + l_iType,l_compress);
    } break;
    case 2:
    {
        argStream.DecreaseArguments(3);
        std::vector<std::string> l_path;
        argStream.ReadTableTexts(l_path,6);
        if(argStream.HasErrors())
        {
            lua_pushboolean(f_vm,0);
            return 1;
        }
        l_tex = LuaManager::m_corePointer->GetElementManager()->CreateTexture(l_path,l_compress);
    } break;
    }
    l_tex ? lua_pushlightuserdata(f_vm,l_tex) : lua_pushboolean(f_vm,0);
    return 1;
}
int textureDestroy(lua_State *f_vm)
{
    Texture *l_tex;
    ArgReader argStream(f_vm,LuaManager::m_corePointer);
    argStream.ReadUserdata((void**)&l_tex,ElementType::TextureElement);
    if(argStream.HasErrors())
    {
        lua_pushboolean(f_vm,0);
        return 1;
    }
    bool result = LuaManager::m_corePointer->GetElementManager()->DestroyTexture(l_tex);
    lua_pushboolean(f_vm,result);
    return 1;
}
int textureDraw(lua_State *f_vm)
{
    Texture *l_tex;
    lua_Number l_val[5];
    lua_Number l_color[4] = {1.0,1.0,1.0,1.0};
    l_val[4] = 0.0;
    ArgReader argStream(f_vm,LuaManager::m_corePointer);
    argStream.ReadUserdata((void**)&l_tex,ElementType::TextureElement);
    for(int i = 0; i < 4; i++) argStream.ReadNumber(l_val[i]);
    argStream.ReadNextNumber(l_val[4]);
    for(int i = 0; i < 4; i++) argStream.ReadNextNumber(l_color[i]);
    if(argStream.HasErrors())
    {
        lua_pushboolean(f_vm,0);
        return 1;
    }
    glm::vec2 l_pos(l_val[0],l_val[1]);
    glm::vec2 l_size(l_val[2],l_val[3]);
    glm::vec4 l_vColor(l_color[0],l_color[1],l_color[2],l_color[3]);
    LuaManager::m_corePointer->GetRenderManager()->Render(l_tex,l_pos,l_size,static_cast<float>(l_val[4]),l_vColor);
    lua_pushboolean(f_vm,1);
    return 1;
}

}
}
