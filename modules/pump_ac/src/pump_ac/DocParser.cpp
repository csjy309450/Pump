/*!
* @file GlobalMacro.h
* @date 2019/12/16
*
* @author yangzheng
* Contact: yangzheng@hikvision.com.cn
*
* @brief xml/json解析校验实现文件
*
* TODO: long description
*
* @note HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
*/
#include <cstring>
#include <cassert>
#include "libxml/xmlerror.h"
#include "libxml/xmlreader.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "pump_ac/DocParser.h"
#include "pump_ac/jsoncpp/primitive_base.h"

using namespace JsdValidator;

CCtxtBase::CCtxtBase()
{
    
}

CCtxtBase::~CCtxtBase()
{
    
}

const LPNET_PROP_CTXT_ERROR CCtxtBase::GetValidateMsg()
{
    return m_objError.GetCtxtError();
}

CCtxtError & CCtxtBase::GetCtxtError()
{
    return m_objError;
}

CDocParseCtxt::CDocParseCtxt(const char* szProto, unsigned int dwSize)
:m_enumType(NET_PROP_DOC_UNKNOW),
m_objXmlBody(NULL)
{
    this->Parse(szProto, dwSize);
}

CDocParseCtxt::CDocParseCtxt()
:m_enumType(NET_PROP_DOC_UNKNOW),
m_objXmlBody(NULL)
{
}

CDocParseCtxt::~CDocParseCtxt()
{
    if (m_objXmlBody)
    {
        xmlFreeDoc(m_objXmlBody);
    }
}

void XMLCALL CDocParseCtxt::XmlStructuredErrorCallback(void *ctx, const char *msg, ...)
{
    CDocParseCtxt * pthis = static_cast<CDocParseCtxt *>(ctx);
    std::string strErr;
    va_list argv;
    va_start(argv, msg);
    //Vprintf(strErr, msg, argv);
    va_end(argv);
    pthis->m_objError.addMsgPost(strErr);
}

bool CDocParseCtxt::Parse(const char* szProto, unsigned int dwSize)
{
    // 清除错误码状态
    xmlResetLastError();
    m_objError.Reset();
    // 设置错误信息处理接口
    xmlSetGenericErrorFunc(this, XmlStructuredErrorCallback);
    // 解析报文
    xmlDocPtr pDoc = xmlReadMemory(szProto, dwSize, NULL, "utf8", XML_PARSE_RECOVER);
    if (pDoc) // 解析成功，说明缓冲区报文为xml报文
    {
        xmlErrorPtr pErr = xmlGetLastError();
        if (!pErr || pErr->code == XML_ERR_OK)
        {
            m_objXmlBody = pDoc;
            m_enumType = NET_PROP_DOC_XML;
            //NET_MCB_INFO("CDocParser::Parse xml parse succ");
            return true;
        }
    }
    m_objXmlBody = NULL;
    //NET_MCB_ERROR("CDocParser::Parse xml failed");

    // json报文探测
    try
    {
        Json::Reader objReader;
        if(objReader.parse(szProto, m_objJsonBody)) // 解析成功，说明缓冲区报文为json报文
        {
            m_enumType = NET_PROP_DOC_JSON;
            //NET_MCB_INFO("CDocParser::Parse json parse succ");
            return true;
        }
    }
    catch (...)
    {
        // DONE errno json解析异常
        //NET_MCB_ERROR("CDocParser::Parse json parse failed");
    }
    // DONE errno 未知类型报文格式
    //INTER_GetGlobalCtrl()->SetLastError(NET_PROP_DOCP_FORMAT_INVALID);
    //NET_MCB_ERROR("CDocParser::Parse body failed");
    return false;
}

void* CDocParseCtxt::GetBodyObject()
{
    switch (m_enumType)
    {
    case NET_PROP_DOC_JSON:
        return &m_objJsonBody;
        break;
    case NET_PROP_DOC_XML:
        return m_objXmlBody;
        break;
    default:
        break;
    }
    return NULL;
}

NET_PROP_DOC_TYPE CDocParseCtxt::GetBodyType() const
{
    return m_enumType;
}

CSchmaValidCtxt::RealValidator::RealValidator()
    :m_emVType(NET_PROP_DOC_UNKNOW)
{
    memset(&m_unValidator, 0, sizeof(m_unValidator));
}

CSchmaValidCtxt::RealValidator::RealValidator(RealValidator const &)
{}

CSchmaValidCtxt::RealValidator::~RealValidator()
{
    Reset();
}

CSchmaValidCtxt::RealValidator& CSchmaValidCtxt::RealValidator::operator=(RealValidator const&)
{
    return *this;
}

void CSchmaValidCtxt::RealValidator::Set(xmlDocPtr pXsd)
{
    m_emVType = (NET_PROP_DOC_XML);
    m_unValidator.vXsd.pXsd = (pXsd);
}

void CSchmaValidCtxt::RealValidator::Set(JsonValidator * pJValidator)
{
    m_emVType = (NET_PROP_DOC_JSON);
    m_unValidator.vJsd.pJsd = pJValidator;
}

void XMLCALL CSchmaValidCtxt::RealValidator::XmlStructuredErrorCallback(void *ctx, const char *msg, ...)
{
    CSchmaValidCtxt::RealValidator * pthis = static_cast<CSchmaValidCtxt::RealValidator *>(ctx);
    std::string strErr;
    va_list argv;
    va_start(argv, msg);
    //Vprintf(strErr, msg, argv);
    va_end(argv);
    pthis->m_strErr += strErr;
}

void CSchmaValidCtxt::RealValidator::Reset()
{
    switch (m_emVType)
    {
    case NET_PROP_DOC_XML:
        xmlFreeDoc(m_unValidator.vXsd.pXsd);
        break;
    case NET_PROP_DOC_JSON:
        delete m_unValidator.vJsd.pJsd;
        break;
    default:
        break;
    }
    m_emVType = NET_PROP_DOC_UNKNOW;
}

CSchmaValidCtxt::CSchmaValidCtxt()
    : m_pDoc(NULL)
{
    
}

CSchmaValidCtxt::~CSchmaValidCtxt()
{
    ResetValidator();
}

void CSchmaValidCtxt::ResetValidator()
{
    m_objError.Reset(); // 重置错误对象
    m_objRealValidator.Reset(); //重置校验器
    delete m_pDoc; //释放前一次的校验正文
}

int CSchmaValidCtxt::Validate(CDocParseCtxt * objDoc, const char* szSchema, unsigned int dwSize)
{
    if (!objDoc)
    {
        // DONE errno 未知类型报文格式
        //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_INVALIDE_PARAM);
        //NET_MCB_ERROR("CSchmaValidator::Validate objDoc mustn't NULL");
        return NET_PROP_FAIL;
    }
    if (objDoc->GetBodyType() == NET_PROP_DOC_UNKNOW)
    {
        //INTER_GetGlobalCtrl()->SetLastError(NET_PROP_DOCP_FORMAT_INVALID);
        //NET_MCB_ERROR("COM_PROP_Validate body parse failed");
        return NET_PROP_FAIL;
    }
    // 重置校验器状态，包括错误码，释放RealValidator
    ResetValidator();
    m_pDoc = objDoc;
    switch (objDoc->GetBodyType())
    {
    case NET_PROP_DOC_XML:
        return XsdValidate(*m_pDoc, szSchema, dwSize);
    case NET_PROP_DOC_JSON:
        return JsdValidate(*m_pDoc, szSchema, dwSize);
    default:
        break;
    }
    return NET_PROP_FAIL;
}

int CSchmaValidCtxt::XsdValidate(CDocParseCtxt & objDoc, const char* szSchema, unsigned int dwSize)
{
    // 获取xml body
    xmlDocPtr pDocXml = static_cast<xmlDocPtr>(objDoc.GetBodyObject());
    if (!pDocXml)
    {
        assert(0 == 1); // 理论上进不到这里
        return NET_PROP_FAIL;
    }
    xmlResetLastError();
    m_objRealValidator.m_strErr.clear();
    xmlSetGenericErrorFunc(this, RealValidator::XmlStructuredErrorCallback);
    xmlDocPtr pSchemaDoc = xmlReadMemory(szSchema, dwSize, NULL, "utf8", XML_PARSE_RECOVER);
    if (!pSchemaDoc) // 解析成功，说明缓冲区报文为xml报文
    {
        xmlErrorPtr pErr = xmlGetLastError();
        if (pErr && pErr->code != XML_ERR_OK)
        {
            //m_objError.SetErrorStatus()
            //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_ALLOC_RESOURCE_FAILD);
            //NET_MCB_ERROR("CSchmaValidator::XsdValidate xsd format invalide");
            return NET_PROP_FAIL;
        }
    }
    xmlSchemaParserCtxtPtr pXsdParser = xmlSchemaNewDocParserCtxt(pSchemaDoc);
    if (!pXsdParser)
    {
        // DONE xsd parser创建失败
        xmlFreeDoc(pSchemaDoc);
        //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_ALLOC_RESOURCE_FAILD);
        //NET_MCB_ERROR("CSchmaValidator::XsdValidat create xsd parser failed");
        return NET_PROP_FAIL;
    }

    xmlSchemaPtr pSchema = xmlSchemaParse(pXsdParser);
    if (!pSchema)
    {
        // TODO xsd解析失败错误，转换成校验业务错误
        //xmlErrorPtr err = xmlGetLastError();
        xmlSchemaFreeParserCtxt(pXsdParser);
        xmlFreeDoc(pSchemaDoc);
        //INTER_GetGlobalCtrl()->SetLastError(NET_PROP_SCHEMAP_XSD_INVALID);
        //NET_MCB_ERROR("CSchmaValidator::XsdValidat create xsd failed");
        xmlErrorPtr pError = xmlGetLastError();
        m_objError.SetErrorStatus(pError->message, NET_PROP_FAIL);
        return NET_PROP_FAIL;
    }
    xmlSchemaValidCtxtPtr pValidCtxt = xmlSchemaNewValidCtxt(pSchema);
    if (!pValidCtxt)
    {
        // DONE 创建 xsd Valid失败
        xmlSchemaFree(pSchema);
        xmlSchemaFreeParserCtxt(pXsdParser);
        xmlFreeDoc(pSchemaDoc);
        //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_ALLOC_RESOURCE_FAILD);
        //NET_MCB_ERROR("CSchmaValidator::XsdValidat create xsd validator failed");
        return NET_PROP_FAIL;
    }
    // 缓存校验资源
    m_objRealValidator.Set(pSchemaDoc);
    // 设置错误回调（同步回调）
    xmlSchemaSetValidErrors(pValidCtxt, CSchmaValidCtxt::XmlSchemaValidityErrorCallback, NULL, this);
    // 开始校验
    int ret = xmlSchemaValidateDoc(pValidCtxt, pDocXml);
    // 释放校验资源
    xmlSchemaFree(pSchema);
    xmlSchemaFreeParserCtxt(pXsdParser);
    xmlSchemaFreeValidCtxt(pValidCtxt);
    // 处理无错误
    if (ret == 0)
    {
        m_objError.SetErrorStatus("Validate succ", NET_PROP_SUCC);
        return NET_PROP_SUCC;
    }
    return NET_PROP_FAIL;
}

void CSchmaValidCtxt::XmlSchemaValidityErrorCallback(void *ctx, const char *msg, ...)
{
    CSchmaValidCtxt * pThis = (CSchmaValidCtxt *)ctx;
    if (!pThis->m_objError.IsRset())
    {
        return;
    }
    pThis->HandleXsdValidateError(xmlGetLastError());
}

void CSchmaValidCtxt::HandleXsdValidateError(xmlErrorPtr pErr)
{
    if (!pErr)
    {
        // 理论上不应进到此处
        assert(0 == 1);
        //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_UNKONWN);
        //NET_MCB_ERROR("CSchmaValidator::XsdValidat xmlSchemaValidateDoc failed but error unknown");
        return;
    }
    // 错误码转换
   
    m_objError.SetErrorStatus(pErr->message, CCtxtError::ConvertXsdError(pErr->code));
    m_objError.SetErrorNode(
        NET_PROP_DOC_XML,
        m_pDoc->GetBodyObject(),
        m_objRealValidator.m_unValidator.vXsd.pXsd,
        pErr->node);
}

int CSchmaValidCtxt::JsdValidate(CDocParseCtxt & objDoc, const char* szSchema, unsigned int dwSize)
{
    // 获取json body
    Json::Value * pDocjson = static_cast<Json::Value*>(objDoc.GetBodyObject());
    std::string strSchema(szSchema);
    JsdValidator::JsonValidator * pJValidator = new (std::nothrow) JsdValidator::JsonValidator();
    if (!pJValidator)
    {
        //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_ALLOC_RESOURCE_FAILD);
        //NET_MCB_ERROR("CSchmaValidator::JsdValidat create Schema Validor failed");
        return NET_PROP_FAIL;
    }
    m_objRealValidator.Set(pJValidator);
    try
    {
        pJValidator->parseSchema(strSchema);
    }
    catch (JsdValidator::JVException &e)
    {
        // TODO 与schema相关的错误码全部放到m_objError中
        //INTER_GetGlobalCtrl()->SetLastError(NET_PROP_SCHEMAP_JSD_INVALID);
        //NET_MCB_ERROR("CSchmaValidator::JsdValidat jsd parse failed");
        m_objError.SetErrorStatus(e.what(), NET_PROP_FAIL);
        return NET_PROP_FAIL;
    }

    // DONE: 此处JSD校验，返回的就是错误码
    try
    {
        int iError = pJValidator->validate(pDocjson);
    }
    catch (JsdValidator::JVException &e)
    {
        HandleJsdValidateError(&e);
        return NET_PROP_FAIL;
    }
    m_objError.SetErrorStatus("Validate succ", NET_PROP_SUCC);
    return NET_PROP_SUCC;
}

void CSchmaValidCtxt::HandleJsdValidateError(JsdValidator::JVException * pErr)
{
    if (!pErr)
    {
        // 理论上不应进到此处
        assert(0 == 1);
        //INTER_GetGlobalCtrl()->SetLastError(MCB_ERR_UNKONWN);
        //NET_MCB_ERROR("CSchmaValidator::HandleJsdValidateError jsonSchemaValidate failed but error unknown");
        return;
    }
    // 错误码转换
    m_objError.SetErrorStatus(pErr->what(), CCtxtError::ConvertJsdError(pErr->error()))
        .SetErrorNode(
        NET_PROP_DOC_JSON,
        m_pDoc->GetBodyObject(),
        m_objRealValidator.m_unValidator.vJsd.pJsd->getSchema(),
        pErr->node());
}

CCtxtError::CCtxtError()
    :m_bReset(false)
    , m_strMsg()
{
    memset(&m_struValidateMsg, 0, sizeof(m_struValidateMsg));
}

CCtxtError::~CCtxtError()
{
}

CCtxtError::CCtxtError(CCtxtError const & other)
    : m_bReset(false)
    , m_strMsg(other.m_strMsg)
    , m_struValidateMsg(other.m_struValidateMsg)
{
    m_struValidateMsg.szMsg = m_strMsg.c_str();
}

CCtxtError & CCtxtError::operator=(CCtxtError const & other)
{
    m_bReset = (false);
    m_strMsg = (other.m_strMsg);
    m_struValidateMsg = (other.m_struValidateMsg);
    m_struValidateMsg.szMsg = m_strMsg.c_str();
    return *this;
}

const LPNET_PROP_CTXT_ERROR CCtxtError::GetCtxtError()
{
    return &m_struValidateMsg;
}

bool CCtxtError::IsRset() const
{
    return m_bReset;
}

void CCtxtError::Reset()
{
    m_strMsg.clear();
    memset(&m_struValidateMsg, 0, sizeof(m_struValidateMsg));
    m_bReset = true;
}

CCtxtError & CCtxtError::SetErrorStatus(std::string const & strMsg, int iError)
{
    m_struValidateMsg.iError = iError;
    m_strMsg = (strMsg);
    m_struValidateMsg.szMsg = m_strMsg.c_str();
    m_bReset = false;
    return *this;
}

CCtxtError & CCtxtError::SetErrorNode(
    NET_PROP_DOC_TYPE emDocType,
    const void * pDoc,
    const void * pSchema,
    const void * pErrNode)
{
    m_struValidateMsg.emDocType = emDocType;
    m_struValidateMsg.pDoc = (pDoc);
    m_struValidateMsg.pSchema = pSchema;
    m_struValidateMsg.pErrNode = pErrNode;
    return *this;
}

const char* CCtxtError::GetMsg() const
{
    return m_strMsg.c_str();
}

int CCtxtError::GetError() const
{
    return m_struValidateMsg.iError;
}

unsigned int  CCtxtError::GetDocType() const
{
    return m_struValidateMsg.emDocType;
}
const void * CCtxtError::GetDocRoot()
{
    return m_struValidateMsg.pDoc;
}

const void * CCtxtError::GetSchema()
{
    return m_struValidateMsg.pSchema;
}

const void * CCtxtError::GetErrorNode()
{
    return m_struValidateMsg.pErrNode;
}

void CCtxtError::addMsgPost(std::string const & strPost)
{
    m_strMsg = strPost + m_strMsg;
}

void CCtxtError::addMsgPre(std::string const & strPre)
{
    m_strMsg = m_strMsg + strPre;
}

int CCtxtError::ConvertJsdError(int iInterErr)
{
    int iOuterErr = NET_PROP_SCHEMAV_JSD_UNKNOWN;
    switch (iInterErr)
    {
    case JVAL_ERR_DEPENDENCIES_INVALID:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ELEMENT_DEPENDENCIES_INVALID;
        break;
    case JVAL_ERR_NOT_AN_INTEGER:
    case JVAL_ERR_NOT_A_NUMBER:
    case JVAL_ERR_NOT_A_STRING:
    case JVAL_ERR_NOT_AN_ARRAY:
    case JVAL_ERR_NOT_AN_OBJECT:
    case JVAL_ERR_NOT_A_BOOLEAN:
    case JVAL_ERR_NOT_A_NULL:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ELEMENT_TYPE_INVALID;
        break;
    case JVAL_ERR_INVALID_PROPERTY:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ELEMENT_REQUIRED_ITEM_MISSING;
        break;
    case JVAL_ERR_REQUIRED_ITEM_MISSING:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ELEMENT_UNEXPECTED;
        break;
    case JVAL_ERR_INVALID_ARRAY_ITEM:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ARR_ITEM_INVALID;
        break;
    case JVAL_ERR_DUPLICATE_ITEMS:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ARR_DUPLICATE_ITEMS;
        break;
    case JVAL_ERR_INVALID_MAX_ITEMS:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ARR_MAX_ITEMS_INVALID;
        break;
    case JVAL_ERR_INVALID_MIN_ITEMS:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ARR_MIN_ITEMS_INVALID;
        break;
    case JVAL_ERR_ENUMERATION_INVALID:
        iOuterErr = NET_PROP_SCHEMAV_JSD_ENUM_INVALID;
        break;
    case JVAL_ERR_INVALID_MAXIMUM:
        iOuterErr = NET_PROP_SCHEMAV_JSD_NUM_MAXIMUM_INVALID;
        break;
    case JVAL_ERR_INVALID_MINIMUM:
        iOuterErr = NET_PROP_SCHEMAV_JSD_NUM_MINIMUM_INVALID;
        break;
    case JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT:
        iOuterErr = NET_PROP_SCHEMAV_JSD_NUM_EXCEED_INHERENT_LIMIT;
        break;
    case JVAL_ERR_FORMAT_MISMATCH:
        iOuterErr = NET_PROP_SCHEMAV_JSD_STR_FORMAT_INVALID;
        break;
    case JVAL_ERR_PATTERN_MISMATCH:
        iOuterErr = NET_PROP_SCHEMAV_JSD_STR_PATTERN_INVALID;
        break;
    case JVAL_ERR_INVALID_MAX_LENGTH:
        iOuterErr = NET_PROP_SCHEMAV_JSD_STR_MAXLENGTH_INVALID;
        break;
    case JVAL_ERR_INVALID_MIN_LENGTH:
        iOuterErr = NET_PROP_SCHEMAV_JSD_STR_MINLENGTH_INVALID;
        break;
    default:
        break;
    }
    return iOuterErr;
}

int CCtxtError::ConvertXsdError(int iInterErr)
{
    int iOuterErr = NET_PROP_SCHEMAV_XSD_UNKNOWN;
    switch (iInterErr)
    {
    case XML_SCHEMAV_CVC_DATATYPE_VALID_1_2_1:
    case XML_SCHEMAV_CVC_DATATYPE_VALID_1_2_2:
    case XML_SCHEMAV_CVC_DATATYPE_VALID_1_2_3:
        iOuterErr = NET_PROP_SCHEMAV_XSD_ELEMENT_TYPE_INVALID;
        break;
    case XML_SCHEMAV_ELEMENT_CONTENT:
        iOuterErr = NET_PROP_SCHEMAV_XSD_ELEMENT_UNEXPECTED;
        break;
    case XML_SCHEMAV_CVC_PATTERN_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_STR_PATTERN_INVALID;
        break;
    case XML_SCHEMAV_CVC_MAXLENGTH_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_STR_MAXLENGTH_INVALID;
        break;
    case XML_SCHEMAV_CVC_MINLENGTH_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_STR_MINLENGTH_INVALID;
        break;
    case XML_SCHEMAV_CVC_LENGTH_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_STR_LENGTH_INVALID;
        break;
    case XML_SCHEMAV_CVC_MININCLUSIVE_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_NUM_MININCLUSIVE_INVALID;
        break;
    case XML_SCHEMAV_CVC_MAXINCLUSIVE_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_NUM_MAXINCLUSIVE_INVALID;
        break;
    case XML_SCHEMAV_CVC_MINEXCLUSIVE_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_NUM_MINEXCLUSIVE_INVALID;
        break;
    case XML_SCHEMAV_CVC_MAXEXCLUSIVE_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_NUM_MAXEXCLUSIVE_INVALID;
        break;
    case XML_SCHEMAV_CVC_ENUMERATION_VALID:
        iOuterErr = NET_PROP_SCHEMAV_XSD_ENUM_ENUMERATION_INVALID;
        break;
    default:
        iOuterErr = NET_PROP_SCHEMAV_XSD_UNKNOWN;
    }
    return iOuterErr;
}