#include "pump_core/pump_core_string.h"
#include "pump_memory/smart_buffer/smart_buffer.h"
#include "pump_http/pump_http_request.h"
#include "pump_http/pump_http_def.h"

namespace Pump
{
namespace Http
{

const char *const
CHttpRequest::static_header_strings_[CHttpRequest::NUM_HEADER_STRINGS] =
{
    "Date",
    "Pragma",
    "Authorization",
    "From",
    "If-Modified-Since",
    "Referrer",
    "User-Agent",
    "Allow",
    "Content-Encoding",
    "Content-Length",
    "Content-Type",
    "Expires",
    "Last-Modified"
};

const char *const
CHttpRequest::static_method_strings_[CHttpRequest::NUM_METHOD_STRINGS] =
{
    "GET",
    "HEAD",
    "POST",
    "PUT"
};

// Implementation of class CHttpHeadersMapItem

CHttpHeadersMapItem::CHttpHeadersMapItem(void)
    : header_(0),
    value_(0)
{
}

CHttpHeadersMapItem::~CHttpHeadersMapItem(void)
{
    ::free((void *) this->header_);
    ::free((void *) this->value_);
    this->header_ = this->value_ = 0;
}

// CHttpHeadersMapItem::operator const char * (void) const
// {
//   return this->value_ == 0 ? this->no_value_ : this->value_;
// }

CHttpHeadersMapItem &
CHttpHeadersMapItem::operator= (char * value)
{
    ::free((void *) this->value_);
    this->value_ = ::strdup(value);
    return *this;
}

CHttpHeadersMapItem &
CHttpHeadersMapItem::operator= (const char * value)
{
    ::free((void *) this->value_);
    this->value_ = ::strdup(value);
    return *this;
}

CHttpHeadersMapItem &
CHttpHeadersMapItem::operator= (const CHttpHeadersMapItem & mi)
{
    ::free((void *) this->value_);
    ::free((void *) this->header_);
    this->header_ = ::strdup(mi.header_);
    this->value_ = (mi.value_ ? ::strdup(mi.value_) : 0);
    return *this;
}

const char *
CHttpHeadersMapItem::header(void) const
{
    return this->header_;
}

const char *
CHttpHeadersMapItem::value(void) const
{
    return this->value_;
}

// Implementation of class CHttpHeadersMap

CHttpHeadersMap::CHttpHeadersMap(void)
    : num_headers_(0)
{
}

CHttpHeadersMap::~CHttpHeadersMap(void)
{
}

CHttpHeadersMapItem &
CHttpHeadersMap::operator[] (const char * const header)
{
    CHttpHeadersMapItem *item_ptr;

    item_ptr = this->find(header);

    if (item_ptr == 0)
        item_ptr = this->place(header);

    return *item_ptr;
}

const CHttpHeadersMapItem &
CHttpHeadersMap::operator[] (const char * const header) const
{
    CHttpHeadersMapItem *item_ptr;
    CHttpHeadersMap *mutable_this = (CHttpHeadersMap *)this;

    item_ptr = this->find(header);

    if (item_ptr == 0)
        item_ptr = mutable_this->place(header);

    return *item_ptr;
}

int
CHttpHeadersMap::mapped(const char * const header) const
{
    int result = this->find(header) != 0;

    return result;
}

CHttpHeadersMapItem *
CHttpHeadersMap::find(const char * const header) const
{
    CHttpHeadersMap *const mutable_this = (CHttpHeadersMap *) this;

    mutable_this->garbage_.header_ = header;
#if 0
    CHttpHeadersMapItem *mi_ptr = (CHttpHeadersMapItem *)
        ::bsearch(&this->garbage_,
        this->map_,
        this->num_headers_,
        sizeof(CHttpHeadersMapItem),
        CHttpHeadersMap::compare);
#else
    int i = 0;
    int j = this->num_headers_;

    while (i < j - 1)
    {
        int k = (i + j) / 2;
        if (CHttpHeadersMap::compare(&this->garbage_, this->map_ + k) < 0)
            j = k;
        else
            i = k;
    }

    CHttpHeadersMapItem *mi_ptr = mutable_this->map_ + i;
    if (CHttpHeadersMap::compare(&this->garbage_, mi_ptr) != 0)
        mi_ptr = 0;
#endif

    mutable_this->garbage_.header_ = 0;

    return mi_ptr;
}

CHttpHeadersMapItem *
CHttpHeadersMap::place(const char *const header)
{
    this->garbage_.header_ = ::strdup(header);

    int i = this->num_headers_++;
    ::free((void *) this->map_[i].header_);
    ::free((void *) this->map_[i].value_);
    this->map_[i].header_ = 0;
    this->map_[i].value_ = 0;
    CHttpHeadersMapItem temp_item;

    while (i > 0)
    {
        if (CHttpHeadersMap::compare(&this->garbage_,
            &this->map_[i - 1]) > 0)
            break;

        this->map_[i].header_ = this->map_[i - 1].header_;
        this->map_[i].value_ = this->map_[i - 1].value_;
        this->map_[i - 1].header_ = 0;
        this->map_[i - 1].value_ = 0;

        i--;
    }

    this->map_[i].header_ = this->garbage_.header_;
    this->map_[i].value_ = this->garbage_.value_;

    this->garbage_.header_ = 0;

    return &this->map_[i];
}

int
CHttpHeadersMap::compare(const void *item1,
const void *item2)
{
    CHttpHeadersMapItem *a, *b;
    int result;

    a = (CHttpHeadersMapItem *)item1;
    b = (CHttpHeadersMapItem *)item2;

    if (a->header_ == 0 || b->header_ == 0)
    {
        if (a->header_ == 0 && b->header_ == 0)
            result = 0;
        else if (a->header_ == 0)
            result = 1;
        else
            result = -1;
    }
    else
        result = PUMP_CORE_StrCaseCmp(a->header_, b->header_);

    return (result < 0) ? -1 : (result > 0);
}

// Implementation of class CHttpHeaders

CHttpHeaders::CHttpHeaders(void) : done_(0)
{
}

CHttpHeaders::~CHttpHeaders(void)
{
}

void
CHttpHeaders::recognize(const char * const header)
{
    (void)this->map_[header];
}

void
CHttpHeaders::parse_header_line(char * const header_line)
{
    char *ptr = header_line;
    char *buf = header_line;
    int offset = 1;

    ptr = ::strchr(header_line, '\n');

    if (ptr > header_line && ptr[-1] == '\r')
    {
        ptr--;
        offset++;
    }

    if (ptr == header_line)
    {
        this->done_ = 1;
        return;
    }

    *ptr = '\0';
    ptr += offset;

    char *value = 0;
    char *header = ::PUMP_CORE_StrtOk_R(buf, ":", &value);

    //ACE_DEBUG((LM_DEBUG, " (%t) CHttpHeaders::parse_header_line [%s]\n",
    //    header ? header : "<empty>"));

    if (header != 0 && this->map_.mapped(header))
    {
        while (PUMP_CORE_CharIsSpace(*value))
            value++;

        this->map_[header] = value;

        //ACE_DEBUG((LM_DEBUG, " (%t) CHttpHeaders::parse_header_line <%s>\n",
        //    value ? value : "<empty>"));
    }

    // Write back the unused portion of the input.
    ::memmove(header_line, ptr, ::strlen(ptr) + 1);
}

int
CHttpHeaders::complete_header_line(char *const header_line)
{
    // Algorithm --
    // Scan for end of line marker.
    // If the next character is linear white space, then unfold the header.
    // Else, if the next character is printable, we have a complete header line.
    // Else, presumably the next character is '\0', so the header is incomplete.

    // return -1 if end of line but not complete header line
    // return 0 if no end of line marker
    // return > 0 if complete header line

    char *ptr = header_line;
    int offset;

    if (!this->end_of_line(ptr, offset))
        return 0;

    if (ptr == header_line)
    {
        ::memmove(ptr, ptr + offset, ::strlen(ptr + offset) + 1);
        this->done_ = 1;
        //ACE_DEBUG((LM_DEBUG, "  (%t) no more headers\n"));
        return 0;
    }

    do
    {
        switch (ptr[offset])
        {
        case ' ':
        case '\t':
            ::memmove(ptr, ptr + offset, ::strlen(ptr + offset) + 1);
            break;

        case '\n':
        case '\r':
            return (ptr - header_line);

        default:
            if (PUMP_CORE_CharIsAlpha(ptr[offset]))
                return (ptr - header_line);
            else
                return -1;
        }
    } while (this->end_of_line(ptr, offset) != 0);

    return 0;
}

int
CHttpHeaders::end_of_headers(void) const
{
    return this->done_;
}

CHttpHeadersMapItem &
CHttpHeaders::operator[] (const char * const header)
{
    return this->map_[header];
}

const CHttpHeadersMapItem &
CHttpHeaders::operator[] (const char * const header) const
{
    return this->map_[header];
}

int
CHttpHeaders::end_of_line(char *&line, int &offset) const
{
    char *old_line = line;
    char *ptr = ::strchr(old_line, '\n');

    if (ptr == 0)
        return 0;

    line = ptr;
    offset = 1;

    if (line > old_line
        && line[-1] == '\r')
    {
        line--;
        offset = 2;
    }

    return 1;
}

// For reasons of efficiency, this class expects buffer to be
// null-terminated, and buflen does NOT include the \0.

CHttpRequest::CHttpRequest(void)
    : got_request_line_(0),
    method_(0),
    uri_(0),
    version_(0),
    path_(0),
    cgi_(0),
    cgi_env_(0),
    cgi_args_(0),
    query_string_(0),
    path_info_(0),
    header_strings_(CHttpRequest::static_header_strings_),
    method_strings_(CHttpRequest::static_method_strings_)
    , data_(NULL)
    , datalen_(-1)
    , content_length_(-1)
    , filename_(NULL)
{

    for (size_t i = 0;
        i < CHttpRequest::NUM_HEADER_STRINGS;
        i++)
        this->headers_.recognize(this->header_strings_[i]);
}

CHttpRequest::~CHttpRequest(void)
{
    free(this->method_);
    free(this->uri_);
    free(this->version_);
    free(this->path_);
    free(this->query_string_);
    free(this->path_info_);

    delete[] this->cgi_env_;
}

pump_bool_t CHttpRequest::parse_header(::Pump::Memory::IoBuffer &mb)
{
    // Note that RFC 822 does not mention the maximum length of a header
    // line.  So in theory, there is no maximum length.

    // In Apache, they assume that each header line should not exceed
    // 8K.
    char* pBuffer = NULL, *pWork = NULL;
    // clone buffer
    pBuffer = new char[(unsigned int)mb.size()];
    memset(pBuffer, 0, (size_t)mb.size());
    if (mb.clone(pBuffer, (size_t)mb.size())<=0)
    {
        return PUMP_FALSE;
    }
    pWork = pBuffer;
    int header_len = 0;
    int result = this->headers_.complete_header_line(pWork);

    if (result != 0)
    {
        header_len += result;
        if (!this->got_request_line())
        {
            this->parse_request_line(pWork);
            while ((result = this->headers_.complete_header_line(pWork)) > 0)
            {
                header_len += result;
                this->headers_.parse_header_line(pWork);
            }
        }
        else if (result > 0)
        {
            do
            {
                header_len += result;
                this->headers_.parse_header_line(pWork);
            }
            while (this->headers_.complete_header_line(pWork) > 0);
        }
    }

    // erase header txt from buffer
    mb.erase(header_len);
    //mb.wr_ptr(strlen(pBuffer) - mb.length());
    //pWork = pBuffer + 
    if (this->headers_.end_of_headers()
        || (this->got_request_line() && this->version() == 0))
        // parser http body
        return PUMP_TRUE;
    else
        return PUMP_FALSE;
}

void
CHttpRequest::parse_request_line(char * request_line)
{
    char *ptr = request_line;
    char *buf = request_line;
    int offset = 1;

    this->status_ = HTTP_Status_Code::STATUS_OK;

    ptr = strchr(request_line, '\n');

    if (ptr > request_line && ptr[-1] == '\r')
        ptr--, offset++;

    if (ptr == request_line)
    {
        this->status_ = HTTP_Status_Code::STATUS_BAD_REQUEST;
        return;
    }

    *ptr = '\0';
    ptr += offset;

    char *lasts = 0; // for PUMP_CORE_StrtOk_R

    // Get the request type.
    this->got_request_line_ = 1;

    if (this->method(PUMP_CORE_StrtOk_R(buf, " \t", &lasts))
        && this->uri(PUMP_CORE_StrtOk_R(0, " \t", &lasts)))
    {
        this->type(this->method());

        if (this->version(PUMP_CORE_StrtOk_R(0, " \t", &lasts)) == 0
            && this->type() != CHttpRequest::GET)
            this->status_ = HTTP_Status_Code::STATUS_NOT_IMPLEMENTED;

        if (this->path(this->uri()) == 0)
            this->status_ = HTTP_Status_Code::STATUS_NOT_FOUND;
    }

    //ACE_DEBUG((LM_DEBUG, " (%t) request %s %s %s parsed\n",
    //    (this->method() ? this->method() : "-"),
    //    (this->uri() ? this->uri() : "="),
    //    (this->version() ? this->version() : "HTTP/0.9")));

    memmove(buf, ptr, strlen(ptr) + 1);
}

int
CHttpRequest::init(char *const buffer,
int buflen)
{
    // Initialize these every time.
    content_length_ = -1;

    // Extract the data pointer.
    data_ = buffer;
    datalen_ = 0;

    // Set the datalen
    if (data_ != 0)
        datalen_ = buflen;
    else
        datalen_ = 0;

    //ACE_DEBUG((LM_DEBUG, " (%t) init has initialized\n"));

    return 1;
}

const char *
CHttpRequest::method(void) const
{
    return this->method_;
}

const char *
CHttpRequest::uri(void) const
{
    return this->uri_;
}

const char *
CHttpRequest::version(void) const
{
    return this->version_;
}

const char *
CHttpRequest::path(void) const
{
    return this->path_;
}

int
CHttpRequest::cgi(void) const
{
    return this->cgi_;
}

const char **
CHttpRequest::cgi_env(void) const
{
    return (const char **)this->cgi_env_;
}

const char *
CHttpRequest::cgi_args(void) const
{
    return this->cgi_args_;
}

const char *
CHttpRequest::query_string(void) const
{
    return this->query_string_;
}

const char *
CHttpRequest::path_info(void) const
{
    return this->path_info_;
}

int
CHttpRequest::got_request_line(void) const
{
    return this->got_request_line_;
}

int
CHttpRequest::type(void) const
{
    return type_;
}

const CHttpHeaders &
CHttpRequest::headers(void) const
{
    return this->headers_;
}

const char *
CHttpRequest::header_strings(int index) const
{
    const char *hs = 0;

    if (0 <= index && index < NUM_HEADER_STRINGS)
        hs = this->header_strings_[index];

    return hs;
}

const char *
CHttpRequest::header_values(int index) const
{
    const char *hs = 0;
    const char *hv = 0;

    if (0 <= index && index < NUM_HEADER_STRINGS)
    {
        hs = this->header_strings_[index];
        hv = this->headers_[hs].value();
    }

    return hv;
}

char *
CHttpRequest::data(void)
{
    return data_;
}

int
CHttpRequest::data_length(void)
{
    return datalen_;
}

int
CHttpRequest::content_length(void)
{
    if (this->content_length_ == -1)
    {
        const char * clv = this->headers_["Content-Length"].value();
        this->content_length_ = (clv ? atoi(clv) : 0);
    }

    return this->content_length_;
}

int
CHttpRequest::status(void)
{
    return this->status_;
}

const char *
CHttpRequest::status_string(void)
{
    //return HTTP_Status_Code::instance()[this->status_];
    return NULL;
}

void
CHttpRequest::dump(void)
{
    //ACE_DEBUG((LM_DEBUG, "%s command.\n"
    //    "filename is %s,"
    //    " length of the file is %d,"
    //    " data string is %s,"
    //    " datalen is %d,"
    //    " status is %d, which is %s\n\n",
    //    this->method() ? this->method() : "EMPTY",
    //    this->uri() ? this->uri() : "EMPTY",
    //    this->content_length(),
    //    this->data() ? this->data() : "EMPTY",
    //    this->data_length(),
    //    this->status(),
    //    this->status_string()));
}

const char *
CHttpRequest::method(const char *method_string)
{
    if (this->method_)
        free(this->method_);

    if (method_string == 0)
    {
        this->status_ = HTTP_Status_Code::STATUS_BAD_REQUEST;
        this->method_ = 0;
    }
    else
        this->method_ = strdup(method_string);

    return this->method_;
}

const char *
CHttpRequest::uri(char *uri_string)
{
    if (this->uri_)
        free(this->uri_);

    if (uri_string == 0)
    {
        this->status_ = HTTP_Status_Code::STATUS_BAD_REQUEST;
        this->uri_ = 0;
    }
    else
    {
        this->uri_ = strdup(uri_string);
        this->cgi(this->uri_);
        //HTTP_Helper::HTTP_decode_string(this->uri_);
    }

    return this->uri_;
}

const char *
CHttpRequest::version(const char *version_string)
{
    if (this->version_)
        free(this->version_);

    if (version_string)
        this->version_ = strdup(version_string);
    else
        this->version_ = 0;

    return this->version_;
}

int
CHttpRequest::type(const char *type_string)
{
    this->type_ = CHttpRequest::NO_TYPE;

    if (type_string == 0)
        return this->type_;

    for (size_t i = 0;
        i < CHttpRequest::NUM_METHOD_STRINGS;
        i++)

        if (strcmp(type_string, this->method_strings_[i]) == 0)
        {
            this->type_ = i;
            break;
        }

    if (this->type_ == CHttpRequest::NO_TYPE)
        this->status_ = HTTP_Status_Code::STATUS_NOT_IMPLEMENTED;

    return this->type_;
}

int
CHttpRequest::cgi(char *uri_string)
{
    //this->cgi_ = 0;
    //this->cgi_env_ = 0;
    //this->cgi_args_ = 0;

    ////ACE_DEBUG((LM_DEBUG, " (%t) CHttpRequest::cgi (%s)\n", uri_string));

    //if (uri_string == 0 || strlen(uri_string) == 0)
    //    return 0;

    //// There are 2 cases where a file could be a CGI script
    ////
    //// (1) the file has a CGI extension.
    //// (2) the file resides in a CGI bin directory.

    //char *extra_path_info = 0;
    //if (this->cgi_in_path(uri_string, extra_path_info)
    //    || this->cgi_in_extension(uri_string, extra_path_info))
    //{
    //    cgi_args_and_env(extra_path_info);

    //    if (extra_path_info)
    //    {
    //        this->path_info_ = strdup(extra_path_info);
    //        HTTP_Helper::HTTP_decode_string(this->path_info_);
    //        *extra_path_info = '\0';
    //    }
    //}

    return this->cgi_;
}

int
CHttpRequest::cgi_in_path(char *uri_string, char *&extra_path_info)
{
    //char *cgi_path;

    ////ACE_DEBUG((LM_DEBUG, " (%t) CHttpRequest::cgi_in_path (%s)\n",
    //    uri_string));

    //if (HTTP_Config::instance()->cgi_path())
    //    cgi_path = strdup(HTTP_Config::instance()->cgi_path());
    //else
    //    cgi_path = strdup("");

    //// error checking considered helpful!
    //if (cgi_path == 0)
    //    return 0;

    //char *lasts = 0;
    //char *cgi_path_next = PUMP_CORE_StrtOk_R(cgi_path, ":", &lasts);

    //if (cgi_path_next)
    //    do
    //    {
    //        int len = strlen(cgi_path_next);

    //        // match path to cgi path
    //        int in_cgi_path = 0;

    //        if (*cgi_path_next == '/')
    //        {
    //            // cgi path next points to an ``absolute'' path
    //            extra_path_info = uri_string;
    //            in_cgi_path =
    //                (strncmp(extra_path_info, cgi_path_next, len) == 0);
    //        }
    //        else
    //        {
    //            // cgi path next points to a ``relative'' path
    //            extra_path_info = strstr(uri_string, cgi_path_next);
    //            in_cgi_path = (extra_path_info != 0);
    //        }

    //        if (in_cgi_path)
    //        {
    //            if (extra_path_info[len] == '/')
    //            {
    //                this->cgi_ = 1;
    //                extra_path_info += len;

    //                // move past the executable name
    //                do
    //                    extra_path_info++;
    //                while (*extra_path_info != '/'
    //                    && *extra_path_info != '?'
    //                    && *extra_path_info != '\0');

    //                if (*extra_path_info == '\0')
    //                    extra_path_info = 0;

    //                break;
    //            }
    //        }
    //        extra_path_info = 0;

    //        cgi_path_next = PUMP_CORE_StrtOk_R(0, ":", &lasts);
    //    } while (cgi_path_next);

    //    free(cgi_path);

        return this->cgi_;
}

int
CHttpRequest::cgi_in_extension(char *uri_string, char *&extra_path_info)
{
    //extra_path_info = strstr(uri_string, ".cgi");

    //ACE_DEBUG((LM_DEBUG, " (%t) CHttpRequest::cgi_in_extension (%s)\n",
    //    uri_string));

    //while (extra_path_info != 0)
    //{
    //    extra_path_info += 4;
    //    // skip past ``.cgi''

    //    switch (*extra_path_info)
    //    {
    //    case '\0':
    //        extra_path_info = 0;
    //        break;
    //    case '/':
    //    case '?':
    //        break;
    //    default:
    //        extra_path_info = strstr(extra_path_info, ".cgi");
    //        continue;
    //    }
    //    this->cgi_ = 1;
    //    break;
    //}

    return this->cgi_;
}

void
CHttpRequest::cgi_args_and_env(char *&extra_path_info)
{
    //char *cgi_question = 0;

    //if (extra_path_info)
    //    cgi_question = strchr(extra_path_info, '?');

    //if (extra_path_info == cgi_question)
    //    extra_path_info = 0;

    //if (cgi_question)
    //{
    //    *cgi_question++ = '\0';

    //    if (*cgi_question != '\0')
    //    {
    //        // We need the ``original'' QUERY_STRING for the
    //        // environment.  We will substitute '+'s for spaces in the
    //        // other copy.

    //        this->query_string_ = strdup(cgi_question);

    //        char *ptr = cgi_question;
    //        int count = 0;
    //        do
    //            if (*ptr == '+')
    //                *ptr = ' ';
    //            else if (*ptr == '&' || *ptr == '=')
    //                count++;
    //        while (*++ptr);

    //        count++;

    //        if (strchr(cgi_question, '='))
    //        {
    //            ACE_NEW(this->cgi_env_, char *[count + 1]);

    //            int i = 0;
    //            ptr = cgi_question;
    //            do
    //            {
    //                this->cgi_env_[i++] = ptr;

    //                while (*ptr++)
    //                    if (*ptr == '&' || *ptr == '=')
    //                        *ptr = '\0';

    //                HTTP_Helper::HTTP_decode_string(this->cgi_env_[i - 1]);
    //            } while (i < count);

    //            this->cgi_env_[count] = 0;
    //        }
    //        else
    //        {
    //            this->cgi_args_ = cgi_question;
    //            HTTP_Helper::HTTP_decode_string(cgi_question);
    //        }
    //    }
    //}
}

const char *
CHttpRequest::path(const char *uri_string)
{
//    char const *file_name = uri_string;
//    char buf[MAXPATHLEN + 1];
//    buf[0] = '\0';
//
//    if (file_name == 0) return 0;
//
//    if (*file_name == '/')
//    {
//        file_name++;
//        if (*file_name == '~')
//        {
//            char *ptr = buf;
//
//            while (*++file_name && *file_name != '/')
//                *ptr++ = *file_name;
//
//            *ptr = '\0';
//
//            if (ptr == buf)
//                strcpy(buf, getenv("HOME"));
//            else
//            {
//#if !defined (ACE_WIN32) && !defined (VXWORKS)
//                char pw_buf[BUFSIZ];
//                struct passwd pw_struct;
//                struct passwd *pw_struct_ptr;
//                if (getpwnam_r(buf, &pw_struct, pw_buf,
//                    sizeof(pw_buf), &pw_struct_ptr) == 0)
//                    return 0;
//                strcpy(buf, pw_struct.pw_dir);
//#endif /* NOT ACE_WIN32 AND NOT VXWORKS */
//            }
//
//            strcat(buf, "/");
//            strcat(buf, HTTP_Config::instance()->user_dir());
//            strcat(buf, file_name);
//        }
//        else
//        {
//            // With a starting '/' but no '~'
//            strcat(buf, HTTP_Config::instance()->document_root());
//            strcat(buf, file_name - 1);
//        }
//    }
//
//    if (*buf != '\0')
//        this->path_ = strdup(buf);
//
    return this->path_;
}

}
}