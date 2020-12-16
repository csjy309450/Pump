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
#include <dirent.h>
#ifdef PUMP_OS_APPLE
#include <mach-o/dyld.h>
#include <sys/param.h>
#endif
#endif // PUMP_OS_WINDOWS
#include "pump_core/pump_core_logger.h"
#include "pump_core/os_wrapper/pump_core_os_api.h"
#include "pump_core/os_wrapper/pump_core_os_time.h"
#include "pump_core/__pump_core_time_inner.h"
#include "pump_core/pump_core_file.h"

typedef struct tagPUMP_DIR_INFO
{
#if (defined PUMP_OS_WINDOWS)
    pump_handle_t hFindHandle;
    char   *szDirName;
#   if!(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    WIN32_FIND_DATAA struFindData;
#   else
    WIN32_FIND_DATAW struFindData;
#   endif // !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
    bool   bFirst;
#elif (defined PUMP_OS_POSIX)
    DIR  *pDirStru;
    char *pDirName;
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
            ::free(szDirInfo->szDirName);
        }
        ::free(szDirInfo);
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

/**
 * @brief Get file info from WIN32_FIND_DATA
 * @param (IN) WIN32_FIND_DATA *pFindData
 * @param (OUT) FILE_FIND_INFO *pFileInfo
 */
#if !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
void PUMP_CORE_Inner_FillFileInfo(WIN32_FIND_DATAA *pFindData, PUMP_FILEFIND_INFO *pFileInfo)
{
    ZeroMemory(pFileInfo, sizeof(*pFileInfo));
    ::memcpy(pFileInfo->sFileName, pFindData->cFileName, sizeof(pFileInfo->sFileName));
#else
void PUMP_CORE_Inner_FillFileInfo(WIN32_FIND_DATAW *pFindData, PUMP_FILEFIND_INFO *pFileInfo)
{
    ZeroMemory(pFileInfo, sizeof(*pFileInfo));
    WideCharToMultiByte(CP_ACP, 0, pFindData->cFileName, ::wcslen(pFindData->cFileName), pFileInfo->sFileName, sizeof(pFileInfo->sFileName), PUMP_NULL, PUMP_NULL);
#endif  // !(defined PUMP_OS_WINCE) && !(defined PUMP_OS_WINPHONE)
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
 * @brief open dir
 * @param (IN) const char *szDirName dir path
 * @return fail-PUMP_NULL, succ-dir handler
 */
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_DirOpen(const char *szDirName)
{
    if (PUMP_NULL == szDirName)
    {
        return PUMP_INVALID_HANDLE;
    }

    int iDirLen = (int)::strlen(szDirName);
    PUMP_DIR_INFO *szDirInfo = PUMP_CORE_Inner_AllocDirInfo(iDirLen + 3);
    if (PUMP_NULL == szDirInfo)
    {
        return PUMP_INVALID_HANDLE;
    }

    // init szDirInfo
    szDirInfo->hFindHandle = INVALID_HANDLE_VALUE;
    szDirInfo->bFirst = false;

    ::memcpy(szDirInfo->szDirName, szDirName, iDirLen);
    if (szDirInfo->szDirName[iDirLen - 1] != '/' && szDirInfo->szDirName[iDirLen - 1] != '\\')
    {
        szDirInfo->szDirName[iDirLen] = '/';
        szDirInfo->szDirName[iDirLen + 1] = '*';
    }
    else
    {
        szDirInfo->szDirName[iDirLen] = '*';
    }

    if (PUMP_CORE_DirFindFile((pump_handle_t)szDirInfo, PUMP_NULL) != PUMP_OK)
    {
        PUMP_CORE_Inner_FreeDirInfo(szDirInfo);
        return PUMP_INVALID_HANDLE;
    }

    return (pump_handle_t)szDirInfo;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirFindFile(pump_handle_t hDir, PUMP_FILEFIND_INFO *pFileInfo)
{
    if (PUMP_INVALID_HANDLE == hDir)
    {
        return PUMP_ERROR;
    }

    PUMP_DIR_INFO *szDirInfo = (PUMP_DIR_INFO *)hDir;

    if (INVALID_HANDLE_VALUE == szDirInfo->hFindHandle)
    {
#if !defined(PUMP_OS_WINCE) && !defined PUMP_OS_WINPHONE
        HANDLE hFindHandle = ::FindFirstFile(szDirInfo->szDirName, (LPWIN32_FIND_DATAA)&szDirInfo->struFindData);
#else
        wchar_t dirname[MAX_PATH] = { 0 };
        ::MultiByteToWideChar(CP_ACP, 0, szDirInfo->szDirName, ::strlen(szDirInfo->szDirName), dirname, sizeof(dirname));
#if (defined PUMP_OS_WINCE)
        HANDLE hFindHandle = ::FindFirstFileW(dirname, &szDirInfo->struFindData);
#else
        HANDLE hFindHandle = ::FindFirstFileExW(dirname, FindExInfoStandard, &szDirInfo->struFindData\
            , FindExSearchNameMatch, PUMP_NULL, FIND_FIRST_EX_CASE_SENSITIVE);
#endif // (defined PUMP_OS_WINCE)
#endif
        if (INVALID_HANDLE_VALUE == hFindHandle)
        {
            return PUMP_ERROR;
        }
        szDirInfo->hFindHandle = hFindHandle;
        szDirInfo->bFirst = true;
        return PUMP_OK;
    }

    // output param check
    if (PUMP_NULL == pFileInfo)
    {
        return PUMP_ERROR;
    }

    //first file was found when called OpenDir
    if (szDirInfo->bFirst)
    {
        PUMP_CORE_Inner_FillFileInfo(&szDirInfo->struFindData, pFileInfo);
        szDirInfo->bFirst = false;
        return PUMP_OK;
    }

    // find next file
#if !defined(PUMP_OS_WINCE) && !defined PUMP_OS_WINPHONE
    if (!::FindNextFileA(szDirInfo->hFindHandle, &szDirInfo->struFindData))
#else
    if (!::FindNextFileW(szDirInfo->hFindHandle, &szDirInfo->struFindData))
#endif
    {
        if (ERROR_NO_MORE_FILES == PUMP_CORE_GetSystemError())
        {
            //OutputDebugString("No more files!\n");
        }
        return PUMP_ERROR;
    }

    PUMP_CORE_Inner_FillFileInfo(&szDirInfo->struFindData, pFileInfo);
    return PUMP_OK;
}

/**
 * @brief close dir hander
 * @param (IN) pump_handle_t hDir dir hander
 * @return fail-PUMP_ERROR, succ-PUMP_OK
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirClose(pump_handle_t hDir)
{
    if (PUMP_INVALID_HANDLE == hDir)
    {
        return PUMP_ERROR;
    }

    PUMP_DIR_INFO *szDirInfo = (PUMP_DIR_INFO*)hDir;
    FindClose(szDirInfo->hFindHandle);
    PUMP_CORE_Inner_FreeDirInfo(szDirInfo);

    return PUMP_OK;
}

//////////////////////////////////////////////////////////////////////////

/**
 * Function:    PUMP_CORE_FileOpen
 * Desc:        open file in user asked mode.
 * Input:        @param pFileName: file name, abs name | relative name.
 * @param nFlag: open flag, can be composed by (|) operation
 * @param nFileAttr: file attr, if file exist, this param will be ignored.
 * Output:
 * Return:        if success return PUMP_FILE_HANDLE, otherwise return PUMP_FILE_INVALID_HANDLE.
 */
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_FileOpen(const char* pFileName, pump_uint32_t nFlag, pump_uint32_t nFileAttr)
{
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
    //    dwCreateFlag |= FILE_FLAG_NO_BUFFERING;
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

#if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wFileName[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, pFileName, ::strlen(pFileName), wFileName, sizeof(wFileName));
#if defined PUMP_OS_WINCE
    hFile = CreateFile(wFileName, dwDesiredAccess, dwShareMode, PUMP_NULL, \
        dwCreateFlag, dwFlagAttr, 0);
#else
    CREATEFILE2_EXTENDED_PARAMETERS cep = { 0 };
    cep.dwSize = sizeof(cep);
    cep.dwFileAttributes = dwFlagAttr;
    hFile = CreateFile2(wFileName, dwDesiredAccess, dwShareMode, dwCreateFlag, &cep);
#endif
#else
    hFile = CreateFile((LPCSTR)pFileName, dwDesiredAccess, dwShareMode, PUMP_NULL, \
        dwCreateFlag, dwFlagAttr, 0);
#endif

    if (nFlag & PUMP_APPEND && hFile != PUMP_INVALID_FILE)
    {
#if defined PUMP_OS_WINPHONE
        LARGE_INTEGER li = { 0 };
        ::SetFilePointerEx(hFile, li, PUMP_NULL, FILE_END);
#else
        ::SetFilePointer(hFile, 0, PUMP_NULL, FILE_END);
#endif
    }

    return hFile;
}

/**
 * Function:    PUMP_CORE_FileClose
 * Desc:        close file
 * Input:        @param hFile: file handle to close, file handle is return by PUMP_CORE_FileOpen
 * Output:
 * Return:        if success return PUMP_OK, otherwise return PUMP_ERROR.
 * */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileClose(pump_handle_t hFile)
{
    if (hFile == PUMP_INVALID_FILE)
    {
        return PUMP_ERROR;
    }

    if (::CloseHandle(hFile))
    {
        return PUMP_OK;
    }

    return PUMP_ERROR;
}

/**
 * Function:    PUMP_CORE_FileDelete
 * Desc:        delete file
 * Input:        @param pFileName: name of file to delete.
 * Output:
 * Return:        if success return PUMP_OK, otherwise return PUMP_ERROR.
 * */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileDelete(const char* pFileName)
{
    if (!pFileName)
    {
        return PUMP_ERROR;
    }

#if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wFileName[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, pFileName, ::strlen(pFileName), wFileName, sizeof(wFileName));
    return (::DeleteFileW(wFileName)) ? PUMP_OK : PUMP_ERROR;
#else
    return (::DeleteFile((LPCSTR)pFileName)) ? PUMP_OK : PUMP_ERROR;
#endif
}

/**
 * Function:    PUMP_CORE_FileRead
 * Desc:        read data from file opened.
 * Input:        @param hFile: file handle
 * @param nNumberOfBytesToRead: number bytes to read.
 * Output:        @param pBuf: memory to store readed data.
 * @param pNumberOfBytesRead: number of bytes read.
 * Return:        if success return PUMP_OK, otherwise return PUMP_ERROR.
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileRead(pump_handle_t hFile, pump_pvoid_t pBuf, pump_uint32_t nNumberOfBytesToRead, pump_uint32_t* pNumberOfBytesRead)
{
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

    return PUMP_ERROR;
}

/**
 * Function:    PUMP_CORE_FileWrite
 * Desc:        write data to file
 * Input:        @param hFile: file handle open by PUMP_CORE_FileOpen.
@param pBuf: buf to store write data.
@param nNumberOfBytesToWrite:number of byte to write.
 * Output:        @param pNumberOfBytesWrite: number of byte wrote.
 * Return:        if success return PUMP_OK, otherwise return PUMP_ERROR.
 * */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileWrite(pump_handle_t hFile, const pump_pvoid_t pBuf, pump_uint32_t nNumberOfBytesToWrite, pump_uint32_t* pNumberOfBytesWrite)
{
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

    return PUMP_ERROR;
}

/**
 * Function:    PUMP_CORE_FileSeek
 * Desc:        seek to file pos.
 * Input:        @param hFile: file handle opened by the PUMP_CORE_FileOpen.
@param iOffset: offst over the base .
@param nWhence: the base of the seek.
 * Output:        @param iCurOffset: current file point pos.
 * Return:        if success return PUMP_OK, otherwise return PUMP_ERROR.
 * */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileSeek(pump_handle_t hFile, pump_int64_t iOffset, pump_uint32_t nWhence, pump_int64_t* iCurOffset)
{
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

    return PUMP_OK;
}

/**
 * Function:    PUMP_CORE_FileIsEOF
 * Desc:        judge if file if end.
 * Input:        @param hFile: file handle opened by PUMP_CORE_FileOpen.
 * Output:
 * Return:        if success return PUMP_TRUE, otherwise return PUMP_FALSE.
 * */
PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_FileIsEOF(pump_handle_t hFile)
{
    if (hFile != PUMP_INVALID_FILE)
    {
        return (ERROR_HANDLE_EOF == PUMP_CORE_GetSystemError()) ? PUMP_TRUE : PUMP_FALSE;
    }

    return PUMP_FALSE;
}

/**
 * Function:    PUMP_CORE_FileStat
 * Desc:        statistic the file information.
 * Input:        @param hFile: file handle opened by PUMP_CORE_FileOpen.
 * Output:        @param pInfo: file info.
 * Return:        if success return PUMP_OK, otherwise return PUMP_ERROR.
 * */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileStat(pump_handle_t hFile, PUMP_FINFO* pFInfo)
{
    pump_int32_t iRet1 = PUMP_ERROR;
    pump_int32_t iRet2 = PUMP_ERROR;

    if (hFile == PUMP_INVALID_FILE || pFInfo == PUMP_NULL)
    {
        return PUMP_ERROR;
    }

    ZeroMemory(pFInfo, sizeof(PUMP_FINFO));

#ifdef PUMP_OS_WINPHONE
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
#else
    BY_HANDLE_FILE_INFORMATION struFileInfo = { 0 };

#if !defined PUMP_OS_WINCE
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
#endif
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
#if !defined PUMP_OS_WINCE
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
#endif

    if ((PUMP_OK == iRet1) && (PUMP_OK == iRet2))
    {
        return PUMP_OK;
    }

    return PUMP_ERROR;
#endif
}

/**
 * Function:    PUMP_CORE_FileFlush
 * Desc:        flush data to file
 * Input:        hFile;
 * Output:
 * Return:        0-succ, -1-fail
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileFlush(pump_handle_t hFile)
{
    return ::FlushFileBuffers(hFile) ? PUMP_OK : PUMP_ERROR;
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetWorkDir(char* buf, pump_int32_t size)
{
#ifdef PUMP_OS_WINPHONE
    return PUMP_ERROR;
#else
    TCHAR path[PUMP_MAX_FILEPATH] = { 0 };
    int retVal = ::GetModuleFileName(PUMP_NULL, path, PUMP_MAX_FILEPATH);
    if (retVal == 0)
    {
        return PUMP_ERROR;
    }

    int i = 0;
#if defined PUMP_OS_WINCE
    int len = (int)::wcslen(path);
#else
    int len = (int)::strlen(path);
#endif
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

#if defined PUMP_OS_WINCE
    len = (int)::wcslen(path);
#else
    len = (int)::strlen(path);
#endif

    if (len > size)
    {
        return PUMP_ERROR;
    }

    ::memcpy(buf, path, len + 1);
    return PUMP_OK;
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileAccess(const char *path, int mode)
{
    if (mode & PUMP_X_OK)
    {
        return PUMP_ERROR;
    }
#if !defined PUMP_OS_WINCE && !defined PUMP_OS_WINPHONE
    //WinCE have no ::_access()
    return ::_access(path, mode);
#else
    return PUMP_ERROR;
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileAttributes(const char *szPath)
{
#if !defined PUMP_OS_WINCE && !defined PUMP_OS_WINPHONE
    //WinCE have no ::_access()
    DWORD dwAttrib = ::GetFileAttributesA(szPath);
    return dwAttrib;
#else
    return PUMP_ERROR;
#endif
}

#ifdef PUMP_OS_WINDOWS
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
    DWORD dwAttrib = PUMP_CORE_FileAttributes(szPath);
    return (INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileCopy(const char* src, const char* dst, pump_bool_t bFailIfExists)
{
#if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wsrc[260] = { 0 };
    wchar_t wdst[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, src, ::strlen(src), wsrc, sizeof(wsrc));
    ::MultiByteToWideChar(CP_ACP, 0, dst, ::strlen(dst), wdst, sizeof(wdst));
#if defined PUMP_OS_WINCE
    return ::CopyFile(wsrc, wdst, bFailIfExists) == TRUE ? PUMP_OK : PUMP_ERROR;
#else
    COPYFILE2_EXTENDED_PARAMETERS cep = { 0 };
    cep.dwSize = sizeof(cep);
    if (bFailIfExists)
    {
        cep.dwCopyFlags |= COPY_FILE_FAIL_IF_EXISTS;
    }
    return ::CopyFile2(wsrc, wdst, &cep) == S_OK ? PUMP_OK : PUMP_ERROR;
#endif
#else
    return ::CopyFile(src, dst, bFailIfExists) ? PUMP_OK : PUMP_ERROR;
#endif
}

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileRename(const char* oldName, const char* newName)
{
    if (oldName == PUMP_NULL || newName == PUMP_NULL)
    {
        return PUMP_ERROR;
    }
#if defined PUMP_OS_WINCE || defined PUMP_OS_WINPHONE
    wchar_t wsrc[260] = { 0 };
    wchar_t wdst[260] = { 0 };
    ::MultiByteToWideChar(CP_ACP, 0, oldName, ::strlen(oldName), wsrc, sizeof(wsrc));
    ::MultiByteToWideChar(CP_ACP, 0, newName, ::strlen(newName), wdst, sizeof(wdst));
#if defined PUMP_OS_WINPHONE
    return ::MoveFileExW(wsrc, wdst, MOVEFILE_COPY_ALLOWED) ? PUMP_OK : PUMP_ERROR;
#else
    return ::MoveFile(wsrc, wdst) ? PUMP_OK : PUMP_ERROR;
#endif
#else
    return ::MoveFile(oldName, newName) ? PUMP_OK : PUMP_ERROR;
#endif
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
        PUMP_CORE_ERR("PUMP_CORE_GetBinaryFileArch() failed %d", PUMP_CORE_GetSystemError());
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

