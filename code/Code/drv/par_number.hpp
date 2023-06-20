#ifndef AVIS_RT_PAR_NUMBER_H
#define AVIS_RT_PAR_NUMBER_H
#include "par_agent.hpp"

namespace par_number {
    //用于在，加载一些初始化系统的特殊参数，比如控制频率，
    template<class T>
    class number : public par_agent::par {
    public:
        T val = 0;
    public:
        explicit number(T val_, const char *id) : val(val_), par_agent::par("number", id) {}
        std::string str() override {
            return my::to_string(val);
        }
        void scan(const std::string &str) override {
            if constexpr (std::is_same_v<T, float>)
                val = std::stof(str);
            else if constexpr (std::is_same_v<T, uint32_t>)
                val = std::stoul(str);
            else if constexpr (std::is_same_v<T, int32_t>)
                val = std::stol(str);
        }
    };
}

#endif //AVIS_RT_PAR_NUMBER_H
