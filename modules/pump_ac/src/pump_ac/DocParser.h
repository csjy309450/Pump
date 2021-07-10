/*!
* @file GlobalMacro.h
* @date 2019/12/16
*
* @author yangzheng
* Contact: yangzheng@hikvision.com.cn
*
* @brief xml/json����У�鶨���ļ�
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
* @group ��Ҫ��ӵ��ⲿͷ�ļ���
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
    const char* szMsg; ///< У�������Ϣ
    int iError; ///< У������룬�ο� NET_PROP_CTXT_ERRNO
    NET_PROP_DOC_TYPE emDocType; ///< ��������
    const void * pDoc;///< ���Ķ���emDocType Ϊ XML ʱΪ xmlDocPtr��JSON ʱΪ Json::Value*
    const void * pSchema; ///< shema����emDocType Ϊ XML ʱΪ xmlSchemaPtr��JSON ʱΪ Json::Value*
    const void * pErrNode; ///< У�����ڵ㣬emDocType Ϊ XML ʱΪ xmlDocPtr��JSON ʱΪ Json::Value*
} NET_PROP_CTXT_ERROR, *LPNET_PROP_CTXT_ERROR;

#define NET_PROP_SUCC 0 ///< �ӿڵ��óɹ���У��ͨ��
#define NET_PROP_FAIL -1///< �ӿڵ���ʧ�ܣ�ԭ���У�����
/*@}*/

/* У��ӿڷ���״̬�� */
typedef enum tagNET_PROP_CTXT_ERRNO
{
    NET_PROP_CTXT_NOERR = 0,
    /// ����ʧ�ܣ���ش�����
    NET_PROP_DOCP_FORMAT_INVALID, // ��Ч���ĸ�ʽ������jsonҲ��xml
    NET_PROP_SCHEMAP_XSD_INVALID,  // ��Чxsd��ʽ
    NET_PROP_SCHEMAP_JSD_INVALID, // ��Чjsd��ʽ
    /// У��ʧ�ܣ�XSD��ش��� [50, 99]
    NET_PROP_SCHEMAV_XSD_ELEMENT_UNEXPECTED = 50, ///< xmlԪ�ط�Ԥ�ڣ�������xml�ж����ȱ�ٽڵ㡢˳��ƥ��
    NET_PROP_SCHEMAV_XSD_ELEMENT_TYPE_INVALID, ///< Ԫ�����Ͳ�ƥ��
    NET_PROP_SCHEMAV_XSD_STR_MINLENGTH_INVALID, ///< �ַ���Ԫ�أ����Ȳ��� xs:minLength
    NET_PROP_SCHEMAV_XSD_STR_MAXLENGTH_INVALID, ///< �ַ���Ԫ�أ����ȳ��� xs:maxLength
    NET_PROP_SCHEMAV_XSD_STR_LENGTH_INVALID, ///< �ַ���Ԫ�أ����Ȳ�ƥ�� xs:length
    NET_PROP_SCHEMAV_XSD_STR_PATTERN_INVALID, ///< �ַ���Ԫ�أ�ͨ���ƥ��ʧ�� xs:pattern
    NET_PROP_SCHEMAV_XSD_NUM_MININCLUSIVE_INVALID, ///< ��ֵ��Ԫ�أ������������ xs:minInclusive
    NET_PROP_SCHEMAV_XSD_NUM_MAXINCLUSIVE_INVALID, ///< ��ֵ��Ԫ�أ��ұ��������� xs:maxInclusive
    NET_PROP_SCHEMAV_XSD_NUM_MINEXCLUSIVE_INVALID, ///< ��ֵ��Ԫ�أ����������� xs:minExclusive
    NET_PROP_SCHEMAV_XSD_NUM_MAXEXCLUSIVE_INVALID, ///< ��ֵ��Ԫ�أ��ҿ��������� xs:maxExclusive
    NET_PROP_SCHEMAV_XSD_ENUM_ENUMERATION_INVALID, ///< ö����Ԫ�أ���Чֵ
    NET_PROP_SCHEMAV_XSD_UNKNOWN,   ///< xsdУ��Ԥ�������
    /// У��ʧ�ܣ�JSD��ش���[100, 149]
    NET_PROP_SCHEMAV_JSD_ELEMENT_UNEXPECTED = 100, ///< jsonԪ�ط�Ԥ�ڣ������ǳ��ֶ�����Ԫ��
    NET_PROP_SCHEMAV_JSD_ELEMENT_TYPE_INVALID, ///< jsonԪ�����Ͳ�ƥ��
    NET_PROP_SCHEMAV_JSD_ELEMENT_DEPENDENCIES_INVALID, ///< jsonԪ��ȱ������Ԫ��
    NET_PROP_SCHEMAV_JSD_ELEMENT_REQUIRED_ITEM_MISSING, ///< jsonȱ�ٱ���Ԫ��
    NET_PROP_SCHEMAV_JSD_ARR_ITEM_INVALID, ///< ����Ԫ��������Ч
    NET_PROP_SCHEMAV_JSD_ARR_DUPLICATE_ITEMS, ///< ������Ԫ�أ���Ա�ظ���uniqueItems
    NET_PROP_SCHEMAV_JSD_ARR_MAX_ITEMS_INVALID, ///< ������Ԫ�أ���Ա������������
    NET_PROP_SCHEMAV_JSD_ARR_MIN_ITEMS_INVALID, ///< ������Ԫ�أ���Ա������������
    NET_PROP_SCHEMAV_JSD_ENUM_INVALID, ///< ö����Ԫ�أ���Чֵ
    NET_PROP_SCHEMAV_JSD_NUM_MAXIMUM_INVALID, /// ��ֵ��Ԫ�أ�����������
    NET_PROP_SCHEMAV_JSD_NUM_MINIMUM_INVALID, ///<  110 ��ֵ��Ԫ�أ�����������
    NET_PROP_SCHEMAV_JSD_NUM_EXCEED_INHERENT_LIMIT, ///< ��ֵ��Ԫ�أ�����������ֵ��Χ����999999999����int�����Χ
    NET_PROP_SCHEMAV_JSD_STR_PATTERN_INVALID, ///< �ַ���Ԫ�أ�ͨ���ƥ��ʧ��
    NET_PROP_SCHEMAV_JSD_STR_MINLENGTH_INVALID, ///< �ַ���Ԫ�أ����Ȳ���
    NET_PROP_SCHEMAV_JSD_STR_MAXLENGTH_INVALID, ///< �ַ���Ԫ�أ����ȳ���
    NET_PROP_SCHEMAV_JSD_STR_FORMAT_INVALID, ///< �ַ���Ԫ�أ���ƥ��format����ָ������
    NET_PROP_SCHEMAV_JSD_UNKNOWN,   ///< jsdУ��Ԥ�������
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
    CCtxtError m_objError; ///< ���ڻ���xml���������еĴ�����Ϣ
};

/*!
 * @class CDocParseCtxt
 * @brief xml/json �������ݽ�����
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
     * @brief ��ȡ�ṹ�����Ķ���ָ�룬������ʲô����GetBodyType()����ֵȷ��
     * @return �ṹ�����Ķ���ָ��
     */
    void* GetBodyObject();

    /**
     * @fn GetBodyType()
     * @brief ���ر������ͣ�xml/json
     * @return ��������
     */
    NET_PROP_DOC_TYPE GetBodyType() const;

private:
    CDocParseCtxt();
    CDocParseCtxt(const CDocParseCtxt&);
    CDocParseCtxt & operator=(const CDocParseCtxt &);
    /**
    * @fn Parse(const char* szProto, unsigned int dwSize);
    * @brief ��������
    * @param szProto [in] �������ݻ�����
    * @param dwSize [in] �������ݻ���������
    * @return �����ɹ���ʧ��
    */
    bool Parse(const char* szProto, unsigned int dwSize);

    static void XMLCALL XmlStructuredErrorCallback(void *ctx, const char *msg, ...);

private:
    NET_PROP_DOC_TYPE m_enumType; ///< �����ı�������
    Json::Value m_objJsonBody; ///< ����ṹ����Json����
    xmlDocPtr m_objXmlBody; ///< ����ṹ����xml����
};

/*!
 * @class CSchmaValidCtxt
 * @brief schemaУ����󣬷��̰߳�ȫ����������߳�ʹ��
 * @author yangzheng
 * @date 2020/01/03
 */
class CSchmaValidCtxt
    : public CCtxtBase
{
private:
    /*!
     * @class RealValidator
     * @brief �������е�schemaУ������װ
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
        std::string m_strErr; ///< ���ڻ���xml���������еĴ�����Ϣ

        RealValidator();
        ~RealValidator();
        void Set(xmlDocPtr pXsd);
        void Set(JsdValidator::JsonValidator * pJValidator);
        void Reset(); ///< ����У����
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