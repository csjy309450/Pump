/**
 * @file pump_core_file.h
 * @brief Library Header.
 * @details Declaration of file control interface.
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

#ifndef PUMP_CORE_FILE_H
#define PUMP_CORE_FILE_H
#include "pump_macro/pump_pre.h"
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"
#include "pump_core/time/pump_core_time.h"
#if defined(PUMP_OS_WINDOWS)
typedef PSID pump_uid_t;
typedef PSID pump_gid_t;
typedef DWORD pump_filetype_t;
typedef pump_int64_t pump_inode_t;
#define PUMP_INVALID_FILE INVALID_HANDLE_VALUE
#elif defined(PUMP_OS_POSIX)
typedef uid_t pump_uid_t;
typedef gid_t pump_gid_t;
typedef mode_t pump_filetype_t;
typedef pump_int64_t pump_inode_t;
#define PUMP_INVALID_FILE ((pump_handle_t)-1)
#else
#error OS Not Implement Yet.
#endif

#define PUMP_MAX_FILEPATH	260			///< windows PUMP_MAX_FILEPATH

//(file create flag)
#define PUMP_READ		0x0001		///< (read  file.)
#define PUMP_WRITE		0x0002		///< (write  file.)
#define PUMP_CREATE		0x0004		///< (create file if not exist.)
#define PUMP_APPEND		0x0008		///< (append to the end of file.)
#define PUMP_TRUNCATE	0x0010		///< (open file and truncate the length to 0.)
#define PUMP_BINARY		0x0020		///< (open file in binary mode.)
#define PUMP_EXCL		0x0040		///< (open file will be failed if exist.)
#define PUMP_BUFFERED	0x0080		///< (open file of buffered io.)
#define PUMP_SHARELOCK	0x0100		///< (share the rwlock.)
#define PUMP_XTHREAD		0x0200		///< (multithread.)
#define PUMP_LARGEFILE	0x0400		///< (large file.)
#define PUMP_DIRECT      0x0800      ///< (direct open file. according to the test, this flags should be used with pre-allocate)

//(file attribute flag)
#define PUMP_ATTR_READONLY	0x1000		///< (readonly.)
#define PUMP_ATTR_WRITE		0x2000		///< (write.)
#define PUMP_ATTR_EXECUTABLE	0x4000		///< (exeute.)
#define PUMP_ATTR_HIDDEN		0x8000		///< (hidden.)
#define PUMP_ATTR_SYSTEM		0x0100		///< (system file, windows only)
#define PUMP_DELONCLOSE		0x0200		///< (delete file when close.)

#define PUMP_USETID      0x8000 ///< Set user id 
#define PUMP_UREAD       0x0400 ///< Read by user 
#define PUMP_UWRITE      0x0200 ///< Write by user 
#define PUMP_UEXECUTE    0x0100 ///< Execute by user 

#define PUMP_GSETID      0x4000 ///< Set group id
#define PUMP_GREAD       0x0040 ///< Read by group
#define PUMP_GWRITE      0x0020 ///< Write by group
#define PUMP_GEXECUTE    0x0010 ///< Execute by group

#define PUMP_WSTICKY     0x2000 ///< Sticky bit
#define PUMP_WREAD       0x0004 ///< Read by others
#define PUMP_WWRITE      0x0002 ///< Write by others
#define PUMP_WEXECUTE    0x0001 ///< Execute by others

#define PUMP_OS_DEFAULT  0x0FFF ///< use OS's default permissions


// (file TYPE)
#define PUMP_TYPE_CHAR		0x0001		///< (char dev)
#define PUMP_TYPE_DISK		0x0002		///< (disk dev)
#define PUMP_TYPE_PIPE		0x0004		///< (pipe dev)
#define PUMP_TYPE_UNKNOWN	0x0008		///< (unknown dev)
#define PUMP_TYPE_SOCK		0x0010		///< (sock dev)
#define PUMP_TYPE_FIFO		0x0020		///< (fifo)
#define PUMP_TYPE_LINK		0x0040		///< (symbolic link)
#define PUMP_TYPE_BLK		0x0080		///< (block dev)
#define PUMP_TYPE_DIR		0x0100		///< (dir dev)
#define PUMP_TYPE_REG		0x0200		///< (reg dev)

// (file seek flag)
#define PUMP_SEEK_SET	SEEK_SET	///< (begin.)
#define PUMP_SEEK_CUR	SEEK_CUR	///< (current.)
#define PUMP_SEEK_END	SEEK_END	///< (end.)

/**
 * struct PUMP_FINFO
 */
typedef struct tagPUMP_FINFO
{
    pump_filetype_t nFileType;	///< file type
    pump_uid_t	vUID;			///< user id (posix only)
    pump_gid_t	vGID;			///< group id (posix only)
    pump_uint32_t nProtection;		///< acess permission
    pump_uint32_t nDeviceID;		///< id of the device*/
    pump_uint32_t nHardLink;		///< number of hard link
    pump_inode_t	iINode;			///< node of the file
    pump_size_t nSize;			///< size of the file
    pump_time_t iAccessTime;		///< utc time. last access time.
    pump_time_t iCreateTime;		///< utc time. last crate time.
    pump_time_t iWriteTime;		///< utc time. last change time.
} PUMP_FINFO;

typedef struct tagFILEFIND_INFO
{
    char sFileName[PUMP_MAX_FILEPATH];   ///< file name
    PUMP_FINFO FileInfo; ///< file info
}PUMP_FILEFIND_INFO;

//////////////////////////////////////////////////////////////////////////
// dir operations
//////////////////////////////////////////////////////////////////////////
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirIsExist(const char* szDir);
/**
 * @brief PUMP_CORE_DirCreate make directory byd irectory name.
 * @param (IN) szDir directory name.
 * @return 0 success, -1 fail
 */
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirCreate(const char* szDir);

/**
* @brief remove directory by directory name.
* @param (IN) szDir directory name.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirDelete(const char* szDir);

/**
*   @brief open directory by path.
*   @param (IN) szDirName path.
*   @return fail-PUMP_NULL£¬succ-directory handler
*/
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_DirOpen(const char *szDirName);

/** 
*   @brief close directory
*   @param (IN)  hDir directory handler
*   @return fail-PUMP_ERROR£¬succ-PUMP_OK
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirClose(pump_handle_t hDir);

/**
*   @brief scan files in dir (should call this function in while until failed)
*   @param (IN) hDir directory handler
*   @param (OUT) pFileInfo file info
*   @return fail-PUMP_ERROR£¬succ-PUMP_OK
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_DirFindFile(pump_handle_t hDir, PUMP_FILEFIND_INFO *pFileInfo);

//////////////////////////////////////////////////////////////////////////
// file operations
//////////////////////////////////////////////////////////////////////////
/**
* @brief PUMP_CORE_FileOpen flush the data to the disk.
* @param (IN) pFileName file name, abs name | relative name.
* @param (IN) nFlag open flag, can be composed by (|) operation
* @param (IN) dwFileAttr  file attr. will be ignore if the file already exist.
* @return  if success return PUMP_FILE_HANDLE, otherwise return PUMP_FILE_INVALID_HANDLE.
*/
PUMP_CORE_API pump_handle_t PUMP_CALLBACK PUMP_CORE_FileOpen(const char* pFileName, pump_uint32_t dwFlag, pump_uint32_t dwFileAttr);

/**
* @brief PUMP_CORE_FileClose flush the data to the disk.
* @param hFile (IN) file fd.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileClose(pump_handle_t hFile);

/**
* @brief PUMP_DeleteFile delete file
* @param szFilePath (IN) file name.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileDelete(const char* szFilePath);

/**
* @brief PUMP_CORE_FileRead read data from disk.
* @param hFile (IN) file fd.
* @param pBuf (IN) buffer to store data read from disk.
* @param nNumberOfBytesToRead (IN) number of bytes want to read.
* @param pNumberOfBytesRead (OUT) number of bytes really read.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileRead(pump_handle_t hFile, pump_pvoid_t pBuf, pump_uint32_t dwBytesToRead, pump_uint32_t* pBytesRead);

/**
* @brief PUMP_CORE_FileWrite write data to the disk.
* @param hFile (IN) file fd.
* @param pBuf (IN) date want to write to file.
* @param nNumberOfBytesToWrite (IN) number of bytes want to write.
* @param pNumberOfBytesWrite (OUT) number of bytes really write to file.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileWrite(pump_handle_t hFile, const pump_pvoid_t pBuf, pump_uint32_t dwBytesToWrite, pump_uint32_t* pBytesWrite);

/**
* @brief PUMP_CORE_FileSeek set the file pointer.
* @param hFile (IN) file fd.
* @param iOffset (IN) the offset of the base, negtive data is the opposite direction.
* @param nWhence (IN) the base position of offset.
* @param iCurOffset (OUT) the new file pointer.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileSeek(pump_handle_t hFile, pump_int64_t iOffset, pump_uint32_t dwWhere, pump_int64_t* pCurOffset);

/**
* @brief PUMP_CORE_FileIsEOF judge if reach the end of file.
* @param hFile (IN) file fd.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_FileIsEOF(pump_handle_t hFile);

/**
* @brief PUMP_CORE_FileStat stat the file info.
* @param hFile (IN) file fd.
* @param pFInfo (OUT) structure PUMP_FINFO
* @see PUMP_FINFO
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileStat(pump_handle_t hFile, PUMP_FINFO* pFInfo);

/**
* @brief PUMP_CORE_FileFlush flush the data to the disk.
* @param hFile (IN) file fd.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileFlush(pump_handle_t hFile);

#define PUMP_R_OK    4      ///< Test for read permission. 
#define PUMP_W_OK    2    ///< Test for write permission. 
#define PUMP_X_OK    1      ///< Test for execute permission. 
#define PUMP_F_OK    0       ///< Test for existence. 
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileAccess(const char *szPath, int mode);

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileAttributes(const char *szPath);

#ifdef PUMP_OS_WINDOWS
/*
 * @param [IN] szProperties
 - CompanyName
 - FileDescription
 - FileVersion
 - InternalName
 - LegalCopyright
 - OriginalFilename
 - ProductName
 - ProductVersion
 - Comments
 - LegalTrademarks
 - PrivateBuild
 - SpecialBuild
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileProperties(const char * szPath, const char* szProperties, char* szOut, pump_uint32_t dwOut);
#endif // PUMP_OS_WINDOWS

PUMP_CORE_API pump_bool_t PUMP_CALLBACK PUMP_CORE_FileIsExist(const char *szPath);

PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileCopy(const char* szSrc, const char* szDist, pump_bool_t bFailIfExists);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileMove(const char* szSrc, const char* szDist, pump_bool_t bFailIfExists);
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_FileRename(const char* szOldName, const char* szNewName);

/**
* @brief PUMP_CORE_GetWorkDir get the absolute path of the current execute file
* @param buf (OUT) execute path.
* @param size (IN) buf size.
* @return 0 success, -1 fail
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetWorkDir(char* buf, pump_int32_t size);

/**
* @brief PUMP_CORE_GetBinaryFileArch get the binary file arch
* @param szPath (IN) binary file path.
* @return -1-fail, >=-0 PE FIle: 0-unknown, 8-8bit, 16-16bit, 32-32bit, 64-64bit
*/
PUMP_CORE_API pump_int32_t PUMP_CALLBACK PUMP_CORE_GetBinaryFileArch(const char* szPath);

#endif // PUMP_CORE_FILE_H

