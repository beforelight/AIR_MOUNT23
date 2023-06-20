#ifndef CAPTURE_CONTROL_PAR_AGENT_HPP
#define CAPTURE_CONTROL_PAR_AGENT_HPP
#include <string>
#include <utility>
#include <vector>
#include "to_string.hpp"
// 参数代理，当然也是单例对象
class par_agent {
private:
    par_agent() = default;

public:
    // 需要保存的参数的虚基类
    class par {
    public:
        par(std::string type, std::string id) : m_type(std::move(type)), m_id(std::move(id)) {}
        virtual ~par() = default;

        // 内部参数怎么转换到字符串
        virtual std::string str() = 0;
        // 内部参数怎么从字符串读取
        virtual void scan(const std::string &str) = 0;

    public:
        std::string m_type;
        std::string m_id;
    };

public:
    // 注册需要保存的参数
    void reg(par &p) { m_pars.push_back(&p); }
    void reg(par *p) { m_pars.push_back(p); }
    // 从文件读取参数
    void load();
    // 保存参数到文件
    void save();
    // 返回单例
    static par_agent &get_instance();

    std::vector<par *> find(const std::string &id, const std::string &type);
    std::vector<par *> findById(const std::string &id);
    std::vector<par *> findByType(const std::string &type);
    std::vector<std::string> getTypeList();

private:
    void set_str(const std::string &id, const std::string &str);
    void set_str(const std::string &type, const std::string &id, const std::string &str);

public:
    std::vector<par *> m_pars; // 存储序号
};


void Stringsplit(const std::string &str, char split, std::vector<std::string> &res);
void Stringsplit(const std::string &str, const std::string &splits, std::vector<std::string> &res);

#endif // CAPTURE_CONTROL_PAR_AGENT_HPP
