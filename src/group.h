#ifndef GROUP_H
#define GROUP_H

#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

class Group
{
public:
    std::string name;
    unsigned int gid;
    std::string password;
    std::string members;

    Group(std::string& n)
    {
        name = n;
    }
    Group(unsigned g)
    {
        gid = g;
    }
    Group(std::vector<std::string>& tokens, bool bsd);
    Group(YAML::const_iterator& it)
    {
        gid = it->first.as<unsigned>();
        assert(it->second.Type() == YAML::NodeType::Map);
        if(it->second["name"])
            name = it->second["name"].as<std::string>();
        if(it->second["members"])
            members = it->second["members"].as<std::string>();
    }
    Group(const Group& g)
    {
        name = g.name;
        gid = g.gid;
        password = g.password;
        members = g.members;
    }
    ~Group() { }

    void printGroup(std::ostream& out) const
    {
        out << name << ":" << password << ":" << gid << ":" << members << std::endl;
    }
    void printGroupLinux(std::ostream& out) const
    {
        out << name << ":x:" << gid << ":" << members << std::endl;
    }
    void printGroupYAML(YAML::Emitter& out) const
    {
        out << YAML::Key << gid;
        out << YAML::Value << YAML::Flow << YAML::BeginMap << YAML::Key << "name" << YAML::Value << name
            << YAML::Key << "members" << YAML::Value << members << YAML::EndMap;

    }
    void printGroupBSD(std::ostream& out) const
    {
        out << name << ":*:" << gid << ":" << members << std::endl;
    }
    void printGShadow(std::ostream& out) const
    {
        out << name << ":!::" << members << std::endl;
    }
    bool operator==(const Group& g2) const
    {
        if(!name.empty() && !g2.name.empty())
            return name.compare(g2.name) == 0;
        return gid == g2.gid;
    }
    bool operator==(const Group* g2) const
    {
        if(!name.empty() && !g2->name.empty())
            return name.compare(g2->name) == 0;
        return gid == g2->gid;
    }
};
inline std::ostream& operator<<(std::ostream& out, const Group& g)
{
    g.printGroup(out);
    return out;
};
inline YAML::Emitter& operator<<(YAML::Emitter& out, const Group& g)
{
    g.printGroupYAML(out);
    return out;
};

struct GroupOrder {
    bool operator() (const Group& a, const Group& b) const
    {
        return a.gid < b.gid;
    }
};
#endif
