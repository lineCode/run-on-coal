#pragma once

namespace ROC
{

class Core;
class Model;
class LuaArguments;
class TreeNode;
typedef void(*OnPreRender)(void);

class PreRenderManager final
{
    Core *m_core;

    TreeNode *m_modelTreeRoot;
    std::unordered_map<Model*, TreeNode*> m_modelToNodeMap;
    std::unordered_map<Model*, TreeNode*>::iterator m_modelToNodeMapEnd;

    std::vector<TreeNode*> m_nodeStack;

    LuaArguments *m_argument;
    OnPreRender m_callback;

    PreRenderManager(const PreRenderManager& that);
    PreRenderManager &operator =(const PreRenderManager &that);
public:
    inline void SetPreRenderCallback(OnPreRender f_callback) { m_callback = f_callback; }
protected:
    explicit PreRenderManager(Core *f_core);
    ~PreRenderManager();

    void AddModel(Model *f_model);
    void RemoveModel(Model *f_model);

    void AddLink(Model *f_model, Model *f_parent);
    void RemoveLink(Model *f_model);

    void DoPulse_S1();
    void DoPulse_S2();

    friend class Core;
    friend class ElementManager;
    friend class InheritanceManager;
};

}
