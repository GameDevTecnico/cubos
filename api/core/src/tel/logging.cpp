#include <cubos/api/core/tel/logging.h>

#include <cubos/core/tel/logging.hpp>
#include <cubos/core/reflection/external/cstring.hpp>

#include <cstdlib>

void cubos_tel_logging_info(char* str)
{
    CUBOS_INFO("{}", str);
    free(str);
}

void cubos_tel_logging_warn(char* str)
{
    CUBOS_WARN("{}", str);
    free(str);
}

void cubos_tel_logging_error(char* str)
{
    CUBOS_ERROR("{}", str);
    free(str);
}

void cubos_tel_logging_critical(char* str)
{
    CUBOS_CRITICAL("{}", str);
    free(str);
}
