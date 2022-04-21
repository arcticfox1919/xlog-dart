#include <vector>
#include <string>
#include "mars/log/appender.h"
#include "mars/log/xlogger_interface.h"
#include "mars/comm/xlogger/xlogger.h"
#include "c_xlog.h"


#define LONGTHREADID2INT(a) ((a >> 32)^((a & 0xFFFF)))

// #define CHECK_LOG_INSTANCE(log_instance) \
//     if (log_instance < 0) return

extern "C" {

int64_t Dart_newXlogInstance(const LogConfig *log_config) {
    if (NULL == log_config) {
        xerror2(TSF"logconfig is null");
        return -1;
    }

    int level = log_config->level;
    int mode = log_config->mode;
    char * logdir = log_config->log_dir;
    char * nameprefix = log_config->name_prefix;
    char * pubkey = log_config->pubkey;
    int compressmode = log_config->compress_mode;
    int compresslevel = log_config->compress_level;
    char * cachedir = log_config->cache_dir;
    int cachedays = log_config->cache_days;

    std::string cachedir_str;
    if (NULL != cachedir) {
        cachedir_str = cachedir;
    }

    std::string pubkey_str;
    if (NULL != pubkey) {
        pubkey_str = pubkey;
    }

    std::string logdir_str;
    if (NULL != logdir) {
        logdir_str = logdir;
    }

    std::string nameprefix_str;
    if (NULL != nameprefix) {
        nameprefix_str = nameprefix;
    }

    mars::xlog::XLogConfig config = {(mars::xlog::TAppenderMode)mode, logdir_str, nameprefix_str, pubkey_str,
                                     (mars::xlog::TCompressMode)compressmode, compresslevel, cachedir_str, cachedays};
    mars::comm::XloggerCategory* category = mars::xlog::NewXloggerInstance(config, (TLogLevel)level);
    if (nullptr == category) {
        return -1;
    }
    return reinterpret_cast<uintptr_t>(category);
}

int64_t Dart_getXlogInstance(const char *_nameprefix) {
    mars::comm::XloggerCategory* category = mars::xlog::GetXloggerInstance(_nameprefix);
    if (nullptr == category) {
        return -1;
    }
    return reinterpret_cast<uintptr_t>(category);
}

void Dart_releaseXlogInstance(const char *_nameprefix) {
    mars::xlog::ReleaseXloggerInstance(_nameprefix);
}

void Dart_appenderOpen(const LogConfig *log_config) {
    if (NULL == log_config) {
        xerror2(TSF"logconfig is null");
        return;
    }

    int level = log_config->level;
    int mode = log_config->mode;
    char * logdir = log_config->log_dir;
    char * nameprefix = log_config->name_prefix;
    char * pubkey = log_config->pubkey;
    int compressmode = log_config->compress_mode;
    int compresslevel = log_config->compress_level;
    char * cachedir = log_config->cache_dir;
    int cachedays = log_config->cache_days;

    std::string cachedir_str;
    if (NULL != cachedir) {
        cachedir_str = cachedir;
    }

    std::string pubkey_str;
    if (NULL != pubkey) {
        pubkey_str = pubkey;
    }

    std::string logdir_str;
    if (NULL != logdir) {
        logdir_str = logdir;
    }

    std::string nameprefix_str;
    if (NULL != nameprefix) {
        nameprefix_str = nameprefix;
    }

    mars::xlog::XLogConfig config = {(mars::xlog::TAppenderMode)mode, logdir_str, nameprefix_str, pubkey_str,
                                     (mars::xlog::TCompressMode)compressmode, compresslevel, cachedir_str, cachedays};
    appender_open(config);
	xlogger_SetLevel((TLogLevel)level);
}

void Dart_appenderClose(void) {
    mars::xlog::appender_close();
}

void Dart_appenderFlush(int64_t _log_instance_ptr, bool _is_sync) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    mars::xlog::Flush(_log_instance_ptr, _is_sync);
}

void Dart_logWrite(const LogInfo *log_info, char *_log) {
    if (NULL == log_info || NULL == _log) {
        xerror2(TSF"loginfo or log is null");
        return;
    }

    int32_t level = log_info->level;

    if (!xlogger_IsEnabledFor((TLogLevel)level)) {
        return;
    }

    char* tag = log_info->tag;
    char* filename = log_info->filename;
    char* funcname = log_info->funcname;
    int32_t line = log_info->line;
    int64_t pid = log_info->pid;
    int64_t tid = log_info->tid;
    int64_t maintid = log_info->main_tid;

    XLoggerInfo xlog_info;
    gettimeofday(&xlog_info.timeval, NULL);
    xlog_info.level = (TLogLevel)level;
    xlog_info.line = line;
    xlog_info.pid = (int)pid;
    xlog_info.tid = LONGTHREADID2INT(tid);
    xlog_info.maintid = LONGTHREADID2INT(maintid);;
    xlog_info.tag = tag;
    xlog_info.filename = filename;
    xlog_info.func_name = funcname;

    xlogger_Write(&xlog_info, _log);
}

void Dart_logWrite2(int64_t _log_instance_ptr, int32_t _level, char* _tag, char* _filename,
          char* _funcname, int32_t _line, int32_t _pid, int64_t _tid, int64_t _maintid, char* _log) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    if (!mars::xlog::IsEnabledFor(_log_instance_ptr, (TLogLevel)_level)) {
        return;
    }

    XLoggerInfo xlog_info;
    gettimeofday(&xlog_info.timeval, NULL);
    xlog_info.level = (TLogLevel)_level;
    xlog_info.line = (int)_line;
    xlog_info.pid = (int)_pid;
    xlog_info.tid = LONGTHREADID2INT(_tid);
    xlog_info.maintid = LONGTHREADID2INT(_maintid);

    const char* tag_cstr = NULL;
    const char* filename_cstr = NULL;
    const char* funcname_cstr = NULL;
    const char* log_cstr = NULL;

    if (NULL != _tag) {
        tag_cstr = _tag;
    }

    if (NULL != _filename) {
        filename_cstr = _filename;
    }

    if (NULL != _funcname) {
        funcname_cstr = _funcname;
    }

    if (NULL != _log) {
        log_cstr = _log;
    }

    xlog_info.tag = NULL == tag_cstr ? "" : tag_cstr;
    xlog_info.filename = NULL == filename_cstr ? "" : filename_cstr;
    xlog_info.func_name = NULL == funcname_cstr ? "" : funcname_cstr;

    mars::xlog::XloggerWrite(_log_instance_ptr, &xlog_info, NULL == log_cstr ? "NULL == log" : log_cstr);
}

int32_t Dart_getLogLevel(int64_t _log_instance_ptr) {
    if (_log_instance_ptr < 0) return kLevelNone;
    return mars::xlog::GetLevel(_log_instance_ptr);
}

void Dart_setLogLevel(int64_t _log_instance_ptr, int32_t _log_level) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    mars::xlog::SetLevel(_log_instance_ptr, (TLogLevel)_log_level);
}

void Dart_setAppenderMode(int64_t _log_instance_ptr, int32_t _mode) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    mars::xlog::SetAppenderMode(_log_instance_ptr, (mars::xlog::TAppenderMode)_mode);
}

void Dart_setConsoleLogOpen(int64_t _log_instance_ptr, bool _is_open) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    mars::xlog::SetConsoleLogOpen(_log_instance_ptr, _is_open);
}

void Dart_setMaxFileSize(int64_t _log_instance_ptr, int64_t _max_size) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    mars::xlog::SetMaxFileSize(_log_instance_ptr, _max_size);
}

void Dart_setMaxAliveTime(int64_t _log_instance_ptr, int64_t _max_time) {
    // CHECK_LOG_INSTANCE(_log_instance_ptr);
    mars::xlog::SetMaxAliveTime(_log_instance_ptr, _max_time);
}
}

void ExportXlog() {}
