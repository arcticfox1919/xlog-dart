#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t level;
    int32_t mode;
    int32_t compress_mode;
    int32_t compress_level;
    int32_t cache_days;
    char *log_dir;
    char *name_prefix;
    char *pubkey;
    char *cache_dir;
} LogConfig;

typedef struct {
    int32_t level;
    int32_t line;
    int64_t pid;
    int64_t tid;
    int64_t main_tid;
    char* tag;
    char* filename;
    char *funcname;
} LogInfo;

int64_t Dart_newXlogInstance(const LogConfig *log_config);

int64_t Dart_getXlogInstance(const char *_nameprefix);

void Dart_releaseXlogInstance(const char *_nameprefix);

void Dart_appenderOpen(const LogConfig *log_config);

void Dart_appenderClose(void);

void Dart_appenderFlush(int64_t _log_instance_ptr, bool _is_sync);

void Dart_logWrite(const LogInfo *log_info, char *_log);

void Dart_logWrite2(int64_t _log_instance_ptr, int32_t _level, char* _tag, char* _filename,
          char* _funcname, int32_t _line, int32_t _pid, int64_t _tid, int64_t _maintid, char* _log);

int32_t Dart_getLogLevel(int64_t _log_instance_ptr);

void Dart_setLogLevel(int64_t _log_instance_ptr, int32_t _log_level);

void Dart_setAppenderMode(int64_t _log_instance_ptr, int32_t _mode);

void Dart_setConsoleLogOpen(int64_t _log_instance_ptr, bool _is_open);

void Dart_setMaxFileSize(int64_t _log_instance_ptr, int64_t _max_size);

void Dart_setMaxAliveTime(int64_t _log_instance_ptr, int64_t _max_time);

#ifdef __cplusplus
}
#endif