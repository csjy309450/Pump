#include "pump_core/logger/pump_core_logger.h"
#include "pump_core/file/pump_core_file.h"

int CheckMsvcCl12()
{
    const char* kMsvc12Dir = "C:/Program Files (x86)/Microsoft Visual Studio 12.0";
    const char* kmsvc12_cl_default = "/VC/bin/cl.exe";
    const char* kmsvc12_cl_x86_arm = "/VC/bin/x86_arm/cl.exe";
    const char* kmsvc12_cl_x86_amd64 = "/VC/bin/x86_amd64/cl.exe";

    std::string strBuff;
    pump_bool_t ret = PUMP_CORE_DirIsExist(kMsvc12Dir);
    if (!ret)
    {
        PUMP_CORE_ERR << "[" << kMsvc12Dir << "] isn't exist";
    }
    else
    {
        PUMP_CORE_INFO << "[" << kMsvc12Dir << "] is exist";
        
        strBuff = kMsvc12Dir;
        strBuff += kmsvc12_cl_default;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc12Dir << kmsvc12_cl_default << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc12Dir << kmsvc12_cl_default << "] is exist";
        }

        strBuff = kMsvc12Dir;
        strBuff += kmsvc12_cl_x86_arm;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc12Dir << kmsvc12_cl_x86_arm << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc12Dir << kmsvc12_cl_x86_arm << "] is exist";
        }

        strBuff = kMsvc12Dir;
        strBuff += kmsvc12_cl_x86_amd64;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc12Dir << kmsvc12_cl_x86_amd64 << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc12Dir << kmsvc12_cl_x86_amd64 << "] is exist";
        }
    }
    return 0;
}

int CheckMsvcCl14()
{
    const char* kMsvc14Dir = "C:/Program Files (x86)/Microsoft Visual Studio 14.0";
    const char* kmsvc14_cl_default = "/VC/bin/cl.exe";
    const char* kmsvc14_cl_x86_arm = "/VC/bin/x86_arm/cl.exe";
    const char* kmsvc14_cl_x86_amd64 = "/VC/bin/x86_amd64/cl.exe";

    std::string strBuff;
    pump_bool_t ret = PUMP_CORE_DirIsExist(kMsvc14Dir);
    if (!ret)
    {
        PUMP_CORE_ERR << "[" << kMsvc14Dir << "] isn't exist";
    }
    else
    {
        PUMP_CORE_INFO << "[" << kMsvc14Dir << "] is exist";

        strBuff = kMsvc14Dir;
        strBuff += kmsvc14_cl_default;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc14Dir << kmsvc14_cl_default << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc14Dir << kmsvc14_cl_default << "] is exist";
        }

        strBuff = kMsvc14Dir;
        strBuff += kmsvc14_cl_x86_arm;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc14Dir << kmsvc14_cl_x86_arm << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc14Dir << kmsvc14_cl_x86_arm << "] is exist";
        }

        strBuff = kMsvc14Dir;
        strBuff += kmsvc14_cl_x86_amd64;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc14Dir << kmsvc14_cl_x86_amd64 << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc14Dir << kmsvc14_cl_x86_amd64 << "] is exist";
        }
    }
    return 0;
}

int CheckMsvcNMake12()
{
    const char* kMsvc12Dir = "C:/Program Files (x86)/Microsoft Visual Studio 12.0";
    const char* kmsvc12_nmake_default = "/VC/bin/nmake.exe";
    const char* kmsvc12_nmake_amd64 = "/VC/bin/amd64/nmake.exe";

    std::string strBuff;
    pump_bool_t ret = PUMP_CORE_DirIsExist(kMsvc12Dir);
    if (!ret)
    {
        PUMP_CORE_ERR << "[" << kMsvc12Dir << "] isn't exist";
    }
    else
    {
        PUMP_CORE_INFO << "[" << kMsvc12Dir << "] is exist";

        strBuff = kMsvc12Dir;
        strBuff += kmsvc12_nmake_default;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc12Dir << kmsvc12_nmake_default << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc12Dir << kmsvc12_nmake_default << "] is exist";
        }

        strBuff = kMsvc12Dir;
        strBuff += kmsvc12_nmake_amd64;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc12Dir << kmsvc12_nmake_amd64 << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc12Dir << kmsvc12_nmake_amd64 << "] is exist";
        }
    }
    return 0;
}

int CheckMsvcNMake14()
{
    const char* kMsvc14Dir = "C:/Program Files (x86)/Microsoft Visual Studio 14.0";
    const char* kmsvc14_nmake_default = "/VC/bin/nmake.exe";
    const char* kmsvc14_nmake_amd64 = "/VC/bin/amd64/nmake.exe";

    std::string strBuff;
    pump_bool_t ret = PUMP_CORE_DirIsExist(kMsvc14Dir);
    if (!ret)
    {
        PUMP_CORE_ERR << "[" << kMsvc14Dir << "] isn't exist";
    }
    else
    {
        PUMP_CORE_INFO << "[" << kMsvc14Dir << "] is exist";

        strBuff = kMsvc14Dir;
        strBuff += kmsvc14_nmake_default;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc14Dir << kmsvc14_nmake_default << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc14Dir << kmsvc14_nmake_default << "] is exist";
        }

        strBuff = kMsvc14Dir;
        strBuff += kmsvc14_nmake_amd64;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvc14Dir << kmsvc14_nmake_amd64 << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvc14Dir << kmsvc14_nmake_amd64 << "] is exist";
        }
    }
    return 0;
}

int CheckMsvcMSBuild12()
{
    const char* kMsvcBuild12Dir = "C:/Program Files (x86)/MSBuild/12.0";
    const char* kmsvc12_msbuild_default = "/Bin/MSBuild.exe";

    std::string strBuff;
    pump_bool_t ret = PUMP_CORE_DirIsExist(kMsvcBuild12Dir);
    if (!ret)
    {
        PUMP_CORE_ERR << "[" << kMsvcBuild12Dir << "] isn't exist";
    }
    else
    {
        PUMP_CORE_INFO << "[" << kMsvcBuild12Dir << "] is exist";

        strBuff = kMsvcBuild12Dir;
        strBuff += kmsvc12_msbuild_default;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvcBuild12Dir << kmsvc12_msbuild_default << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvcBuild12Dir << kmsvc12_msbuild_default << "] is exist";
        }
    }
    return 0;
}

int CheckMsvcMSBuild14()
{
    const char* kMsvcBuild14Dir = "C:/Program Files (x86)/MSBuild/14.0";
    const char* kmsvc14_msbuild_default = "/Bin/MSBuild.exe";

    std::string strBuff;
    pump_bool_t ret = PUMP_CORE_DirIsExist(kMsvcBuild14Dir);
    if (!ret)
    {
        PUMP_CORE_ERR << "[" << kMsvcBuild14Dir << "] isn't exist";
    }
    else
    {
        PUMP_CORE_INFO << "[" << kMsvcBuild14Dir << "] is exist";

        strBuff = kMsvcBuild14Dir;
        strBuff += kmsvc14_msbuild_default;
        ret = PUMP_CORE_FileIsExist(strBuff.c_str());
        if (!ret)
        {
            PUMP_CORE_ERR << "[" << kMsvcBuild14Dir << kmsvc14_msbuild_default << "] isn't exist";
        }
        else
        {
            PUMP_CORE_INFO << "[" << kMsvcBuild14Dir << kmsvc14_msbuild_default << "] is exist";
        }
    }
    return 0;
}

int test_logger()
{
    PUMP_CORE_LOG_CONF struLogCong;
    memset(&struLogCong, 0, sizeof(struLogCong));
    struLogCong.szFilePath = "";
    struLogCong.emLogLevel = PUMP_LOG_INFO;
    PUMP_CORE_InitLogger(&struLogCong);
    PUMP_CORE_INFO << "-------test begin-------";
    return  0;
}

int test_BinaryFile()
{
    pump_int32_t ret = PUMP_CORE_GetBinaryFileArch("C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\cl.exe");
    PUMP_CORE_INFO << "[C:\\Program Files(x86)\\Microsoft Visual Studio 12.0\\VC\bin\\cl.exe] is " << ret << " bit exe";
    ret = PUMP_CORE_GetBinaryFileArch("C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\amd64\\cl.exe");
    PUMP_CORE_INFO << "[C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\bin\\amd64\\cl.exe] is " << ret << " bit exe";
    return 0;
}

int main(int argc, char** argv)
{
    test_logger();
    CheckMsvcCl12();
    CheckMsvcCl14();
    CheckMsvcNMake12();
    CheckMsvcNMake14();
    CheckMsvcMSBuild12();
    CheckMsvcMSBuild14();
    test_BinaryFile();
    return getchar();
}