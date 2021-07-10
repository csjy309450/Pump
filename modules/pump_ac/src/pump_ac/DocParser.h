/*!
* @file GlobalMacro.h
* @date 2019/12/16
*
* @author yangzheng
* Contact: yangzheng@hikvision.com.cn
*
* @brief xml/json解析校验定义文件
*
* TODO: long description
*
* @note HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
*/
#ifndef NET_PROP_DOCPARSER_H
#define NET_PROP_DOCPARSER_H
#include "pump_ac/jsoncpp/json.h"
#include "libxml/tree.h"
#include "pump_ac/jsoncpp/validator.h"

struct _xmlSchemaParserCtxt;
typedef struct _xmlSchemaParserCtxt xmlSchemaParserCtxt;
typedef xmlSchemaParserCtxt *xmlSchemaParserCtxtPtr;
struct _xmlSchema;
typedef struct _xmlSchema xmlSchema;
typedef xmlSchema *xmlSchemaPtr;
struct _xmlSchemaValidCtxt;
typedef struct _xmlSchemaValidCtxt xmlSchemaValidCtxt;
typedef xmlSchemaValidCtxt *xmlSchemaValidCtxtPtr;

/*
* @group 需要添加到外部头文件中
*/
/*@{*/
typedef enum tagNET_PROP_DOC_TYPE
{
    NET_PROP_DOC_UNKNOW = 0,
    NET_PROP_DOC_XML,
    NET_PROP_DOC_JSON,
} NET_PROP_DOC_TYPE;

typedef void * NET_PROP_VALIDCTXT_H;

typedef struct tagNET_PROP_CTXT_ERROR
{
    const char* szMsg; ///< 校验错误信息
    int iError; ///< 校验错误码，参考 NET_PROP_CTXT_ERRNO
    NET_PROP_DOC_TYPE emDocType; ///< 报文类型
    const void * pDoc;///< 报文对象，emDocType 为 XML 时为 xmlDocPtr，JSON 时为 Json::Value*
    const void * pSchema; ///< shema对象，emDocType 为 XML 时为 xmlSchemaPtr，JSON 时为 Json::Value*
    const void * pErrNode; ///< 校验错误节点，emDocType 为 XML 时为 xmlDocPtr，JSON 时为 Json::Value*
} NET_PROP_CTXT_ERROR, *LPNET_PROP_CTXT_ERROR;

#define NET_PROP_SUCC 0 ///< 接口调用成功，校验通过
#define NET_PROP_FAIL -1///< 接口调用失败，原因非校验产生
/*@}*/

/* 校验接口返回状态码 */
typedef enum tagNET_PROP_CTXT_ERRNO
{
    NET_PROP_CTXT_NOERR = 0,
    /// 解析失败，相关错误码
    NET_PROP_DOCP_FORMAT_INVALID, // 无效报文格式，即非json也非xml
    NET_PROP_SCHEMAP_XSD_INVALID,  // 无效xsd格式
    NET_PROP_SCHEMAP_JSD_INVALID, // 无效jsd格式
    /// 校验失败，XSD相关错误 [50, 99]
    NET_PROP_SCHEMAV_XSD_ELEMENT_UNEXPECTED = 50, ///< xml元素非预期，可能是xml中多余或缺少节点、顺序不匹配
    NET_PROP_SCHEMAV_XSD_ELEMENT_TYPE_INVALID, ///< 元素类型不匹配
    NET_PROP_SCHEMAV_XSD_STR_MINLENGTH_INVALID, ///< 字符串元素，长度不足 xs:minLength
    NET_PROP_SCHEMAV_XSD_STR_MAXLENGTH_INVALID, ///< 字符串元素，长度超过 xs:maxLength
    NET_PROP_SCHEMAV_XSD_STR_LENGTH_INVALID, ///< 字符串元素，长度不匹配 xs:length
    NET_PROP_SCHEMAV_XSD_STR_PATTERN_INVALID, ///< 字符串元素，通配符匹配失败 xs:pattern
    NET_PROP_SCHEMAV_XSD_NUM_MININCLUSIVE_INVALID, ///< 数值型元素，左闭区间下溢 xs:minInclusive
    NET_PROP_SCHEMAV_XSD_NUM_MAXINCLUSIVE_INVALID, ///< 数值型元素，右闭区间上溢 xs:maxInclusive
    NET_PROP_SCHEMAV_XSD_NUM_MINEXCLUSIVE_INVALID, ///< 数值型元素，左开区间下溢 xs:minExclusive
    NET_PROP_SCHEMAV_XSD_NUM_MAXEXCLUSIVE_INVALID, ///< 数值型元素，右开区间上溢 xs:maxExclusive
    NET_PROP_SCHEMAV_XSD_ENUM_ENUMERATION_INVALID, ///< 枚举型元素，无效值
    NET_PROP_SCHEMAV_XSD_UNKNOWN,   ///< xsd校验预期外错误
    /// 校验失败，JSD相关错误[100, 149]
    NET_PROP_SCHEMAV_JSD_ELEMENT_UNEXPECTED = 100, ///< json元素非预期，可能是出现定义外元素
    NET_PROP_SCHEMAV_JSD_ELEMENT_TYPE_INVALID, ///< json元素类型不匹配
    NET_PROP_SCHEMAV_JSD_ELEMENT_DEPENDENCIES_INVALID, ///< json元素缺少依赖元素
    NET_PROP_SCHEMAV_JSD_ELEMENT_REQUIRED_ITEM_MISSING, ///< json缺少必填元素
    NET_PROP_SCHEMAV_JSD_ARR_ITEM_INVALID, ///< 数组元素类型无效
    NET_PROP_SCHEMAV_JSD_ARR_DUPLICATE_ITEMS, ///< 数组型元素，成员重复，uniqueItems
    NET_PROP_SCHEMAV_JSD_ARR_MAX_ITEMS_INVALID, ///< 数组型元素，成员数量超过上限
    NET_PROP_SCHEMAV_JSD_ARR_MIN_ITEMS_INVALID, ///< 数组型元素，成员数量不足下限
    NET_PROP_SCHEMAV_JSD_ENUM_INVALID, ///< 枚举型元素，无效值
    NET_PROP_SCHEMAV_JSD_NUM_MAXIMUM_INVALID, /// 数值型元素，右区间上溢
    NET_PROP_SCHEMAV_JSD_NUM_MINIMUM_INVALID, ///<  110 数值型元素，左区间下溢
    NET_PROP_SCHEMAV_JSD_NUM_EXCEED_INHERENT_LIMIT, ///< 数值型元素，超过固有数值范围，如999999999超过int的最大范围
    NET_PROP_SCHEMAV_JSD_STR_PATTERN_INVALID, ///< 字符串元素，通配符匹配失败
    NET_PROP_SCHEMAV_JSD_STR_MINLENGTH_INVALID, ///< 字符串元素，长度不足
    NET_PROP_SCHEMAV_JSD_STR_MAXLENGTH_INVALID, ///< 字符串元素，长度超过
    NET_PROP_SCHEMAV_JSD_STR_FORMAT_INVALID, ///< 字符串元素，不匹配format属性指定类型
    NET_PROP_SCHEMAV_JSD_UNKNOWN,   ///< jsd校验预期外错误
} NET_PROP_CTXT_ERRNO;

class CCtxtError
{
private:
    friend class CCtxtBase;
    friend class CSchmaValidCtxt;
    friend class CDocParseCtxt;
    CCtxtError();
    ~CCtxtError();
    CCtxtError(CCtxtError const & other);
    CCtxtError & operator=(CCtxtError const & other);
private:
    void Reset();
    CCtxtError & SetErrorStatus(std::string const & strMsg, int iError);
    CCtxtError & SetErrorNode(
        NET_PROP_DOC_TYPE emDocType,
        const void * pDoc,
        const void * pSchema,
        const void * pErrNode);
public:
    bool IsRset() const;
    const LPNET_PROP_CTXT_ERROR GetCtxtError();
    const char* GetMsg() const;
    int GetError() const;
    unsigned int  GetDocType() const;
    const void *  GetDocRoot();
    const void *  GetSchema();
    const void *  GetErrorNode();
    void addMsgPost(std::string const & strPost);
    void addMsgPre(std::string const & strPre);

    static int ConvertJsdError(int iInterErr);
    static int ConvertXsdError(int iInterErr);

private:
    bool m_bReset;
    std::string m_strMsg;
    NET_PROP_CTXT_ERROR m_struValidateMsg;
};

class CCtxtBase
{
public:
    CCtxtBase();
    ~CCtxtBase();
    const LPNET_PROP_CTXT_ERROR GetValidateMsg();
    CCtxtError & GetCtxtError();
protected:
    CCtxtError m_objError; ///< 用于缓存xml解析过程中的错误信息
};

/*!
 * @class CDocParseCtxt
 * @brief xml/json 报文数据解析器
 * @author yangzheng
 * @date 2019/11/27
 */
class CDocParseCtxt
    : public CCtxtBase
{
public:
    CDocParseCtxt(const char* szProto, unsigned int dwSize);
    ~CDocParseCtxt();

    /**
     * @fn GetBodyObject()
     * @brief 获取结构化报文对象指针，具体是什么更具GetBodyType()返回值确定
     * @return 结构化报文对象指针
     */
    void* GetBodyObject();

    /**
     * @fn GetBodyType()
     * @brief 返回报文类型，xml/json
     * @return 报文类型
     */
    NET_PROP_DOC_TYPE GetBodyType() const;

private:
    CDocParseCtxt();
    CDocParseCtxt(const CDocParseCtxt&);
    CDocParseCtxt & operator=(const CDocParseCtxt &);
    /**
    * @fn Parse(const char* szProto, unsigned int dwSize);
    * @brief 解析报文
    * @param szProto [in] 报文数据缓冲区
    * @param dwSize [in] 报文数据缓冲区长度
    * @return 解析成功或失败
    */
    bool Parse(const char* szProto, unsigned int dwSize);

    static void XMLCALL XmlStructuredErrorCallback(void *ctx, const char *msg, ...);

private:
    NET_PROP_DOC_TYPE m_enumType; ///< 解析的报文类型
    Json::Value m_objJsonBody; ///< 保存结构化的Json对象
    xmlDocPtr m_objXmlBody; ///< 保存结构化的xml对象
};

/*!
 * @class CSchmaValidCtxt
 * @brief schema校验对象，非线程安全，不允许跨线程使用
 * @author yangzheng
 * @date 2020/01/03
 */
class CSchmaValidCtxt
    : public CCtxtBase
{
private:
    /*!
     * @class RealValidator
     * @brief 三方库中的schema校验器封装
     * @author yangzheng
     * @date 2020/01/03
     */
    class RealValidator
    {
    public:
        union
        {
            struct  
            {
                JsdValidator::JsonValidator * pJsd;
            } vJsd;
            struct
            {
                xmlDocPtr pXsd;
            } vXsd;
        } m_unValidator;
        NET_PROP_DOC_TYPE m_emVType;
        std::string m_strErr; ///< 用于缓存xml解析过程中的错误信息

        RealValidator();
        ~RealValidator();
        void Set(xmlDocPtr pXsd);
        void Set(JsdValidator::JsonValidator * pJValidator);
        void Reset(); ///< 重置校验器
        static void XMLCALL XmlStructuredErrorCallback(void *ctx, const char *msg, ...);

    private:
        RealValidator(RealValidator const &);
        RealValidator & operator=(RealValidator const &);
    };
    
public:
    CSchmaValidCtxt();
    ~CSchmaValidCtxt();
    int Validate(CDocParseCtxt * objDoc, const char* szSchema, unsigned int dwSize);

private:
    void ResetValidator();

    int XsdValidate(CDocParseCtxt & objDoc, const char* szSchema, unsigned int dwSize);
    void HandleXsdValidateError(xmlErrorPtr pErr);
    static void XmlSchemaValidityErrorCallback(void *ctx, const char *msg, ...);

    int JsdValidate(CDocParseCtxt & objDoc, const char* szSchema, unsigned int dwSize);
    void HandleJsdValidateError(JsdValidator::JVException * pErr);

private:
    CDocParseCtxt * m_pDoc;
    RealValidator m_objRealValidator;
};

#endif //NET_PROP_DOCPARSER_H