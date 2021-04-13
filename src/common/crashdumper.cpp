/****************************************************************************
 *  @file     crashdumper.cpp
 *  @brief    异常退出dumper
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2021-01-08
 *  Copyright (c) 2021 SmartMore All rights reserved.
 ****************************************************************************/

#include "crashdumper.h"
#include "vtkcommon.h"
#include <QDir>
#include <QtGlobal>

#ifdef Q_OS_WINDOWS
#include <Windows.h>

#include <DbgHelp.h>
#elif defined(Q_OS_LINUX)
#include <sys/resource.h>
#define CORE_SIZE (1024 * 1024 * 500)
#endif

namespace vtk::common
{

#ifdef Q_OS_WINDOWS
LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    const char *path = "dumps";
    QDir dir(".");
    dir.mkpath(path);
    HANDLE hFile =
        CreateFile(L"dumps/smore_vtk.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
    stExceptionParam.ThreadId = GetCurrentThreadId();
    stExceptionParam.ExceptionPointers = pExceptionInfo;
    stExceptionParam.ClientPointers = FALSE;
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &stExceptionParam, NULL, NULL);
    CloseHandle(hFile);
    return EXCEPTION_EXECUTE_HANDLER;
}

void registerCrashDumper()
{
    SetUnhandledExceptionFilter(TopLevelExceptionFilter);
}
#elif defined(Q_OS_LINUX)
void registerCrashDumper()
{
    struct rlimit rlmt;
    if (getrlimit(RLIMIT_CORE, &rlmt) == -1)
    {
        LOGW(common, "failed to enable core dump");
        return;
    }

    rlmt.rlim_cur = (rlim_t)CORE_SIZE;
    rlmt.rlim_max = (rlim_t)CORE_SIZE;

    if (setrlimit(RLIMIT_CORE, &rlmt) == -1)
    {
        LOGW(common, "failed to enable core dump");
        return;
    }

    if (getrlimit(RLIMIT_CORE, &rlmt) == -1)
    {
        LOGW(common, "failed to enable core dump");
        return;
    }
    LOGI(common, "set core dump max size to:%d\n", (int)rlmt.rlim_max);
}
#endif

}  // namespace vtk::common
