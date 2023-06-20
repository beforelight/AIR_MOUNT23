#ifndef CAPTURE_CONTROL_SAFE_LOCK_HPP
#define CAPTURE_CONTROL_SAFE_LOCK_HPP
#include "cmsis_compiler.h"
class safe_lock
{
public:
  safe_lock()
  {
    priMask = __get_PRIMASK();
    __disable_irq();
  }
  ~safe_lock() { __set_PRIMASK(priMask); }
  safe_lock(const safe_lock &) = delete;
  safe_lock(safe_lock &&) = delete;
  safe_lock &operator=(const safe_lock &) = delete;
  safe_lock &operator=(safe_lock &&) = delete;

private:
  uint32_t priMask;
};
#endif // CAPTURE_CONTROL_SAFE_LOCK_HPP
