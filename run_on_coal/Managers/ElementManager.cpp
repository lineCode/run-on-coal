#include "stdafx.h"
#include "Core/Core.h"
#include "Managers/ElementManager.h"
#include "Managers/InheritanceManager.h"
#include "Managers/LogManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/PreRenderManager.h"
#include "Managers/RenderManager/RenderManager.h"
#include "Elements/Animation/Animation.h"
#include "Elements/Camera.h"
#include "Elements/Collision.h"
#include "Elements/File.h"
#include "Elements/Font.h"
#include "Elements/Geometry/Geometry.h"
#include "Elements/Light.h"
#include "Elements/Model/Model.h"
#include "Elements/RenderTarget.h"
#include "Elements/Scene.h"
#include "Elements/Shader.h"
#include "Elements/Sound.h"
#include "Elements/Texture.h"
#include "Utils/Utils.h"

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

    std::string l_work, l_path;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(l_anim->Load(l_work)) m_core->GetMemoryManager()->AddMemoryPointer(l_anim);
    else
    {
        delete l_anim;
        l_anim = NULL;
    }
    return l_anim;
}

ROC::Geometry* ROC::ElementManager::CreateGeometry(const std::string &f_path)
{
    Geometry *l_geometry = new Geometry();

    std::string l_work, l_path;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_geometry->Load(l_work)) m_core->GetMemoryManager()->AddMemoryPointer(l_geometry);
    else
    {
        delete l_geometry;
        l_geometry = NULL;
    }
    return l_geometry;
}

ROC::Model* ROC::ElementManager::CreateModel(Geometry *f_geometry)
{
    Model *l_model = new Model(f_geometry);
    if(f_geometry) m_core->GetInheritManager()->SetModelGeometry(l_model, f_geometry);
    m_core->GetMemoryManager()->AddMemoryPointer(l_model);
    m_core->GetPreRenderManager()->AddModel(l_model);
    m_core->GetPhysicsManager()->AddModel(l_model);
    return l_model;
}

ROC::Shader* ROC::ElementManager::CreateShader(const std::string &f_vpath, const std::string &f_fpath, const std::string &f_gpath)
{
    Shader *l_shader = new Shader();

    std::string l_path[3], l_work;
    m_core->GetWorkingDirectory(l_work);

    if(!f_vpath.empty())
    {
        std::string l_vertexPath;
        Utils::AnalyzePath(f_vpath, l_vertexPath);
        l_path[0].append(l_work);
        Utils::JoinPaths(l_path[0], l_vertexPath);
    }
    if(!f_fpath.empty())
    {
        std::string l_fragmentPath;
        Utils::AnalyzePath(f_fpath, l_fragmentPath);
        l_path[1].append(l_work);
        Utils::JoinPaths(l_path[1], l_fragmentPath);
    }
    if(!f_gpath.empty())
    {
        std::string l_geometryPath;
        Utils::AnalyzePath(f_gpath, l_geometryPath);
        l_path[2].append(l_work);
        Utils::JoinPaths(l_path[2], l_geometryPath);
    }
    if(l_shader->Load(l_path[0], l_path[1], l_path[2]))
    {
        m_core->GetMemoryManager()->AddMemoryPointer(l_shader);
        if(m_locked) m_core->GetRenderManager()->RestoreActiveShader(l_shader);
    }
    else
    {
        std::string l_shaderError;
        l_shader->GetError(l_shaderError);
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
        l_shader = NULL;
    }
    return l_shader;
}

ROC::Sound* ROC::ElementManager::CreateSound(const std::string &f_path, bool f_loop)
{
    Sound *l_sound = new Sound(f_loop);

    std::string l_path, l_work;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(l_sound->Load(l_work)) m_core->GetMemoryManager()->AddMemoryPointer(l_sound);
    else
    {
        delete l_sound;
        l_sound = NULL;
    }
    return l_sound;
}

ROC::RenderTarget* ROC::ElementManager::CreateRenderTarget(unsigned int f_num, glm::ivec2 &f_size, int f_type, int f_filter)
{
    RenderTarget *l_rt = new RenderTarget();

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_rt->Create(f_num, f_size, f_type, f_filter)) m_core->GetMemoryManager()->AddMemoryPointer(l_rt);
    else
    {
        std::string l_error;
        l_rt->GetError(l_error);
        m_core->GetLogManager()->Log(l_error);
        delete l_rt;
        l_rt = NULL;
    }
    return l_rt;
}

ROC::Texture* ROC::ElementManager::CreateTexture(const std::string &f_path, int f_type, int f_filter, bool f_compress)
{
    Texture *l_tex = new Texture();

    std::string l_path, l_work;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_tex->Load(l_work, f_type, f_filter, f_compress)) m_core->GetMemoryManager()->AddMemoryPointer(l_tex);
    else
    {
        delete l_tex;
        l_tex = NULL;
    }
    return l_tex;
}
ROC::Texture* ROC::ElementManager::CreateTexture(const std::vector<std::string> &f_path, int f_filter, bool f_compress)
{
    Texture *l_tex = new Texture();

    std::vector<std::string> l_path;
    std::string l_work;
    m_core->GetWorkingDirectory(l_work);

    for(auto iter : f_path)
    {
        std::string l_iterPath;
        std::string l_iterFullPath(l_work);
        Utils::AnalyzePath(iter, l_iterPath);
        Utils::JoinPaths(l_iterFullPath, l_iterPath);
        l_path.push_back(l_iterFullPath);
    }

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_tex->LoadCubemap(l_path, f_filter, f_compress)) m_core->GetMemoryManager()->AddMemoryPointer(l_tex);
    else
    {
        delete l_tex;
        l_tex = NULL;
    }
    return l_tex;
}

ROC::Font* ROC::ElementManager::CreateFont_(const std::string &f_path, int f_size, const glm::ivec2 &f_atlas, int f_filter)
{
    Font *l_font = new Font();

    std::string l_path, l_work;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(m_locked) m_core->GetRenderManager()->ResetCallsReducing();
    if(l_font->LoadTTF(l_work, f_size, f_atlas, f_filter)) m_core->GetMemoryManager()->AddMemoryPointer(l_font);
    else
    {
        delete l_font;
        l_font = NULL;
    }
    return l_font;
}

ROC::File* ROC::ElementManager::CreateFile_(const std::string &f_path)
{
    File *l_file = new File();

    std::string l_path, l_work;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(l_file->Create(l_work, f_path)) m_core->GetMemoryManager()->AddMemoryPointer(l_file);
    else
    {
        delete l_file;
        l_file = NULL;
    }
    return l_file;
}
ROC::File* ROC::ElementManager::OpenFile(const std::string &f_path, bool f_ro)
{
    File *l_file = new File();

    std::string l_path, l_work;
    m_core->GetWorkingDirectory(l_work);
    Utils::AnalyzePath(f_path, l_path);
    Utils::JoinPaths(l_work, l_path);

    if(l_file->Open(l_work, f_path, f_ro)) m_core->GetMemoryManager()->AddMemoryPointer(l_file);
    else
    {
        delete l_file;
        l_file = NULL;
    }
    return l_file;
}

ROC::Collision* ROC::ElementManager::CreateCollision(int f_type, glm::vec3 &f_size)
{
    Collision *l_col = new Collision();
    if(l_col->Create(f_type, f_size))
    {
        m_core->GetMemoryManager()->AddMemoryPointer(l_col);
        m_core->GetPhysicsManager()->AddCollision(l_col);
    }
    else
    {
        delete l_col;
        l_col = NULL;
    }
    return l_col;
}

void ROC::ElementManager::DestroyElement(Element *f_element)
{
    switch(f_element->GetElementType())
    {
        case Element::ElementType::SceneElement:
        {
            m_core->GetRenderManager()->RemoveAsActiveScene(dynamic_cast<Scene*>(f_element));
            m_core->GetInheritManager()->RemoveParentRelation(f_element);
        } break;
        case Element::ElementType::CameraElement: case Element::ElementType::LightElement: case Element::ElementType::RenderTargetElement : case Element::ElementType::TextureElement:
            m_core->GetInheritManager()->RemoveChildRelation(f_element);
            break;
        case Element::ElementType::AnimationElement: case Element::ElementType::GeometryElement:
            m_core->GetInheritManager()->RemoveParentRelation(f_element);
            break;
        case Element::ElementType::ModelElement:
        {
            m_core->GetInheritManager()->RemoveParentRelation(f_element);
            m_core->GetInheritManager()->RemoveChildRelation(f_element);
            m_core->GetPreRenderManager()->RemoveModel(dynamic_cast<Model*>(f_element));
            m_core->GetPhysicsManager()->RemoveModel(dynamic_cast<Model*>(f_element));
        } break;
        case Element::ElementType::ShaderElement:
        {
            m_core->GetRenderManager()->RemoveAsActiveShader(dynamic_cast<Shader*>(f_element));
            m_core->GetInheritManager()->RemoveParentRelation(f_element);
        } break;
        case Element::ElementType::CollisionElement:
            m_core->GetPhysicsManager()->RemoveCollision(dynamic_cast<Collision*>(f_element));
            break;
    }
    m_core->GetMemoryManager()->RemoveMemoryPointer(f_element);
    delete f_element;
}
void ROC::ElementManager::DestroyElementByPointer(void *f_pointer)
{
    //Called only at the end of work
    delete reinterpret_cast<Element*>(f_pointer);
}
