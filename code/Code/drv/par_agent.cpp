#include "par_agent.hpp"
#include "lfs_port.h"

par_agent &par_agent::get_instance()
{
  static par_agent par_agent_onlyone;
  return par_agent_onlyone;
}

/*
 * 保存文件格式的说明（伪代码），每一行的格式是这样的
 * "{}:{}\r\n".format(par.id,par.str)
 * */

void par_agent::load(const std::string &file)
{
  lfs_file_t lff;
  std::string id;
  std::string str;
  int state = 0; // 状态机，0读取id,1读取str
  int len = 0;   // 记录此时的字符宽度，太长直接pass
  int count = 0;
  constexpr int len_limit = 1024;
  if (0 == lfs_file_open(hlfs, &lff, file.c_str(), LFS_O_RDWR)) {
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
          set_str(id, str);
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
    printf("读取完成，总计%d个参数\r\n", count);
  } else {
    printf("打开文件%s失败\r\n", file.c_str());
  }
}

void par_agent::save(const std::string &file)
{
  lfs_file_t lff;
  int count = 0;
  if (0 == lfs_file_open(hlfs, &lff, file.c_str(), LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC)) {
    for (auto &i : m_pars) {
      lfs_ssize_t state = 1;
      state = std::min(state, lfs_file_write(hlfs, &lff, i->m_id.c_str(), i->m_id.length()));
      state = std::min(state, lfs_file_write(hlfs, &lff, ":", 1));
      auto str = i->str();
      state = std::min(state, lfs_file_write(hlfs, &lff, str.c_str(), str.length()));
      state = std::min(state, lfs_file_write(hlfs, &lff, "\r\n", 2));
      if (state < 0) {
        printf("文件%s写入失败，错误码%d", file.c_str(), int(state));
        lfs_file_close(hlfs, &lff);
        return;
      }
    }
    lfs_file_close(hlfs, &lff);
    printf("写入完成，总计%d个参数\r\n", count);
  } else {
    printf("打开文件%s失败\r\n", file.c_str());
  }
}

void par_agent::set_str(const std::string &id, const std::string &str)
{
  for (auto &i : findById(id)) {
    i->scan(str);
  }
}
std::vector<par_agent::par *> par_agent::find(const std::string &id, const std::string &type)
{
  std::vector<par_agent::par *> rtv;
  for (auto &i : m_pars) {
    if (i->m_id == id && i->m_type == type)
      rtv.push_back(i);
  }
  return rtv;
}
std::vector<par_agent::par *> par_agent::findById(const std::string &id)
{
  std::vector<par_agent::par *> rtv;
  for (auto &i : m_pars) {
    if (i->m_id == id)
      rtv.push_back(i);
  }
  return rtv;
}
std::vector<par_agent::par *> par_agent::findByType(const std::string &type)
{
  std::vector<par_agent::par *> rtv;
  for (auto &i : m_pars) {
    if (i->m_type == type)
      rtv.push_back(i);
  }
  return rtv;
}
