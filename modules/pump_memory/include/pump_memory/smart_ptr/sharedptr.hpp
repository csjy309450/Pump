//
// Created by yz on 18-10-28.
//

#ifndef LIBPUMP_SHAREDPTR_HPP
#define LIBPUMP_SHAREDPTR_HPP

#include "voidsptr.hpp"
#include "pump_memory/smart_ptr/voidwptr.hpp"

namespace nsp_std = std;

namespace Pump {
namespace Memory {

        enum SpState {
            SP_STATE_NULL,   //! ��ָ��
            SP_STATE_NEW,    //! ����ռ�
            SP_STATE_INIT,   //! �ѹ���
        };

        template<class T>
        class SharedPtr
            : public VoidSPtr {
        protected:
            typedef SharedPtr<T> this_type;

            template<class Y> friend class WeakPtr;
        public:

            typedef typename SP_Element<T>::ElmType element_type;

            SharedPtr()
                : VoidSPtr(),
                m_state(SP_STATE_NULL) {
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO << "SharedPtr()";
#endif //_TEST_LEVEL_INFO
            }

//#ifndef BOOST_NO_CXX11_NULLPTR
//
//            SharedPtr(boost::detail::sp_nullptr_t/*ռλ����*/)
//                : VoidSPtr(sizeof(element_type)),
//                m_state(SP_STATE_NEW) {
//#ifdef _TEST_LEVEL_INFO
//                PUMP_CORE_INFO << "SharedPtr()";
//#endif //_TEST_LEVEL_INFO
//            }
//
//#endif //BOOST_NO_CXX11_NULLPTR

            SharedPtr(const NullPtr &/*ռλ����*/)
                : VoidSPtr(sizeof(element_type)),
                m_state(SP_STATE_NEW) {
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO ( "SharedPtr()");
#endif //_TEST_LEVEL_INFO
            }

            explicit SharedPtr(const VoidSPtr &r)
                : VoidSPtr(r),
                m_state(SP_STATE_NEW) {
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO ( "SharedPtr()");
#endif //_TEST_LEVEL_INFO
            }

            template<class A>
            SharedPtr(A &a, NullPtr/*ռλ����*/)
                : VoidSPtr(a, sizeof(element_type)),
                m_state(SP_STATE_NEW) {
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO ( "SharedPtr()");
#endif //_TEST_LEVEL_INFO
            }

            template<class A, class D>
            SharedPtr(A &a, D d)
                : VoidSPtr(a, sizeof(element_type), d),
                m_state(SP_STATE_NEW) {
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO ( "SharedPtr()");
#endif //_TEST_LEVEL_INFO
            }

            SharedPtr(SharedPtr const &r)
                : VoidSPtr(static_cast<VoidSPtr const &>(r)),
                m_state(r.m_state) {
#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO ( "SharedPtr()");
#endif //_TEST_LEVEL_INFO
            }

            virtual ~SharedPtr() {
                // FIXME [fixed] ��Ӧ��Ĭ�ϵ��ö�������������������������ŵ�SharedCount�е��ã�����߳�ͬ������
                /**
                * @bug [fixed] ���߳��£���α�֤ this->use_count() �� this->destroy() �����ڼ� \
                *      û�������̹߳�����һ��SharedPtr�����Ǵ��ڼ������̹߳�����һ��SharedPtr���²�һ��
                * @brief �������������ֻ���ڳɹ����ù��캯���Ժ�Ż��ڶ�������ǰ���ã����Ϊ�������̰߳�ȫ���뽫 \
                *        �����������÷����ڴ�����ǰһ�̣��Ա���������ٵ�һ�룬�ֱ������߳�������ɲ�һ�µ����� \
                *        ����ڹ��캯�����óɹ���ע��˽�����������������ֶ�������ע��˽��������������������
                */
                // ֻ�е���Ҫ�ͷ���Դʱ�ŵ�����������
                //    if (*this != nullptr
                //        && m_state == SP_STATE_INIT) {
                //      this->destroy();
                //    }

#ifdef _TEST_LEVEL_INFO
                PUMP_CORE_INFO ( "~SharedPtr()");
#endif //_TEST_LEVEL_INFO
            }

            template<class Y>
            SharedPtr &operator=(SharedPtr<Y> const &r) {
                VoidSPtr::operator=(static_cast<VoidSPtr const &>(r));
                m_state = r.state();
                return *this;
            }

            SharedPtr &operator=(SharedPtr const &r) {
                VoidSPtr::operator=(static_cast<VoidSPtr const &>(r));
                m_state = r.state();
                return *this;
            }

            template<class... Args>
            void construct(Args &&...args) {
                if ((*this != PUMP_NULLPTR)
                    && (m_state == SP_STATE_NEW)) {
                    ::new(this->r_get<element_type>())element_type(std::forward<Args>(args)...);
                    m_state = SP_STATE_INIT;
                    // ����ɹ���ע��˽����������
                    m_policy.fn_destruct().Bind(&SharedPtr::destroy_private);
                }
            }

            void destroy() {
                if (typeid(element_type) != typeid(void)) {
                    this->r_get<element_type>()->~element_type();
                    m_state = SP_STATE_NEW;
                    // �ֶ�������ע��˽�����������������������
                    m_policy.fn_destruct().Clear();
#ifdef _TEST_LEVEL_INFO
                    PUMP_CORE_INFO << "destroy";
#endif //_TEST_LEVEL_INFO
                }
            }

        protected:
            /**
            * @fn destroy_private
            * @param pself �й��ڴ�ָ��
            * @return void.
            * @brief Ϊ��ʵ���ͷ��й��ڴ�
            */
            static void destroy_private(void * pself) {
                element_type * self = (element_type *)pself;
                if (typeid(element_type) != typeid(void)) {
                    self->~element_type();
#ifdef _TEST_LEVEL_INFO
                    PUMP_CORE_INFO ( "destroy_private" );
#endif //_TEST_LEVEL_INFO
                }
            }

        public:
            const element_type &operator*() const {
                return *(VoidSPtr::get<element_type>());
            }

            VoidSPtr operator&() const {
                return static_cast<VoidSPtr>(*this);
            }

            element_type *operator->() const {
                if (m_state != SP_STATE_INIT) {
                    // FIXME ���ô�����
                    PUMP_CORE_ERR ( "Object not constructed.");
                    throw 0;
                }
                return (VoidSPtr::get<element_type>());
            }

            virtual void reset()  {
                if (*this == PUMP_NULLPTR) {
                    return;
                }
                //    this->destroy();
                VoidSPtr::reset();
                m_state = SP_STATE_NULL;
            }

            /**
            * @fn ref() [���ý׶�]
            * @return element_type
            * @brief ����
            */
            element_type & ref() {
                if (m_state != SP_STATE_INIT) {
                    // FIXME ���ô�����
                    PUMP_CORE_ERR ( "Object not constructed.");
                    throw 0;
                }
                return *VoidSPtr::r_get<element_type>();
            }

            enum SpState state() const {
                return m_state;
            }

        protected:
            enum SpState m_state; //! ָ����������
        };

    }
}
#endif //LIBPUMP_SHAREDPTR_HPP
