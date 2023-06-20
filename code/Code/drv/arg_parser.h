#ifndef CAPTURE_CONTROL_ARG_PARSER_H
#define CAPTURE_CONTROL_ARG_PARSER_H
#include <stdarg.h>
#include <string>
#include <vector>
namespace arg
{
// 针对letter-shell的简单 辅助解析参数的类。
class parser
{
public:
  parser(int _argn, const char **_argv) : argn(_argn), argv(_argv)
  {
    for (int i = 0; i < argn; ++i) {
      args.emplace_back(argv[i]);
    }
    args.emplace_back("");
  }

  [[nodiscard]] inline int size() const { return argn; }

  // 是否还有参数(内部计数器)
  explicit operator bool() const { return idx < argn; }
  // 指向下一个参数(内部计数器)
  inline void next() { idx++; }
  // 当前参数(内部计数器)
  const std::string &current() { return operator bool() ? args[idx] : args[argn]; }
  // 当前参数(内部计数器)转换到数字
  template <class Tp>
  Tp toNum()
  {
    static_assert(std::is_same<Tp, int>::value || std::is_same<Tp, float>::value, "仅支持部分类型");
    if constexpr (std::is_same<Tp, int>::value)
      return stoi(current());
    else if constexpr (std::is_same<Tp, float>::value)
      return stof(current());
  }

  // 安全访问，第x个参数
  const std::string &operator[](int i) const { return i < argn ? args[i] : args[argn]; }

private:
  int idx = 0;
  int argn;
  const char **argv;
  std::vector<std::string> args;
};

// 辅助解析程序退出
class exit
{
public:
  ~exit()
  {
    for (auto &i : msgs) {
      printf("%s", i.c_str());
    }
  }

  // 自动加\r\n
  void append(const char *formatString, ...)
  {
    char printBuf[256];
    va_list arg;
    int logLength;
    va_start(arg, formatString);
    logLength = vsnprintf(printBuf, sizeof(printBuf) - 3, formatString, arg);
    va_end(arg);
    printBuf[logLength] = '\r';
    logLength++;
    printBuf[logLength] = '\n';
    logLength++;
    printBuf[logLength] = 0;
    msgs.emplace_back(printBuf);
  }
private:
  std::vector<std::string> msgs;
};
} // namespace arg

#endif // CAPTURE_CONTROL_ARG_PARSER_H
