#include "stdafx.h"

#include "Managers/ElementManager.h"
#include "Core/Core.h"
#include "Elements/Animation/Animation.h"
#include "Elements/Camera.h"
#include "Elements/Collision.h"
#include "Elements/File.h"
#include "Elements/Font.h"
#include "Elements/Geometry/Geometry.h"
#include "Elements/Light.h"
#include "Elements/Model/Model.h"
#include "Elements/Movie.h"
#include "Elements/RenderTarget.h"
#include "Elements/Scene.h"
#include "Elements/Shader/Shader.h"
#include "Elements/Sound.h"
#include "Elements/Texture.h"

#include "Managers/AsyncManager.h"
#include "Managers/InheritanceManager.h"
#include "Managers/LogManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/PreRenderManager.h"
#include "Managers/RenderManager/RenderManager.h"
#include "Utils/PathUtils.h"

ROC::ElementManager::ElementManager(Core *f_core)
{
    m_core = f_core;
    m_locked = false;
}
ROC::ElementManager::~ElementManager()
{}

ROC::Scene* ROC::ElementManager::CreateScene()
{
    ROC::Scene *l_scene = new Scene();
    m_core->GetMemoryManager()->AddMemoryPointer(l_scene);
    return l_scene;
}

ROC::Camera* ROC::ElementManager::CreateCamera(int f_type)
{
    Camera *l_camera = new Camera(f_type);

    m_core->GetMemoryManager()->AddMemoryPointer(l_camera);
    return l_camera;
}

ROC::Light* ROC::ElementManager::CreateLight()
{
    Light *l_light = new Light();
    m_core->GetMemoryManager()->AddMemoryPointer(l_light);
    return l_light;
}

ROC::Animation* ROC::ElementManager::CreateAnimation(const std::string &f_path)
{
    Animation *l_anim = new Animation();

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(l_anim->Load(l_path)) m_core->GetMemoryManager()->AddMemoryPointer(l_anim);
    else
    {
        delete l_anim;
        l_anim = nullptr;
    }
    return l_anim;
}

ROC::Geometry* ROC::ElementManager::CreateGeometry(const std::string &f_path, bool f_async)
{
    Geometry *l_geometry = new Geometry(f_async);

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(!f_async && m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(f_async)
    {
        m_core->GetMemoryManager()->AddMemoryPointer(l_geometry);
        m_core->GetAsyncManager()->AddGeometryToQueue(l_geometry, l_path);
    }
    else
    {
        if(l_geometry->Load(l_path)) m_core->GetMemoryManager()->AddMemoryPointer(l_geometry);
        else
        {
            delete l_geometry;
            l_geometry = nullptr;
        }
    }
    return l_geometry;
}

ROC::Model* ROC::ElementManager::CreateModel(Geometry *f_geometry)
{
    Model *l_model = nullptr;
    if(f_geometry)
    {
        if(f_geometry->IsLoaded())
        {
            l_model = new Model(f_geometry);
            m_core->GetInheritManager()->SetModelGeometry(l_model, f_geometry);
        }
    }
    else l_model = new Model(nullptr);
    if(l_model)
    {
        m_core->GetMemoryManager()->AddMemoryPointer(l_model);
        m_core->GetPreRenderManager()->AddModel(l_model);
        m_core->GetPhysicsManager()->AddModel(l_model);
    }
    return l_model;
}

ROC::Shader* ROC::ElementManager::CreateShader(const std::string &f_vpath, const std::string &f_fpath, const std::string &f_gpath)
{
    Shader *l_shader = new Shader();

    std::string l_path[3] = { f_vpath, f_fpath, f_gpath };

    if(!f_vpath.empty())
    {
        PathUtils::EscapePath(l_path[0]);
        l_path[0].insert(0U, m_core->GetWorkingDirectory());
    }
    if(!f_fpath.empty())
    {
        PathUtils::EscapePath(l_path[1]);
        l_path[1].insert(0U, m_core->GetWorkingDirectory());
    }
    if(!f_gpath.empty())
    {
        PathUtils::EscapePath(l_path[2]);
        l_path[2].insert(0U, m_core->GetWorkingDirectory());
    }
    if(m_locked)  m_core->GetRenderManager()->DisableActiveShader();
    if(l_shader->Load(l_path[0], l_path[1], l_path[2])) m_core->GetMemoryManager()->AddMemoryPointer(l_shader);
    else
    {
        const std::string &l_shaderError = l_shader->GetError();
        if(!l_shaderError.empty())
        {
            std::string l_error("[");
            l_error.append(f_vpath);
            l_error.push_back(',');
            l_error.append(f_fpath);
            l_error.push_back(',');
            l_error.append(f_gpath);
            l_error.append("] -> ");
            l_error.append(l_shaderError);
            m_core->GetLogManager()->Log(l_error);
        }
        delete l_shader;
        l_shader = nullptr;
    }
    if(m_locked) m_core->GetRenderManager()->EnableActiveShader();
    return l_shader;
}

ROC::Sound* ROC::ElementManager::CreateSound(const std::string &f_path, bool f_loop)
{
    Sound *l_sound = new Sound(f_loop);

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(l_sound->Load(l_path)) m_core->GetMemoryManager()->AddMemoryPointer(l_sound);
    else
    {
        delete l_sound;
        l_sound = nullptr;
    }
    return l_sound;
}

ROC::RenderTarget* ROC::ElementManager::CreateRenderTarget(int f_type, const glm::ivec2 &f_size, int f_filter)
{
    RenderTarget *l_rt = new RenderTarget();

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_rt->Create(f_type, f_size, f_filter)) m_core->GetMemoryManager()->AddMemoryPointer(l_rt);
    else
    {
        m_core->GetLogManager()->Log(l_rt->GetError());
        delete l_rt;
        l_rt = nullptr;
    }
    return l_rt;
}

ROC::Texture* ROC::ElementManager::CreateTexture(const std::string &f_path, int f_type, int f_filter, bool f_compress)
{
    Texture *l_texture = new Texture();

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_texture->Load(l_path, f_type, f_filter, f_compress)) m_core->GetMemoryManager()->AddMemoryPointer(l_texture);
    else
    {
        delete l_texture;
        l_texture = nullptr;
    }
    return l_texture;
}
ROC::Texture* ROC::ElementManager::CreateTexture(const std::vector<std::string> &f_path, int f_filter, bool f_compress)
{
    Texture *l_texture = new Texture();

    std::vector<std::string> l_path;
    for(auto iter : f_path)
    {
        std::string l_iterPath(iter);
        PathUtils::EscapePath(iter);
        l_iterPath.insert(0U, m_core->GetWorkingDirectory());
        l_path.push_back(l_iterPath);
    }

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_texture->LoadCubemap(l_path, f_filter, f_compress)) m_core->GetMemoryManager()->AddMemoryPointer(l_texture);
    else
    {
        delete l_texture;
        l_texture = nullptr;
    }
    return l_texture;
}

ROC::Font* ROC::ElementManager::CreateFont_(const std::string &f_path, int f_size, const glm::ivec2 &f_atlas, int f_filter)
{
    Font *l_font = new Font();

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_font->Load(l_path, f_size, f_atlas, f_filter)) m_core->GetMemoryManager()->AddMemoryPointer(l_font);
    else
    {
        delete l_font;
        l_font = nullptr;
    }
    return l_font;
}

ROC::File* ROC::ElementManager::CreateFile_(const std::string &f_path)
{
    File *l_file = new File();

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(l_file->Create(l_path, f_path)) m_core->GetMemoryManager()->AddMemoryPointer(l_file);
    else
    {
        delete l_file;
        l_file = nullptr;
    }
    return l_file;
}
ROC::File* ROC::ElementManager::OpenFile(const std::string &f_path, bool f_ro)
{
    File *l_file = new File();

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(l_file->Open(l_path, f_path, f_ro)) m_core->GetMemoryManager()->AddMemoryPointer(l_file);
    else
    {
        delete l_file;
        l_file = nullptr;
    }
    return l_file;
}

ROC::Collision* ROC::ElementManager::CreateCollision(int f_type, glm::vec3 &f_size, float f_mass)
{
    Collision *l_col = new Collision();

    if(l_col->Create(f_type, f_size, f_mass))
    {
        m_core->GetMemoryManager()->AddMemoryPointer(l_col);
        m_core->GetPhysicsManager()->AddCollision(l_col);
    }
    else
    {
        delete l_col;
        l_col = nullptr;
    }
    return l_col;
}

ROC::Movie* ROC::ElementManager::CreateMovie(const std::string &f_path)
{
    Movie *l_movie = new Movie();

    std::string l_path(f_path);
    PathUtils::EscapePath(l_path);
    l_path.insert(0U, m_core->GetWorkingDirectory());

    if(l_movie->Load(l_path))
    {
        m_core->GetMemoryManager()->AddMemoryPointer(l_movie);
        m_core->GetRenderManager()->AddMovie(l_movie);
    }
    else
    {
        delete l_movie;
        l_movie = nullptr;
    }
    return l_movie;
}

bool ROC::ElementManager::DestroyElement(Element *f_element)
{
    bool l_result = false;
    if(m_core->GetMemoryManager()->IsValidMemoryPointer(f_element))
    {
        switch(f_element->GetElementType())
        {
            case Element::ET_Scene:
            {
                m_core->GetRenderManager()->RemoveAsActiveScene(reinterpret_cast<Scene*>(f_element));
                m_core->GetInheritManager()->RemoveParentRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_Camera: case Element::ET_Light: case Element::ET_Texture:
            {
                m_core->GetInheritManager()->RemoveChildRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_RenderTarget:
            {
                m_core->GetRenderManager()->RemoveAsActiveTarget(reinterpret_cast<RenderTarget*>(f_element));
                m_core->GetInheritManager()->RemoveChildRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_Animation:
            {
                m_core->GetInheritManager()->RemoveParentRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_Geometry:
            {
                Geometry *l_geometry = reinterpret_cast<Geometry*>(f_element);
                if(!l_geometry->IsAsyncLoad() || l_geometry->IsReleased())
                {
                    m_core->GetInheritManager()->RemoveParentRelations(f_element);
                    m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                    delete l_geometry;
                    l_result = true;
                }
            } break;

            case Element::ET_Model:
            {
                m_core->GetInheritManager()->RemoveParentRelations(f_element);
                m_core->GetInheritManager()->RemoveChildRelations(f_element);
                m_core->GetPreRenderManager()->RemoveModel(reinterpret_cast<Model*>(f_element));
                m_core->GetPhysicsManager()->RemoveModel(reinterpret_cast<Model*>(f_element));
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_Shader:
            {
                m_core->GetRenderManager()->RemoveAsActiveShader(reinterpret_cast<Shader*>(f_element));
                m_core->GetInheritManager()->RemoveParentRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_Collision:
            {
                m_core->GetPhysicsManager()->RemoveCollision(reinterpret_cast<Collision*>(f_element));
                m_core->GetInheritManager()->RemoveChildRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;

            case Element::ET_Movie:
            {
                m_core->GetRenderManager()->RemoveMovie(reinterpret_cast<Movie*>(f_element));
                m_core->GetInheritManager()->RemoveChildRelations(f_element);
                m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
                delete f_element;
                l_result = true;
            } break;
        }
    }
    return l_result;
}
void ROC::ElementManager::DestroyElementByPointer(void *f_ptr)
{
    delete reinterpret_cast<Element*>(f_ptr);
}
