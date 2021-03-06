#include "stdafx.h"

#include "Lua/LuaDefs/LuaMovieDef.h"
#include "Lua/LuaDefs/LuaDrawableDef.h"
#include "Lua/LuaDefs/LuaElementDef.h"

#include "Core/Core.h"
#include "Managers/ElementManager.h"
#include "Managers/LuaManager.h"
#include "Managers/MemoryManager.h"
#include "Elements/Movie.h"
#include "Lua/ArgReader.h"
#include "Utils/LuaUtils.h"

void ROC::LuaMovieDef::Init(lua_State *f_vm)
{
    LuaUtils::AddClass(f_vm, "Movie", Create);
    LuaUtils::AddClassMethod(f_vm, "play", Play);
    LuaUtils::AddClassMethod(f_vm, "pause", Pause);
    LuaUtils::AddClassMethod(f_vm, "stop", Stop);
    LuaUtils::AddClassMethod(f_vm, "getSampleRate", GetSampleRate);
    LuaUtils::AddClassMethod(f_vm, "getChannelCount", GetChannelCount);
    LuaUtils::AddClassMethod(f_vm, "getFramerate", GetFramerate);
    LuaUtils::AddClassMethod(f_vm, "getDuration", GetDuration);
    LuaUtils::AddClassMethod(f_vm, "getVolume", GetVolume);
    LuaUtils::AddClassMethod(f_vm, "setVolume", SetVolume);
    LuaUtils::AddClassMethod(f_vm, "getTime", GetTime);
    LuaUtils::AddClassMethod(f_vm, "setTime", SetTime);
    LuaDrawableDef::AddHierarchyMethods(f_vm);
    LuaElementDef::AddHierarchyMethods(f_vm);
    LuaUtils::AddClassFinish(f_vm);
}

int ROC::LuaMovieDef::Create(lua_State *f_vm)
{
    // element Movie(str path)
    std::string l_path;
    ArgReader argStream(f_vm);
    argStream.ReadText(l_path);
    if(!argStream.HasErrors() && !l_path.empty())
    {
        Movie *l_movie = LuaManager::GetCore()->GetElementManager()->CreateMovie(l_path);
        l_movie ? argStream.PushElement(l_movie) : argStream.PushBoolean(false);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::Play(lua_State *f_vm)
{
    // bool Movie:play()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    if(!argStream.HasErrors())
    {
        l_movie->Play();
        argStream.PushBoolean(true);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::Pause(lua_State *f_vm)
{
    // bool Movie:pause()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    if(!argStream.HasErrors())
    {
        l_movie->Pause();
        argStream.PushBoolean(true);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::Stop(lua_State *f_vm)
{
    // bool Movie:stop()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    if(!argStream.HasErrors())
    {
        l_movie->Stop();
        argStream.PushBoolean(true);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::GetSampleRate(lua_State *f_vm)
{
    // int Movie:getSampleRate()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    !argStream.HasErrors() ? argStream.PushInteger(l_movie->GetSampleRate()) : argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::GetChannelCount(lua_State *f_vm)
{
    // int Movie:getChannelCount()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    !argStream.HasErrors() ? argStream.PushInteger(l_movie->GetChannelCount()) : argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::GetFramerate(lua_State *f_vm)
{
    // float Movie:getFramerate()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    !argStream.HasErrors() ? argStream.PushNumber(l_movie->GetFramerate()) : argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::GetDuration(lua_State *f_vm)
{
    // float Movie:getDuration()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    !argStream.HasErrors() ? argStream.PushNumber(l_movie->GetDuration()) : argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::GetVolume(lua_State *f_vm)
{
    // float Movie:getVolume()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    !argStream.HasErrors() ? argStream.PushNumber(l_movie->GetVolume()) : argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::SetVolume(lua_State *f_vm)
{
    // bool Movie:setVolume(float volume)
    Movie *l_movie;
    float l_volume;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    argStream.ReadNumber(l_volume);
    if(!argStream.HasErrors())
    {
        l_movie->SetVolume(l_volume);
        argStream.PushBoolean(true);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::GetTime(lua_State *f_vm)
{
    // float Movie:getTime()
    Movie *l_movie;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    !argStream.HasErrors() ? argStream.PushNumber(l_movie->GetTime()) : argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
int ROC::LuaMovieDef::SetTime(lua_State *f_vm)
{
    // bool Movie:setTime(float time)
    Movie *l_movie;
    float l_time;
    ArgReader argStream(f_vm);
    argStream.ReadElement(l_movie);
    argStream.ReadNumber(l_time);
    if(!argStream.HasErrors())
    {
        l_movie->SetTime(l_time);
        argStream.PushBoolean(true);
    }
    else argStream.PushBoolean(false);
    return argStream.GetReturnValue();
}
