/**
 * @file pump_core_file.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of file control interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "pump_core/pump_core_handle.h"
#if (defined PUMP_OS_WINDOWS)
#   if !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
#include <Aclapi.h>
#include <direct.h>
#include <io.h>
#   else
#define FILE_ATTRIBUTE_DEVICE 0x00000040
#   endif // !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
#elif (defined PUMP_OS_POSIX)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#   ifdef PUMP_OS_APPLE
#include <mach-o/dyld.h>
#include <sys/param.h>
#   endif
#endif // PUMP_OS_WINDOWS
#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/pump_core_api.h"
#include "pump_core/time/pump_core_time.h"
#include "pump_core/time/__pump_core_time_inner.h"
#include "pump_core/file/pump_core_file.h"

typedef struct tagPUMP_DIR_INFO
{
#if (defined PUMP_OS_WINDOWS)
    pump_handle_t hFindHandle;
    char   *szDirName;
#   if !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    WIN32_FIND_DATAA struFindData;
#   else
    WIN32_FIND_DATAW struFindData;
#   endif // !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    bool   bFirst;
#elif (defined PUMP_OS_POSIX)
    DIR  *pDirStru;
    char *szDirName;
    struct dirent struEntry;
#endif // (defined PUMP_OS_WINDOWS)
} PUMP_DIR_INFO;

#if (defined PUMP_OS_WINDOWS)
/**
 * @brief convert 2 DWORD type to INT64 type
 * @param dwLow 
 * @param dwHigh
 * @param iDst
 */
inline void PUMP_CORE_Inner_2DWORD2INT64(DWORD dwLow, DWORD dwHigh, INT64* iDst)
{
    *iDst = (Int64ShllMod32(dwHigh, 32) | dwLow);
}
#endif // (defined PUMP_OS_WINDOWS)

/**
 * @brief release PUMP_DIR_INFO*
 * @param (IN) PUMP_DIR_INFO* szDirInfo pointer need release
 * @return none
 */
void PUMP_CORE_Inner_FreeDirInfo(PUMP_DIR_INFO* szDirInfo)
{
    if (szDirInfo != PUMP_NULL)
    {
        if (szDirInfo->szDirName != PUMP_NULL)
        {
            free(szDirInfo->szDirName);
        }
        free(szDirInfo);
    }
}

/**
 * @brief allocate PUMP_DIR_INFO*
 * @param (IN) int iDirNameLen length of dir path
 * @return succ-memory pointer, fail-PUMP_NULL
 */
PUMP_DIR_INFO *PUMP_CORE_Inner_AllocDirInfo(int iDirNameLen)
{
    PUMP_DIR_INFO *szDirInfo = (PUMP_DIR_INFO *)::malloc(sizeof(PUMP_DIR_INFO));
    if (PUMP_NULL == szDirInfo)
    {
        goto ret_alloc;
    }
    ::memset(szDirInfo, 0, sizeof(*szDirInfo));

    szDirInfo->szDirName = (char *)::malloc(iDirNameLen + 1);
    if (PUMP_NULL == szDirInfo->szDirName)
    {
        goto ret_alloc;
    }
    ::memset(szDirInfo->szDirName, 0, iDirNameLen + 1);

    return szDirInfo;

ret_alloc:
    PUMP_CORE_Inner_FreeDirInfo(szDirInfo);
    return PUMP_NULL;
}

#if (defined PUMP_OS_POSIX)
int PUMP_CORE_GetFileTypeByMode(mode_t mode)
{
    if (S_ISREG(mode))
    {
        return PUMP_TYPE_REG;
    }
    else if (S_ISDIR(mode))
    {
        return PUMP_TYPE_DIR;
    }
    else if (S_ISCHR(mode))
    {
        return PUMP_TYPE_CHAR;
    }
    else if (S_ISBLK(mode))
    {
        return PUMP_TYPE_BLK;
    }
    else if (S_ISFIFO(mode))
    {
        return PUMP_TYPE_FIFO;
    }
    else if (S_ISLNK(mode))
    {
        return PUMP_TYPE_LINK;
    }
    else if (S_ISSOCK(mode))
    {
        return PUMP_TYPE_SOCK;
    }
    else
    {
        return PUMP_TYPE_UNKNOWN;
    }
}

pump_uint32_t PUMP_CORE_GetPermByMode(mode_t mode)
{
    pump_uint32_t perms = 0;

    if (mode & S_ISUID)
    {
        perms |= PUMP_USETID;
    }
    if (mode & S_IRUSR)
    {
        perms |= PUMP_UREAD;
    }

    if (mode & S_IWUSR)
    {
        perms |= PUMP_UWRITE;
    }
    if (mode & S_IXUSR)
    {
        perms |= PUMP_UEXECUTE;
    }

    if (mode & S_ISGID)
    {
        perms |= PUMP_GSETID;
    }
    if (mode & S_IRGRP)
    {
        perms |= PUMP_GREAD;
    }
    if (mode & S_IWGRP)
    {
        perms |= PUMP_GWRITE;
    }
    if (mode & S_IXGRP)
    {
        perms |= PUMP_GEXECUTE;
    }

    if (mode & S_IROTH)
    {
        perms |= PUMP_WREAD;
    }
    if (mode & S_IWOTH)
    {
        perms |= PUMP_WWRITE;
    }
    if (mode & S_IXOTH)
    {
        perms |= PUMP_WEXECUTE;
    }

    return perms;
}
#endif // (defined PUMP_OS_POSIX)

/**
* @brief Get file info from WIN32_FIND_DATA
* @param (IN) WIN32_FIND_DATA *pFindData
* @param (OUT) FILE_FIND_INFO *pFileInfo
*/
#if (defined PUMP_OS_WINDOWS)
#   if !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
void PUMP_CORE_Inner_FillFileInfo(WIN32_FIND_DATAA *pFindData, PUMP_FILEFIND_INFO *pFileInfo)
{
    ZeroMemory(pFileInfo, sizeof(*pFileInfo));
    ::memcpy(pFileInfo->sFileName, pFindData->cFileName, sizeof(pFileInfo->sFileName));
#   else
void PUMP_CORE_Inner_FillFileInfo(WIN32_FIND_DATAW *pFindData, PUMP_FILEFIND_INFO *pFileInfo)
{
    ZeroMemory(pFileInfo, sizeof(*pFileInfo));
    WideCharToMultiByte(CP_ACP, 0, pFindData->cFileName, ::wcslen(pFindData->cFileName), pFileInfo->sFileName, sizeof(pFileInfo->sFileName), PUMP_NULL, PUMP_NULL);
#   endif  // !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    //File type
    if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
    {
        pFileInfo->FileInfo.nFileType = PUMP_TYPE_LINK;
    }
    else if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        pFileInfo->FileInfo.nFileType = PUMP_TYPE_DIR;
    }
    else if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
    {
        pFileInfo->FileInfo.nFileType = PUMP_TYPE_CHAR;
    }
    else
    {
        pFileInfo->FileInfo.nFileType = PUMP_TYPE_REG;
    }

    //Access
    if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    {
        pFileInfo->FileInfo.nProtection |= PUMP_ATTR_READONLY;
    }
    else
    {
        pFileInfo->FileInfo.nProtection |= (PUMP_ATTR_WRITE | PUMP_ATTR_EXECUTABLE);
    }

    if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
    {
        pFileInfo->FileInfo.nProtection |= PUMP_ATTR_HIDDEN;
    }

    //Size
    PUMP_CORE_Inner_2DWORD2INT64(pFindData->nFileSizeLow, pFindData->nFileSizeHigh, &pFileInfo->FileInfo.nSize);

    //Time
    PUMP_CORE_Inner_FileTimeToTime(&(pFindData->ftCreationTime), &pFileInfo->FileInfo.iCreateTime);
    PUMP_CORE_Inner_FileTimeToTime(&pFindData->ftLastAccessTime, &pFileInfo->FileInfo.iAccessTime);
    PUMP_CORE_Inner_FileTimeToTime(&pFindData->ftLastWriteTime, &pFileInfo->FileInfo.iWriteTime);
}

void PUMP_CORE_Inner_Convert2DOWRDTOINT64(DWORD dwLow, DWORD dwHigh, INT64* iDst)
{
    *iDst = (Int64ShllMod32(dwHigh,32) | dwLow);
}
#elif (defined PUMP_OS_POSIX)
void PUMP_CORE_Inner_FillFileInfo(char *pDirName, struct dirent *pEntry, PUMP_FILEFIND_INFO *pFileInfo)
{
    memset(pFileInfo, 0, sizeof(*pFileInfo));
    memcpy(pFileInfo->sFileName, pEntry->d_name, strlen(pEntry->d_name));

    // file path
    int iDirLen = strlen(pDirName);
    int iNameLen = strlen(pEntry->d_name);
    char *pFilePath = (char*)malloc(iDirLen + iNameLen + 1);
    if (NULL == pFilePath)
    {
        return;
    }

    memcpy(pFilePath, pDirName, iDirLen);
    memcpy(pFilePath + iDirLen, pEntry->d_name, iNameLen);
    pFilePath[iDirLen + iNameLen] = '\0';

    // Get file property
    struct stat st;
    int iRet = stat(pFilePath, &st);
    if (iRet)
    {
        free(pFilePath);
        //printf("Get file stat failed! err:%d\n", errno);
        return;
    }

    pFileInfo->FileInfo.nFileType = PUMP_CORE_GetFileTypeByMode(st.st_mode);
    pFileInfo->FileInfo.nProtection = PUMP_CORE_GetPermByMode(st.st_mode);
    pFileInfo->FileInfo.vUID = st.st_uid;
    pFileInfo->FileInfo.vGID = st.st_gid;
    pFileInfo->FileInfo.nSize = st.st_size;
    pFileInfo->FileInfo.iINode = st.st_ino;
    pFileInfo->FileInfo.nHardLink = st.st_nlink;
    pFileInfo->FileInfo.nDeviceID = st.st_rdev;
    pFileInfo->FileInfo.iAccessTime = ((pump_time_t)st.st_atime) * PUMP_USEC_PER_SEC;
    pFileInfo->FileInfo.iCreateTime = ((pump_time_t)st.st_ctime) * PUMP_USEC_PER_SEC;
    pFileInfo->FileInfo.iWriteTime = ((pump_time_t)st.st_mtime) * PUMP_USEC_PER_SEC;

    free(pFilePath);
}

int PUMP_CORE_GetOFlagByFlag(pump_uint32_t nFlag)
{
    int iOFlag = 0;

    if ((nFlag & PUMP_READ) && (nFlag & PUMP_WRITE))
    {
        iOFlag |= O_RDWR;
    }
    else if ((nFlag & PUMP_READ))
    {
        iOFlag |= O_RDONLY;
    }
    else if ((nFlag & PUMP_WRITE))
    {
        iOFlag |= O_WRONLY;
    }
    else
    {
        return -1;
    }

    if (nFlag & PUMP_CREATE)
    {
        iOFlag |= O_CREAT;
        if (nFlag & PUMP_EXCL)
        {
            iOFlag |= O_EXCL;
        }
    }

    //if ( (iOFlag & O_CREAT) && !(iOFlag & O_EXCL) )
    //{
    //	return -1;
    //}

    if (nFlag & PUMP_APPEND)
    {
        iOFlag |= O_APPEND;
    }
    if (nFlag & PUMP_TRUNCATE)
    {
        iOFlag |= O_TRUNC;
    }

#if defined (__linux__)
    if (nFlag & PUMP_DIRECT)
    {
        iOFlag |= O_DIRECT;
    }
#endif

    return iOFlag;
}

mode_t PUMP_CORE_GetModeByPerm(pump_uint32_t perms)
{
    if (perms & PUMP_ATTR_READONLY)
    {
        return S_IRUSR;
    }

    mode_t mode = S_IRUSR;

    if (perms & PUMP_ATTR_WRITE)
    {
        perms &= (~PUMP_ATTR_WRITE);
        mode |= S_IWUSR;
    }
    if (perms & PUMP_ATTR_EXECUTABLE)
    {
        perms &= (~PUMP_ATTR_EXECUTABLE);
        mode |= S_IXUSR;
    }

    if (perms & PUMP_USETID)
    {
        mode |= S_ISUID;
    }
    if (perms & PUMP_UREAD)
    {
        mode |= S_IRUSR;
    }
    if (perms & PUMP_UWRITE)
    {
        mode |= S_IWUSR;
    }
    if (perms & PUMP_UEXECUTE)
    {
        mode |= S_IXUSR;
    }

    if (perms & PUMP_GSETID)
    {
        mode |= S_ISGID;
    }
    if (perms & PUMP_GREAD)
    {
        mode |= S_IRGRP;
    }
    if (perms & PUMP_GWRITE)
    {
        mode |= S_IWGRP;
    }
    if (perms & PUMP_GEXECUTE)
    {
        mode |= S_IXGRP;
    }

    if (perms & PUMP_WREAD)
    {
        mode |= S_IROTH;
    }
    if (perms & PUMP_WWRITE)
    {
        mode |= S_IWOTH;
    }
    if (perms & PUMP_WEXECUTE)
    {
        mode |= S_IXOTH;
    }

    return mode;
}
#endif // (defined PUMP_OS_WINDOWS)

/**
* @brief To determine whether there is directory.
* @param (IN) WIN32_FIND_DATA *pFindData
* @param (OUT) FILE_FIND_INFO *pFileInfo
*/
PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_DirIsExist(const char* szDir)
{
    if (!szDir)
    {
        return PUMP_FALSE;
    }
#if (defined PUMP_OS_WINDOWS)
    DWORD dwAttrib = PUMP_CORE_FileAttributes(szDir);
    return (INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#elif (defined PUMP_OS_POSIX)
    return PUMP_FALSE;
#endif // (defined PUMP_OS_WINDOWS)
}

/**
* @brief create directory.
* @param szDir: directory to make.
* @return if success return PUMP_OK, otherwise return PUMP_ERROR.
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirCreate(const char* szDir)
{
    if (!szDir)
    {
        return PUMP_ERROR;
    }
#if (defined PUMP_OS_WINDOWS)
#   if (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)
    wchar_t wDir[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, szDir, ::strlen(szDir), wDir, sizeof(wDir));
    return (::CreateDirectoryW(wDir, PUMP_NULL)) ? PUMP_OK : PUMP_ERROR;
#   else
    return (::CreateDirectory((LPCSTR)szDir, PUMP_NULL)) ? PUMP_OK : PUMP_ERROR;
#   endif // (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)
#elif (defined PUMP_OS_POSIX)
    //permession 1766
    return mkdir(szDir, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#endif // (defined PUMP_OS_WINDOWS)
}

/**
* @brief remove directory.
* @param szDir: remove directory.
* @return if success return PUMP_OK, otherwise return PUMP_ERROR.
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirDelete(const char* szDir)
{
    if (!szDir)
    {
        return PUMP_ERROR;
    }
#if (defined PUMP_OS_WINDOWS)
#   if (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)
    wchar_t wDir[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, szDir, ::strlen(szDir), wDir, sizeof(wDir));
    return (::RemoveDirectoryW(wDir)) ? PUMP_OK : PUMP_ERROR;
#   else
    return (::RemoveDirectory((LPCSTR)szDir)) ? PUMP_OK : PUMP_ERROR;
#   endif // (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)
#elif (defined PUMP_OS_POSIX)
    return remove(szDir);
#endif // (defined PUMP_OS_WINDOWS)
}

/**
*   @brief open dir
*   @param (IN) const char *szDirName dir path
*   @return fail-PUMP_NULL, succ-dir handler
*/
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_DirOpen(const char *szDirName)
{
    if (PUMP_NULL == szDirName)
    {
        return PUMP_INVALID_HANDLE;
    }

#if (defined PUMP_OS_WINDOWS)
    int iDirLen = (int)::strlen(szDirName);
    PUMP_DIR_INFO *pDirInfo = PUMP_CORE_Inner_AllocDirInfo(iDirLen + 3);
    if (PUMP_NULL == pDirInfo)
    {
        return PUMP_INVALID_HANDLE;
    }

    pDirInfo->hFindHandle = INVALID_HANDLE_VALUE;
    pDirInfo->bFirst = false;

    ::memcpy(pDirInfo->szDirName, szDirName, iDirLen);
    if (pDirInfo->szDirName[iDirLen - 1] != '/' && pDirInfo->szDirName[iDirLen - 1] != '\\')
    {
        pDirInfo->szDirName[iDirLen] = '/';
        pDirInfo->szDirName[iDirLen + 1] = '*';
    }
    else
    {
        pDirInfo->szDirName[iDirLen] = '*';
    }

    if (PUMP_CORE_DirFindFile((pump_handle_t)pDirInfo, PUMP_NULL) != PUMP_OK)
    {
        PUMP_CORE_Inner_FreeDirInfo(pDirInfo);
        return PUMP_INVALID_HANDLE;
    }
#elif (defined PUMP_OS_POSIX)
    DIR *pDir = opendir(szDirName);
    if (NULL == pDir)
    {
        return PUMP_INVALID_HANDLE;
    }

    int iDirLen = strlen(szDirName);
    PUMP_DIR_INFO *pDirInfo = PUMP_CORE_Inner_AllocDirInfo(iDirLen + 2);
    if (NULL == pDirInfo)
    {
        return PUMP_INVALID_HANDLE;
    }

    pDirInfo->pDirStru = pDir;
    memcpy(pDirInfo->szDirName, szDirName, iDirLen);
    if (pDirInfo->szDirName[iDirLen - 1] != '/')
    {
        pDirInfo->szDirName[iDirLen] = '/';
    }
#endif // (defined PUMP_OS_WINDOWS)

    return (pump_handle_t)pDirInfo;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirFindFile(pump_handle_t hDir, PUMP_FILEFIND_INFO *pFileInfo)
{
    if (PUMP_INVALID_HANDLE == hDir)
    {
        return PUMP_ERROR;
    }

    PUMP_DIR_INFO *pDirInfo = (PUMP_DIR_INFO *)hDir;
#if (defined PUMP_OS_WINDOWS)
    if (INVALID_HANDLE_VALUE == pDirInfo->hFindHandle)
    {
#   if !defined(PUMP_OS_WINCE) && !defined PUMP_OS_WINPHONE
        HANDLE hFindHandle = ::FindFirstFile(pDirInfo->szDirName, (LPWIN32_FIND_DATAA)&pDirInfo->struFindData);
#   else
        wchar_t dirname[MAX_PATH] = { 0 };
        ::MultiByteToWideChar(CP_ACP, 0, pDirInfo->szDirName, ::strlen(pDirInfo->szDirName), dirname, sizeof(dirname));
#       if (defined PUMP_OS_WINCE)
        HANDLE hFindHandle = ::FindFirstFileW(dirname, &pDirInfo->struFindData);
#       else
        HANDLE hFindHandle = ::FindFirstFileExW(dirname, FindExInfoStandard, &pDirInfo->struFindData\
            , FindExSearchNameMatch, PUMP_NULL, FIND_FIRST_EX_CASE_SENSITIVE);
#       endif // (defined PUMP_OS_WINCE)
#   endif
        if (INVALID_HANDLE_VALUE == hFindHandle)
        {
            return PUMP_ERROR;
        }
        pDirInfo->hFindHandle = hFindHandle;
        pDirInfo->bFirst = true;
        return PUMP_OK;
    }

    // output param check
    if (PUMP_NULL == pFileInfo)
    {
        return PUMP_ERROR;
    }

    //first file was found when called OpenDir
    if (pDirInfo->bFirst)
    {
        PUMP_CORE_Inner_FillFileInfo(&pDirInfo->struFindData, pFileInfo);
        pDirInfo->bFirst = false;
        return PUMP_OK;
    }

    // find next file
#   if !defined(PUMP_OS_WINCE) && !defined PUMP_OS_WINPHONE
    if (!::FindNextFileA(pDirInfo->hFindHandle, &pDirInfo->struFindData))
#   else
    if (!::FindNextFileW(pDirInfo->hFindHandle, &pDirInfo->struFindData))
#   endif
    {
        if (ERROR_NO_MORE_FILES == PUMP_CORE_GetSystemError())
        {
            //OutputDebugString("No more files!\n");
        }
        return PUMP_ERROR;
    }
    PUMP_CORE_Inner_FillFileInfo(&pDirInfo->struFindData, pFileInfo);
#elif (defined PUMP_OS_POSIX)
    struct dirent *pRetent = NULL;
    int iRet = readdir_r(pDirInfo->pDirStru, &pDirInfo->struEntry, &pRetent);
    if (iRet != 0 || NULL == pRetent)
    {
        return PUMP_ERROR;
    }
    PUMP_CORE_Inner_FillFileInfo(pDirInfo->szDirName, &pDirInfo->struEntry, pFileInfo);
#endif // (defined PUMP_OS_WINDOWS)
    return PUMP_OK;
}

/**
* @brief close dir hander
* @param (IN) pump_handle_t hDir dir hander
* @return fail-PUMP_ERROR£¬succ-PUMP_OK
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirClose(pump_handle_t hDir)
{
    if (PUMP_INVALID_HANDLE == hDir)
    {
        return PUMP_ERROR;
    }

    PUMP_DIR_INFO *pDirInfo = (PUMP_DIR_INFO*)hDir;
#if (defined PUMP_OS_WINDOWS)
    FindClose(pDirInfo->hFindHandle);
#elif (defined PUMP_OS_POSIX)
    closedir(pDirInfo->pDirStru);
    pDirInfo->pDirStru = NULL;
#endif // (defined PUMP_OS_WINDOWS)
    PUMP_CORE_Inner_FreeDirInfo(pDirInfo);

    return PUMP_OK;
}

//////////////////////////////////////////////////////////////////////////

/**
* Function:	PUMP_CORE_FileOpen
* Desc:		open file in user asked mode.
* Input:		@param pFileName: file name, abs name | relative name.
@param nFlag: open flag, can be composed by "|" operation
@param nFileAttr: file attr, if file exist, this param will be ignored.
* Output:
* Return:		if success return PUMP_FILE_HANDLE, otherwise return PUMP_FILE_INVALID_HANDLE.
* */
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_FileOpen(const char* pFileName, pump_uint32_t nFlag, pump_uint32_t nFileAttr)
{
#if (defined PUMP_OS_WINDOWS)
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwDesiredAccess = 0;
    DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    DWORD dwCreateFlag = 0;
    DWORD dwFlagAttr = FILE_ATTRIBUTE_NORMAL;

    if (!pFileName)
    {
        return PUMP_INVALID_FILE;
    }

    //get file access right
    if (nFlag & PUMP_READ)
    {
        dwDesiredAccess |= GENERIC_READ;
    }
    if (nFlag & PUMP_WRITE)
    {
        dwDesiredAccess |= GENERIC_WRITE;
    }
    if (nFlag & PUMP_EXCL)
    {
        dwDesiredAccess |= GENERIC_EXECUTE;
    }
    //if (nFlag & PUMP_DIRECT)
    //{
    //	dwCreateFlag |= FILE_FLAG_NO_BUFFERING;
    //}

    ///get file open mode
    if (nFlag & PUMP_CREATE)
    {
        if ((nFlag & PUMP_EXCL))
        {
            dwCreateFlag = CREATE_NEW;
        }
        else if (nFlag & PUMP_TRUNCATE)
        {
            dwCreateFlag = CREATE_ALWAYS;
        }
        else
        {
            dwCreateFlag = OPEN_ALWAYS;
        }
    }
    else if (nFlag & PUMP_TRUNCATE)
    {
        dwCreateFlag = TRUNCATE_EXISTING;
    }
    else
    {
        dwCreateFlag = OPEN_EXISTING;
    }

    if ((nFlag & PUMP_EXCL) && !(nFlag & PUMP_CREATE))
    {
        return PUMP_INVALID_FILE;
    }

    //get file attr
    if (nFileAttr & PUMP_ATTR_READONLY)
    {
        dwFlagAttr |= FILE_ATTRIBUTE_READONLY;
    }
    if (nFileAttr & PUMP_ATTR_HIDDEN)
    {
        dwFlagAttr |= FILE_ATTRIBUTE_HIDDEN;
    }
    if (nFileAttr & PUMP_ATTR_SYSTEM)
    {
        dwFlagAttr |= FILE_ATTRIBUTE_SYSTEM;
    }
    if (nFileAttr & PUMP_DELONCLOSE)
    {
        dwFlagAttr |= FILE_FLAG_DELETE_ON_CLOSE;
    }
    if (nFlag & PUMP_DIRECT)
    {
        dwFlagAttr |= FILE_FLAG_NO_BUFFERING;
    }

#   if (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)
    wchar_t wFileName[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, pFileName, ::strlen(pFileName), wFileName, sizeof(wFileName));
#       if (defined PUMP_OS_WINCE)
    hFile = CreateFile(wFileName, dwDesiredAccess, dwShareMode, PUMP_NULL, \
        dwCreateFlag, dwFlagAttr, 0);
#       else
    CREATEFILE2_EXTENDED_PARAMETERS cep = { 0 };
    cep.dwSize = sizeof(cep);
    cep.dwFileAttributes = dwFlagAttr;
    hFile = CreateFile2(wFileName, dwDesiredAccess, dwShareMode, dwCreateFlag, &cep);
#       endif // (defined PUMP_OS_WINCE)
#   else
    hFile = CreateFile((LPCSTR)pFileName, dwDesiredAccess, dwShareMode, PUMP_NULL, \
        dwCreateFlag, dwFlagAttr, 0);
#   endif // (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)

    if (nFlag & PUMP_APPEND && hFile != PUMP_INVALID_FILE)
    {
#   if (defined PUMP_OS_WINPHONE)
        LARGE_INTEGER li = { 0 };
        ::SetFilePointerEx(hFile, li, PUMP_NULL, FILE_END);
#   else
        ::SetFilePointer(hFile, 0, PUMP_NULL, FILE_END);
#   endif // (defined PUMP_OS_WINPHONE)
    }
#elif (defined PUMP_OS_POSIX)
    int hFile = (int)(long)PUMP_INVALID_FILE;
    int flag = 0;
    mode_t mode;

    if (!pFileName)
    {
        return (pump_handle_t)PUMP_ERROR;
    }

    //flag
    flag = PUMP_CORE_GetOFlagByFlag(nFlag);
    if (flag < 0)
    {
        return (pump_handle_t)hFile;
    }

    //mode
    mode = PUMP_CORE_GetModeByPerm(nFileAttr);

    hFile = open(pFileName, flag, mode);
#endif // (defined PUMP_OS_WINDOWS)
    return (pump_handle_t)hFile;
}

/**
* Function:	PUMP_CORE_FileClose
* Desc:		close file
* Input:		@param hFile: file handle to close, file handle is return by PUMP_CORE_FileOpen
* Output:
* Return:		if success return PUMP_OK, otherwise return PUMP_ERROR.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileClose(pump_handle_t hFile)
{
    if (hFile == PUMP_INVALID_FILE)
    {
        return PUMP_ERROR;
    }

#if (defined PUMP_OS_WINDOWS)
    if (::CloseHandle(hFile))
    {
        return PUMP_OK;
    }
    return PUMP_ERROR;
#elif (defined PUMP_OS_POSIX)
    return close((int)(long)hFile);
#endif // (defined PUMP_OS_WINDOWS)
}

/**
* Function:	PUMP_CORE_FileDelete
* Desc:		delete file
* Input:		@param pFileName: name of file to delete.
* Output:
* Return:		if success return PUMP_OK, otherwise return PUMP_ERROR.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileDelete(const char* pFileName)
{
    if (!pFileName)
    {
        return PUMP_ERROR;
    }

#if (defined PUMP_OS_WINDOWS)
#   if (defined PUMP_OS_WINCE) || (defined PUMP_OS_WINPHONE)
    wchar_t wFileName[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, pFileName, ::strlen(pFileName), wFileName, sizeof(wFileName));
    return (::DeleteFileW(wFileName)) ? PUMP_OK : PUMP_ERROR;
#   else
    return (::DeleteFile((LPCSTR)pFileName)) ? PUMP_OK : PUMP_ERROR;
#   endif
#elif (defined PUMP_OS_POSIX)
    return remove(pFileName);
#endif // (defined PUMP_OS_WINDOWS)
}

/**
* Function:	PUMP_CORE_FileRead
* Desc:		read data from file opened.
* Input:		@param hFile: file handle
@param nNumberOfBytesToRead: number bytes to read.
* Output:		@param pBuf: memory to store readed data.
@param pNumberOfBytesRead: number of bytes read.
* Return:		if success return PUMP_OK, otherwise return PUMP_ERROR.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileRead(pump_handle_t hFile, pump_pvoid_t pBuf, pump_uint32_t nNumberOfBytesToRead, pump_uint32_t* pNumberOfBytesRead)
{
#if (defined PUMP_OS_WINDOWS)
    DWORD dwNumberOfBytesRead = 0;

    if ((hFile != PUMP_INVALID_FILE) && (::ReadFile(hFile, pBuf, nNumberOfBytesToRead, (LPDWORD)&dwNumberOfBytesRead, PUMP_NULL)))
    {
        if (pNumberOfBytesRead)
        {
            *pNumberOfBytesRead = dwNumberOfBytesRead;
        }

        return PUMP_OK;
    }

    if (pNumberOfBytesRead)
    {
        *pNumberOfBytesRead = 0;
    }
#elif (defined PUMP_OS_POSIX)
    if ((PUMP_INVALID_FILE != hFile) && (pBuf))
    {
        if (pNumberOfBytesRead)
        {
            if (-1 != (pump_int32_t)(*pNumberOfBytesRead = read((int)(long)hFile, pBuf, nNumberOfBytesToRead)))
            {
                return PUMP_OK;
            }
        }
        else
        {
            if (-1 != read((int)(long)hFile, pBuf, nNumberOfBytesToRead))
            {
                return PUMP_OK;
            }
        }
    }
#endif // (defined PUMP_OS_WINDOWS)
    return PUMP_ERROR;
}

/**
* Function:	PUMP_CORE_FileWrite
* Desc:		write data to file
* Input:		@param hFile: file handle open by PUMP_CORE_FileOpen.
@param pBuf: buf to store write data.
@param nNumberOfBytesToWrite:number of byte to write.
* Output:		@param pNumberOfBytesWrite: number of byte wrote.
* Return:		if success return PUMP_OK, otherwise return PUMP_ERROR.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileWrite(pump_handle_t hFile, const pump_pvoid_t pBuf, pump_uint32_t nNumberOfBytesToWrite, pump_uint32_t* pNumberOfBytesWrite)
{
#if (defined PUMP_OS_WINDOWS)
    DWORD dwNumberOfBytesRead = 0;

    if ((hFile != PUMP_INVALID_FILE) && (::WriteFile(hFile, pBuf, nNumberOfBytesToWrite, (LPDWORD)&dwNumberOfBytesRead, PUMP_NULL)))
    {
        if (pNumberOfBytesWrite)
        {
            *pNumberOfBytesWrite = dwNumberOfBytesRead;
        }

        return PUMP_OK;
    }

    if (pNumberOfBytesWrite)
    {
        *pNumberOfBytesWrite = 0;
    }
#elif (defined PUMP_OS_POSIX)
    if ((PUMP_INVALID_FILE != hFile) && (pBuf))
    {
        if (pNumberOfBytesWrite)
        {
            if (-1 != (pump_int32_t)(*pNumberOfBytesWrite = write((int)(long)hFile, pBuf, nNumberOfBytesToWrite)))
            {
                return PUMP_OK;
            }
        }
        else
        {
            if (-1 != write((int)(long)hFile, pBuf, nNumberOfBytesToWrite))
            {
                return PUMP_OK;
            }
        }
    }
#endif // (defined PUMP_OS_WINDOWS)
    return PUMP_ERROR;
}

/**
* Function:	PUMP_CORE_FileSeek
* Desc:		seek to file pos.
* Input:		@param hFile: file handle opened by the PUMP_CORE_FileOpen.
@param iOffset: offst over the base .
@param nWhence: the base of the seek.
* Output:		@param iCurOffset: current file point pos.
* Return:		if success return PUMP_OK, otherwise return PUMP_ERROR.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileSeek(pump_handle_t hFile, pump_int64_t iOffset, pump_uint32_t nWhence, pump_int64_t* iCurOffset)
{
#if (defined PUMP_OS_WINDOWS)
    LARGE_INTEGER lInt;

    lInt.QuadPart = iOffset;
#if defined PUMP_OS_WINPHONE
    if (!::SetFilePointerEx(hFile, lInt, &lInt, nWhence))
    {
        return PUMP_ERROR;
    }
#else
    lInt.LowPart = ::SetFilePointer(hFile, lInt.LowPart, &lInt.HighPart, nWhence);
#endif
    if (((DWORD)(-1) == lInt.LowPart) && (NO_ERROR != PUMP_CORE_GetSystemError()))
    {
        return PUMP_ERROR;
    }

    if (iCurOffset)
    {
        *iCurOffset = lInt.QuadPart;
    }
#elif (defined PUMP_OS_POSIX)
    if (iCurOffset)
    {
#   if defined (__linux__)
        *iCurOffset = lseek64((int)(long)hFile, iOffset, nWhence);
#   elif defined (PUMP_OS_APPLE)
        *iCurOffset = lseek((int)(long)hFile, iOffset, nWhence);
#   endif
        if (-1 == *iCurOffset)
        {
            //printf("last error = %d\n", errno);
            return PUMP_ERROR;
        }
    }
    else
    {
#   if defined (__linux__)
        if (-1 == lseek64((int)(long)hFile, iOffset, nWhence))
#   elif defined (PUMP_OS_APPLE)
        if (-1 == lseek((int)(long)hFile, iOffset, nWhence))
#   endif
        {
            //printf("last error = %d\n", errno);
            return PUMP_ERROR;
        }
    }
#endif // (defined PUMP_OS_WINDOWS)
    return PUMP_OK;
}

/**
* Function:	PUMP_CORE_FileIsEOF
* Desc:		judge if file if end.
* Input:		@param hFile: file handle opened by PUMP_CORE_FileOpen.
* Output:
* Return:		if success return PUMP_TRUE, otherwise return PUMP_FALSE.
* */
PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_FileIsEOF(pump_handle_t hFile)
{
#if (defined PUMP_OS_WINDOWS)
    if (hFile != PUMP_INVALID_FILE)
    {
        return (ERROR_HANDLE_EOF == PUMP_CORE_GetSystemError()) ? PUMP_TRUE : PUMP_FALSE;
    }
    return PUMP_FALSE;
#elif (defined PUMP_OS_POSIX)
    return PUMP_TRUE;
#endif // (defined PUMP_OS_WINDOWS)
}

/**
* Function:	PUMP_CORE_FileStat
* Desc:		statistic the file information.
* Input:		@param hFile: file handle opened by PUMP_CORE_FileOpen.
* Output:		@param pInfo: file info.
* Return:		if success return PUMP_OK, otherwise return PUMP_ERROR.
* */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileStat(pump_handle_t hFile, PUMP_FINFO* pFInfo)
{
#if (defined PUMP_OS_WINDOWS)
    pump_int32_t iRet1 = PUMP_ERROR;
    pump_int32_t iRet2 = PUMP_ERROR;

    if (hFile == PUMP_INVALID_FILE || pFInfo == PUMP_NULL)
    {
        return PUMP_ERROR;
    }

    ZeroMemory(pFInfo, sizeof(PUMP_FINFO));

#   if (defined PUMP_OS_WINPHONE)
    FILE_STANDARD_INFO fsi = { 0 };
    FILE_ID_EXTD_DIR_INFO fiedi = { 0 };
    FILE_ID_INFO fii = { 0 };

    if (!::GetFileInformationByHandleEx(hFile, FileStandardInfo, &fsi, sizeof(fsi)))
    {
        return PUMP_ERROR;
    }
    if (!::GetFileInformationByHandleEx(hFile, FileIdExtdDirectoryInfo, &fiedi, sizeof(fiedi)))
    {
        return PUMP_ERROR;
    }
    if (!::GetFileInformationByHandleEx(hFile, FileIdInfo, &fii, sizeof(fii)))
    {
        return PUMP_ERROR;
    }

    PUMP_CORE_Inner_Convert2DOWRDTOINT64(fiedi.CreationTime.u.LowPart, fiedi.CreationTime.u.HighPart, &pFInfo->iCreateTime);
    PUMP_CORE_Inner_Convert2DOWRDTOINT64(fiedi.LastWriteTime.u.LowPart, fiedi.LastWriteTime.u.HighPart, &pFInfo->iWriteTime);
    PUMP_CORE_Inner_Convert2DOWRDTOINT64(fiedi.LastAccessTime.u.LowPart, fiedi.LastAccessTime.u.HighPart, &pFInfo->iAccessTime);
    PUMP_CORE_Inner_Convert2DOWRDTOINT64(fsi.AllocationSize.u.LowPart, fsi.AllocationSize.u.HighPart, &pFInfo->nSize);
    if (fiedi.FileAttributes & FILE_ATTRIBUTE_READONLY)
    {
        pFInfo->nProtection |= PUMP_ATTR_READONLY;
    }
    else
    {
        pFInfo->nProtection |= (PUMP_ATTR_WRITE | PUMP_ATTR_EXECUTABLE);
    }
    if (fiedi.FileAttributes & FILE_ATTRIBUTE_HIDDEN)
    {
        pFInfo->nProtection |= PUMP_ATTR_HIDDEN;
    }
    pFInfo->nHardLink = fsi.NumberOfLinks;
    pFInfo->nDeviceID = (pump_uint32_t)fii.VolumeSerialNumber;
    pFInfo->iINode = fiedi.FileIndex;

    return PUMP_OK;
#   else
    BY_HANDLE_FILE_INFORMATION struFileInfo = { 0 };

#       if !(defined PUMP_OS_WINCE)
    DWORD dwFileType;
    //WinCE have no ::GetFileType function
    if (FILE_TYPE_UNKNOWN == (dwFileType = ::GetFileType(hFile)))
    {
        return PUMP_ERROR;
    }
    switch (dwFileType)
    {
    case FILE_TYPE_CHAR:
        pFInfo->nFileType = PUMP_TYPE_CHAR;
        break;
    case FILE_TYPE_DISK:
        pFInfo->nFileType = PUMP_TYPE_DISK;
        break;
    case FILE_TYPE_PIPE:
        pFInfo->nFileType = PUMP_TYPE_PIPE;
        break;
    default:
        pFInfo->nFileType = PUMP_TYPE_UNKNOWN;
        break;
    }
#       endif // !(defined PUMP_OS_WINCE)
    if (::GetFileInformationByHandle(hFile, &struFileInfo))
    {
        if (struFileInfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            pFInfo->nProtection |= PUMP_ATTR_READONLY;
        }
        else
        {
            pFInfo->nProtection |= (PUMP_ATTR_WRITE | PUMP_ATTR_EXECUTABLE);
        }

        if (struFileInfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        {
            pFInfo->nProtection |= PUMP_ATTR_HIDDEN;
        }

        pFInfo->nHardLink = struFileInfo.nNumberOfLinks;
        pFInfo->nDeviceID = struFileInfo.dwVolumeSerialNumber;
        PUMP_CORE_Inner_2DWORD2INT64(struFileInfo.nFileIndexLow, struFileInfo.nFileIndexHigh, &pFInfo->iINode);
        PUMP_CORE_Inner_2DWORD2INT64(struFileInfo.nFileSizeLow, struFileInfo.nFileSizeHigh, &pFInfo->nSize);
        PUMP_CORE_Inner_FileTimeToTime(&struFileInfo.ftCreationTime, &pFInfo->iCreateTime);
        PUMP_CORE_Inner_FileTimeToTime(&struFileInfo.ftLastAccessTime, &pFInfo->iAccessTime);
        PUMP_CORE_Inner_FileTimeToTime(&struFileInfo.ftLastWriteTime, &pFInfo->iWriteTime);

        iRet1 = PUMP_OK;
    }
#       if !(defined PUMP_OS_WINCE)
    //WinCE have no ::GetSecurityInfo function
    SECURITY_INFORMATION struSinInfo = 0;
    PSECURITY_DESCRIPTOR pDesc = PUMP_NULL;
    if (ERROR_SUCCESS == ::GetSecurityInfo(hFile, SE_FILE_OBJECT, struSinInfo, \
        &pFInfo->vUID, \
        &pFInfo->vGID, \
        PUMP_NULL, \
        PUMP_NULL, &pDesc))
    {
        if (pDesc)
        {
            ::LocalFree(pDesc);
            //pDesc = PUMP_NULL;
        }

        iRet2 = PUMP_OK;
    }
#       endif // !(defined PUMP_OS_WINCE)
    if ((PUMP_OK == iRet1) && (PUMP_OK == iRet2))
    {
        return PUMP_OK;
    }
    return PUMP_ERROR;
#   endif // (defined PUMP_OS_WINPHONE)
#elif (defined PUMP_OS_POSIX)
    int iRet = 0;
#   if defined(PUMP_OS_APPLE)
    struct stat st = { 0 };
#   else
    struct stat64 st = { 0 };
#   endif // defined(PUMP_OS_APPLE)

    if ((!pFInfo) || (PUMP_INVALID_FILE == hFile))
    {
        return PUMP_ERROR;
    }

#   if defined(PUMP_OS_APPLE)
    iRet = fstat((int)(long)hFile, &st);
#   else
    iRet = fstat64((int)(long)hFile, &st);
#   endif // defined(PUMP_OS_APPLE)
    if (!iRet)
    {
        pFInfo->nFileType = PUMP_CORE_GetFileTypeByMode(st.st_mode);
        pFInfo->nProtection = PUMP_CORE_GetPermByMode(st.st_mode);
        pFInfo->vUID = st.st_uid;
        pFInfo->vGID = st.st_gid;
        pFInfo->nSize = st.st_size;
        pFInfo->iINode = st.st_ino;
        pFInfo->nHardLink = st.st_nlink;
        pFInfo->nDeviceID = st.st_rdev;
        pFInfo->iAccessTime = st.st_atime;
        pFInfo->iCreateTime = st.st_ctime;
        pFInfo->iWriteTime = st.st_mtime;
        return PUMP_OK;
    }

    return PUMP_ERROR;
#endif // (defined PUMP_OS_WINDOWS)
}

/**
Function:	PUMP_CORE_FileFlush
Desc:		flush data to file
Input:		hFile;
Output:
Return:		0-succ, -1-fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileFlush(pump_handle_t hFile)
{
#if (defined PUMP_OS_WINDOWS)
    return ::FlushFileBuffers(hFile) ? PUMP_OK : PUMP_ERROR;
#elif (defined PUMP_OS_POSIX)
    if (PUMP_INVALID_FILE == hFile)
    {
        return PUMP_ERROR;
    }
    return fsync((int)(long)hFile);
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetWorkDir(char* buf, pump_int32_t size)
{
#if (defined PUMP_OS_WINDOWS)
#   if (defined PUMP_OS_WINPHONE)
    return PUMP_ERROR;
#   else
    TCHAR path[PUMP_MAX_FILEPATH] = { 0 };
    int retVal = ::GetModuleFileName(PUMP_NULL, path, PUMP_MAX_FILEPATH);
    if (retVal == 0)
    {
        return PUMP_ERROR;
    }

    int i = 0;
#       if (defined PUMP_OS_WINCE)
    int len = (int)::wcslen(path);
#       else
    int len = (int)::strlen(path);
#       endif // (defined PUMP_OS_WINCE)
    while (path[i++] != '\0')
    {
        if (path[i] == '\\' && path[i + 1] == '\\')
        {
            path[i] = '/';
            ::memmove(&path[i + 1], &path[i + 2], (len - i - 1) * sizeof(TCHAR));
            path[len - 1] = '\0';
            len = len - 1;
        }
    }

#   if (defined PUMP_OS_WINCE)
    len = (int)::wcslen(path);
#   else
    len = (int)::strlen(path);
#   endif // (defined PUMP_OS_WINCE)

    if (len > size)
    {
        return PUMP_ERROR;
    }

    ::memcpy(buf, path, len + 1);
    return PUMP_OK;
#   endif // (defined PUMP_OS_WINPHONE)
#elif (defined PUMP_OS_POSIX)
    if (buf == NULL)
    {
        errno = EINVAL;
        return PUMP_ERROR;
    }

#   if defined (PUMP_OS_APPLE)
    char name_buf[PUMP_MAX_FILEPATH] = { 0 };//PUMP_MAX_FILEPATH=1024
    PUMP_UINT32 nLength = sizeof(name_buf);

    if (_NSGetExecutablePath(name_buf, &nLength) != 0)
    {
        return PUMP_ERROR;
    }

    char name_resolve[PUMP_MAX_FILEPATH] = { 0 };
    if (realpath(name_buf, name_resolve) == NULL)
    {
        return PUMP_ERROR;
    }

    nLength = strlen(name_resolve);
    if (size < nLength + 1)
    {
        errno = ENOMEM;
        return PUMP_ERROR;
    }

    memcpy(buf, name_resolve, nLength + 1);
#   else
    char path[PUMP_MAX_FILEPATH] = { 0 };
    int count = readlink("/proc/self/exe", path, PUMP_MAX_FILEPATH);
    if (size < count)
    {
        errno = ENOMEM;
        return PUMP_ERROR;
    }

    memcpy(buf, path, count);
#   endif // defined (PUMP_OS_APPLE)
    return PUMP_OK;
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileAccess(const char *path, int mode)
{
#if (defined PUMP_OS_WINDOWS)
    if (mode & PUMP_X_OK)
    {
        return PUMP_ERROR;
    }
#   if !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    //WinCE have no ::_access()
    return _access(path, mode);
#   else
    return PUMP_ERROR;
#   endif // !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
#elif (defined PUMP_OS_POSIX)
    return access(path, mode);
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileAttributes(const char *szPath)
{
#if (defined PUMP_OS_WINDOWS)
#   if !defined PUMP_OS_WINCE && !defined PUMP_OS_WINPHONE
    //WinCE have no ::_access()
    DWORD dwAttrib = ::GetFileAttributesA(szPath);
    return dwAttrib;
#   else
    return PUMP_ERROR;
#   endif // !defined PUMP_OS_WINCE && !defined PUMP_OS_WINPHONE
#elif (defined PUMP_OS_POSIX)
    return PUMP_ERROR;
#endif // (defined PUMP_OS_WINDOWS)
}

#if  (defined PUMP_OS_WINDOWS)
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileProperties(const char * szPath, const char* szProperties, char* szOut, pump_uint32_t dwOut)
{
    DWORD dwSize;
    DWORD dwRtn;
    //Get size of file version data.
    dwSize = ::GetFileVersionInfoSizeA(szPath, NULL);
    if (dwSize == 0)
    {
        return PUMP_ERROR;
    }
    char *pBuf;
    pBuf = new char[dwSize + 1];
    if (pBuf == NULL)
    {
        delete pBuf;
        return PUMP_ERROR;
    }
    memset(pBuf, 0, dwSize + 1);
    // Get file version info.
    dwRtn = ::GetFileVersionInfoA(szPath, NULL, dwSize, pBuf);
    if (dwRtn == 0)
    {
        delete pBuf;
        return PUMP_ERROR;
    }
    LPVOID lpBuffer = NULL;
    UINT uLen = 0;
    DWORD* pTransTable;
    // set language
    if (!::VerQueryValueA(pBuf, "\\VarFileInfo\\Translation", (void **)&pTransTable, &uLen))
        return PUMP_ERROR;
    DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
    if (dwLangCharset == NULL)
    {
        delete pBuf;
        return PUMP_ERROR;
    }

    char szQuery[128 + 1] = { 0 };
    if (NULL == szQuery)
    {
        delete pBuf;
        return PUMP_ERROR;
    }
    _snprintf(szQuery, 128, "\\StringFileInfo\\%08lx\\%s", dwLangCharset, szProperties);
    dwRtn = ::VerQueryValueA(
        pBuf,
        szQuery,
        &lpBuffer,
        &uLen);
    if (dwRtn == 0)
    {
        delete pBuf;
        return PUMP_ERROR;
    }
    if (strlen((char*)lpBuffer) > dwOut)
    {
        // buffer not enough.
        delete pBuf;
        return PUMP_ERROR;
    }
    ::strcpy(szOut, (char*)lpBuffer);
    delete pBuf;
    return PUMP_OK;
}
#endif // PUMP_OS_WINDOWS

PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_FileIsExist(const char *szPath)
{
#if (defined PUMP_OS_WINDOWS)
    DWORD dwAttrib = PUMP_CORE_FileAttributes(szPath);
    return (INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#elif (defined PUMP_OS_POSIX)
    return PUMP_FALSE;
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileCopy(const char* src, const char* dst, pump_bool_t bFailIfExists)
{
#if (defined PUMP_OS_WINDOWS)
#   if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wsrc[260] = { 0 };
    wchar_t wdst[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, src, ::strlen(src), wsrc, sizeof(wsrc));
    ::MultiByteToWideChar(CP_ACP, 0, dst, ::strlen(dst), wdst, sizeof(wdst));
#       if defined PUMP_OS_WINCE
    return ::CopyFile(wsrc, wdst, bFailIfExists) == TRUE ? PUMP_OK : PUMP_ERROR;
#       else
    COPYFILE2_EXTENDED_PARAMETERS cep = { 0 };
    cep.dwSize = sizeof(cep);
    if (bFailIfExists)
    {
        cep.dwCopyFlags |= COPY_FILE_FAIL_IF_EXISTS;
    }
    return ::CopyFile2(wsrc, wdst, &cep) == S_OK ? PUMP_OK : PUMP_ERROR;
#       endif // defined PUMP_OS_WINCE
#   else
    return ::CopyFile(src, dst, bFailIfExists) ? PUMP_OK : PUMP_ERROR;
#   endif // defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
#elif (defined PUMP_OS_POSIX)
    int fdin = -1;
    int fdout = -1;
    void* pSrc, *pDst;
    struct stat statbuf;
    int retVal = -1;

    int extFile = access(dst, F_OK);
    if (bFailIfExists && extFile == 0)
    {
        return -1;
    }

    if ((fdin = open(src, O_RDONLY)) < 0)
    {
        goto ret_copy;
    }

    /* need size of input file */
    if (fstat(fdin, &statbuf) < 0)
    {
        goto ret_copy;
    }

    if ((fdout = open(dst, O_RDWR | O_CREAT | O_TRUNC, statbuf.st_mode)) < 0)
    {
        goto ret_copy;
    }

    /* set size of output file */
    if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1)
    {
        goto ret_copy;
    }

    if (write(fdout, "", 1) != 1)
    {
        goto ret_copy;
    }

    if ((pSrc = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED)
    {
        goto ret_copy;
    }

    if ((pDst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED)
    {
        goto ret_copy;
    }

    /* does the file copy */
    memcpy(pDst, pSrc, statbuf.st_size);
    return true;

ret_copy:
    if (fdin != -1)
    {
        close(fdin);
        fdin = -1;
    }

    if (fdout != -1)
    {
        close(fdout);
        fdout = -1;
    }

    if (src)
    {
        munmap(pSrc, statbuf.st_size);
        src = NULL;
    }

    if (dst)
    {
        munmap(pDst, statbuf.st_size);
        src = NULL;
    }

    return retVal;
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileRename(const char* oldName, const char* newName)
{
    if (oldName == PUMP_NULL || newName == PUMP_NULL)
    {
        return PUMP_ERROR;
    }
#if (defined PUMP_OS_WINDOWS)
#   if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wsrc[260] = { 0 };
    wchar_t wdst[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, oldName, ::strlen(oldName), wsrc, sizeof(wsrc));
    ::MultiByteToWideChar(CP_ACP, 0, newName, ::strlen(newName), wdst, sizeof(wdst));
#       if defined PUMP_OS_WINPHONE
    return ::MoveFileExW(wsrc, wdst, MOVEFILE_COPY_ALLOWED) ? PUMP_OK : PUMP_ERROR;
#       else
    return ::MoveFile(wsrc, wdst) ? PUMP_OK : PUMP_ERROR;
#       endif // defined PUMP_OS_WINPHONE
#   else
    return ::MoveFile(oldName, newName) ? PUMP_OK : PUMP_ERROR;
#   endif // defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
#elif (defined PUMP_OS_POSIX)
    return rename(oldName, newName);
#endif // (defined PUMP_OS_WINDOWS)
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetBinaryFileArch(const char* szPath)
{
    if (szPath == PUMP_NULL)
    {
        return PUMP_ERROR;
    }
    pump_int32_t ret = PUMP_ERROR;
#if defined PUMP_OS_WINDOWS
    pump_handle_t hFile = PUMP_CORE_FileOpen(szPath, PUMP_READ, PUMP_ATTR_READONLY);
    if (hFile == PUMP_INVALID_FILE)
    {
        PUMP_CORE_ERR << "PUMP_CORE_GetBinaryFileArch() failed " << PUMP_CORE_GetSystemError();
        return PUMP_ERROR;
    }
    unsigned char szBuff[2048] = { 0 };
    pump_uint32_t dwRead = 0;
    PUMP_CORE_FileRead(hFile, szBuff, sizeof(szBuff), &dwRead);
    for (pump_uint32_t i = 0; i < dwRead - 5; ++i)
    {
        if (szBuff[i] == 0x50 && szBuff[i+1] == 0x45) // PE file flag
        {
            if (szBuff[i+4] == 0x64) // PE..d 64 bit exe file
            {
                ret = 64;
                break;
            }
            else if (szBuff[i + 4] == 0x4c) // PE..L 32 bit exe file
            {
                ret = 32;
                break;
            }
            else // unknown exe file
            {
                ret = 0;
                break;
            }
        }
    }
    PUMP_CORE_FileClose(hFile);
#else
#endif
    return ret;
}

