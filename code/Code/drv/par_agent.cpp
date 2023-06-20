#include "par_agent.hpp"
#include "lfs.h"
extern lfs_t *hlfs;

par_agent &par_agent::get_instance() {
    static par_agent par_agent_onlyone;
    return par_agent_onlyone;
}

/*
 * 保存文件格式的说明（伪代码），每一行的格式是这样的
 * "{}:{}\r\n".format(par.id,par.str)
 * 然后par.str中每个参数用'\t'隔开，方便用于输出到控制台
 * */
extern "C" void get_file_cfg(struct lfs_file_config *);
void par_agent::load() {
    lfs_file_t lff;
    struct lfs_file_config fcfg;
    get_file_cfg(&fcfg);
    std::string id;
    std::string str;
    int state = 0; // 状态机，0读取id,1读取str
    int len = 0;   // 记录此时的字符宽度，太长直接pass
    int count = 0;
    constexpr int len_limit = 4 * 1024;
    for (const auto &file: getTypeList()) {
//        if (0 == lfs_file_open(hlfs, &lff, file.c_str(), LFS_O_RDWR)) {
        if (0 == lfs_file_opencfg(hlfs, &lff, file.c_str(), LFS_O_RDWR, &fcfg)) {
            // 一字节一字节的读取
            char buf;
            while (lfs_file_read(hlfs, &lff, &buf, 1) > 0) {
                if (buf == '\n') {
                    continue;
                }
                if (state == 0) {
                    if (buf != ':') {
                        id += buf;
                        len++;
                    } else {
                        state = 1;
                        len = 0;
                    }
                } else {
                    if (buf != '\r') {
                        str += buf;
                        len++;
                    } else {
                        // 处理完一个，进行
                        state = 0;
                        len = 0;
                        set_str(file, id, str);
                        id.clear();
                        str.clear();
                        count++;
                    }
                }
                if (len > len_limit) {
                    printf("读取字符太长，文件%s可能损坏\r\n", file.c_str());
                    lfs_file_close(hlfs, &lff);
                    return;
                }
            }
            lfs_file_close(hlfs, &lff);
        } else {
            printf("打开文件%s失败\r\n", file.c_str());
        }
    }
    printf("读取完成，总计%d个参数\r\n", count);
}
extern "C" void get_file_cfg(struct lfs_file_config *);
void par_agent::save() {
    lfs_file_t lff;
    struct lfs_file_config fcfg;
    get_file_cfg(&fcfg);
    int count = 0;
    for (const auto &file: getTypeList()) {
//        if (0 == lfs_file_open(hlfs, &lff, file.c_str(), LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC)) {
        if (0 == lfs_file_opencfg(hlfs, &lff, file.c_str(), LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC, &fcfg)) {
            for (auto &i: findByType(file)) {
                lfs_ssize_t state = 1;
                state = std::min(state, lfs_file_write(hlfs, &lff, i->m_id.c_str(), i->m_id.length()));
                state = std::min(state, lfs_file_write(hlfs, &lff, ":", 1));
                auto str = i->str();
                state = std::min(state, lfs_file_write(hlfs, &lff, str.c_str(), str.length()));
                state = std::min(state, lfs_file_write(hlfs, &lff, "\r\n", 2));
                if (state < 0) {
                    printf("文件%s写入失败，错误码%d\r\n", file.c_str(), int(state));
                    lfs_file_close(hlfs, &lff);
                    return;
                }
                count++;
            }
            lfs_file_close(hlfs, &lff);
        } else {
            printf("打开文件%s失败\r\n", file.c_str());
        }
    }
    printf("写入完成，总计%d个参数\r\n", count);
}

void par_agent::set_str(const std::string &id, const std::string &str) {
    for (auto &i: findById(id)) {
        i->scan(str);
    }
}

void par_agent::set_str(const std::string &type, const std::string &id, const std::string &str) {
    for (auto &i: findByType(type)) {
        if (i->m_id == id)
            i->scan(str);
    }
}

std::vector<par_agent::par *> par_agent::find(const std::string &id, const std::string &type) {
    std::vector<par_agent::par *> rtv;
    for (auto &i: m_pars) {
        if (i->m_id == id && i->m_type == type)
            rtv.push_back(i);
    }
    return rtv;
}
std::vector<par_agent::par *> par_agent::findById(const std::string &id) {
    std::vector<par_agent::par *> rtv;
    for (auto &i: m_pars) {
        if (i->m_id == id)
            rtv.push_back(i);
    }
    return rtv;
}
std::vector<par_agent::par *> par_agent::findByType(const std::string &type) {
    std::vector<par_agent::par *> rtv;
    for (auto &i: m_pars) {
        if (i->m_type == type)
            rtv.push_back(i);
    }
    return rtv;
}

template<class T>
bool notIn(const std::vector<T> &list, const T &item) {
    for (const auto &i: list) {
        if (i == item)
            return false;
    }
    return true;
}

std::vector<std::string> par_agent::getTypeList() {
    std::vector<std::string> rtv;
    for (auto &i: m_pars) {
        if (notIn(rtv, i->m_type))
            rtv.push_back(i->m_type);
    }
    return rtv;
}
void Stringsplit(const std::string &str, char split, std::vector<std::string> &res) {
    if (str.empty()) return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + split;
    size_t pos = strs.find(split);

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != std::string::npos) {
        std::string temp = strs.substr(0, pos);
        if (!temp.empty() && temp[0] != split)
            res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + 1, strs.size());
        pos = strs.find(split);
    }
}
void Stringsplit(const std::string &str, const std::string &splits, std::vector<std::string> &res) {
    if (str.empty()) return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + splits;
    size_t pos = strs.find(splits);
    auto step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != std::string::npos) {
        std::string temp = strs.substr(0, pos);
        if (!temp.empty() && temp != splits)
            res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}
