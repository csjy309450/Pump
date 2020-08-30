#include "pump_function/pump_function.hpp"

namespace Pump
{
namespace Function
{
CFunctionBase::CFunctionBase()
    : m_bRet(true), m_nArgc(-1) {}

CFunctionBase::CFunctionBase(const __TrueType &bRet, unsigned char nArgc)
    : m_bRet(true), m_nArgc(nArgc) {}

CFunctionBase::CFunctionBase(const __FalseType &bRet, unsigned char nArgc)
    : m_bRet(false), m_nArgc(nArgc) {}

CFunctionBase::~CFunctionBase() {}

bool CFunctionBase::IsRet() const
{
    return m_bRet;
}

const unsigned char CFunctionBase::GetArgc() const { return m_nArgc; }

}
}