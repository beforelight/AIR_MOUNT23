#include "section.h"
#include "up_data_format.hpp"
up_data_format up_data_format_buff_buff[64];
BSS_CCMRAM triPtrFifo<up_data_format, 64> updata_fifo(up_data_format_buff_buff);