#include <cubos/api/core/api.h>

#ifdef __cplusplus
extern "C" {
#endif

CUBOS_CORE_C_API void cubos_tel_logging_info(char* str);
CUBOS_CORE_C_API void cubos_tel_logging_warn(char* str);
CUBOS_CORE_C_API void cubos_tel_logging_error(char* str);
CUBOS_CORE_C_API void cubos_tel_logging_critical(char* str);

#ifdef __cplusplus
}
#endif
