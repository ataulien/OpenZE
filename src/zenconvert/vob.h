#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace ZenConvert
{
    class Vob
    {
    public:
        Vob(const std::string &name, const std::string &className, int classVersion, Vob *pParent = nullptr, int objectID = 0);
        ~Vob();

        Vob *addVob(const std::string &name, const std::string &className, int classVersion, int objectID = 0, Vob *pReference = nullptr);
        void addAttribute(const std::string &name, const std::string &value);

        const std::string &name() const { return m_Name; }
        const std::string &className() const { return m_ClassName; }
        const Vob *reference() const { return m_pReference; }

        int childCount() const { return m_Vobs.size(); }
        Vob * const child(int i) { return m_Vobs[i]; }

    private:
        std::string m_Name;
        std::string m_ClassName;
        int m_ClassVersion;
        int m_ObjectID;
        Vob *m_pParent;
        std::unordered_map<std::string, std::string> m_Attributes;
        std::vector<Vob *> m_Vobs;
        Vob *m_pReference;
    };
}
