#include "sylar/config.h"
// static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT;

#ifdef AAAA
sylar::ConfigVar<int>::ptr g_int_value_config =
    sylar::Config::Lookup("system.port", (int)8080, "system port");

sylar::ConfigVar<float>::ptr g_int_valuex_config =
    sylar::Config::Lookup("system.port", (float)8080, "system port");

sylar::ConfigVar<float>::ptr g_float_value_config =
    sylar::Config::Lookup("system.value", (float)118.32f, "system value");
    
sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
    sylar::Config::Lookup("system.int_vec", std::vector<int>{1,2,4}, "system int vec");
    
sylar::ConfigVar<std::list<int>>::ptr g_int_list_value_config =
    sylar::Config::Lookup("system.int_list", std::list<int>{1,2,4}, "system int list");

sylar::ConfigVar<std::set<int>>::ptr g_int_set_value_config =
    sylar::Config::Lookup("system.int_set", std::set<int>{1,2,4}, "system int set");

sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config =
    sylar::Config::Lookup("system.int_uset", std::unordered_set<int>{1,2,4}
        , "system int uset");

sylar::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config =
    sylar::Config::Lookup("system.str_int_map", std::map<std::string, int>{
        {"key", 14}, {"key2", 25}}, "system str int map");

sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config =
    sylar::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{
        {"key", 104}, {"key2", 205}}, "system str int umap");

void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    }else if(node.IsNull()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    }else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                << it->first << " - "  << it->second.Type() << " - " << level;
            print_yaml(it->second, level+1);
        }
    }else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - "  << node[i].Type() << " - " << level;
            print_yaml(node[i], level+1);
        }
    }
}

void test_yaml () {
    YAML::Node root = YAML::LoadFile("/root/sylar/bin/conf/test.yaml");
    print_yaml(root, 0);
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root.Scalar();
}



void test_config() {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "befor: " << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "befor: " << g_float_value_config->toString();
#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v) { \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix" "#name": " << i; \
        } \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix" "#name" yaml: " << g_var->toString(); \
    }

#define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v) { \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix" "#name": {" \
                    << i.first << " - " << i.second << "}"; \
        } \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix" "#name" yaml: " << g_var->toString(); \
    }

    XX(g_int_vec_value_config, int_vec, befor);
    XX(g_int_list_value_config, int_list, befor);
    XX(g_int_set_value_config, int_set, befor);
    XX(g_int_uset_value_config, int_uset, befor);
    XX_M(g_str_int_map_value_config, str_int_map, befor);
    XX_M(g_str_int_umap_value_config, str_int_umap, befor);
    
    YAML::Node root = YAML::LoadFile("/root/sylar/bin/conf/test.yaml");
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after: " << g_float_value_config->toString();
    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_M(g_str_int_map_value_config, str_int_map, after);
    XX_M(g_str_int_umap_value_config, str_int_umap, after);
#undef XX
}



class Person {
public:
    Person() {}
    std::string m_name = "nullptr";
    int m_age = 0;
    bool m_sex = false;

    std::string toString() const {
        std::stringstream ss;
        ss << "{Person name = " << m_name
           << " age = " << m_age
           << " sex = " << m_sex << "}";
        return ss.str();
    }

    bool operator==(const Person& opt) const {
        return m_name == opt.m_name
            && m_age == opt.m_age
            && m_sex == opt.m_sex;
    }
};

namespace sylar{
template<>   // string to Person
class LexicalCast<std::string, Person>{
public:
    Person operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        Person person;
        std::stringstream ss;
        person.m_name = node["name"].as<std::string>();
        person.m_age = node["age"].as<int>();
        person.m_sex = node["sex"].as<bool>();
        return person;
    }
};

template<>   // Person to string
class LexicalCast<Person, std::string>{
public:
    std::string operator() (const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}

sylar::ConfigVar<Person>::ptr g_person_value = 
    sylar::Config::Lookup("class.person", Person(), "system person");

sylar::ConfigVar<std::map<std::string,Person> >::ptr g_person_map = 
    sylar::Config::Lookup("class.map", std::map<std::string,Person>{}, "system person");

sylar::ConfigVar<std::map<std::string, std::vector<Person>> >::ptr g_person_map_vec = 
    sylar::Config::Lookup("class.map_vec", std::map<std::string, std::vector<Person>>{}, "system person");

void test_class() {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "g_person_value befor: " << g_person_value->getValue().toString();

#define XX_PM(g_val, prefix) \
{ \
    auto v = g_val->getValue(); \
    for(auto& i : v) { \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << prefix << ": " << i.first << " - " \
                    << i.second.toString();\
    } \
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << prefix << ": size: " << v.size();\
}

    g_person_value->addListener([] (const Person& old_val, const Person& new_val){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << " old value: " << old_val.toString()
                    << " new value: " << new_val.toString();
    });
    XX_PM(g_person_map, "class.map before");
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before: " << g_person_map_vec->toString();

    YAML::Node file = YAML::LoadFile("/root/sylar/bin/conf/test.yaml");
    sylar::Config::LoadFromYaml(file);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "g_person_value after: " << g_person_value->getValue().toString();
    
    XX_PM(g_person_map, "class.map after");

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after: " << g_person_map_vec->toString();
#undef XX
}
#endif
static sylar::Logger::ptr g_logger_root = SYLAR_LOG_ROOT();
static sylar::Logger::ptr g_logger_sys = SYLAR_LOG_NAME("system");

void test_conf_log() {
    std::cout << sylar::LoggerMgr::GetIstance()->toYamlString() << std::endl << std::endl;
    SYLAR_LOG_INFO(g_logger_sys) << " system-------";
    YAML::Node file = YAML::LoadFile("/root/sylar/bin/conf/test_log_config.yaml");
    sylar::Config::LoadFromYaml(file);
    std::cout << "+++++++++++++++++++++++++++++" << std::endl << std::endl << std::endl;
    std::cout << file << std::endl;
    std::cout << "+++++++++++++++++++++++++++++" << std::endl << std::endl << std::endl;
    SYLAR_LOG_INFO(g_logger_sys) << " system -------";
    g_logger_sys->setFormatter("%d [%p] %m%n");
    SYLAR_LOG_INFO(g_logger_sys) << " hello sylar" << std::endl << std::endl;
}

int main(int argc, char** argv) {

    // test_yaml();
    // test_config();

    // test_class();

    test_conf_log();
    
    sylar::Config::Visit([](sylar::ConfigVarBase::ptr var) {
        SYLAR_LOG_INFO(g_logger_root) << "name=" << var->getName()
                << " description=" << var->getDescription()
                << " typename=" << var->getTypeName()
                << " value=" << var->toString();
    });

    return 0;
}