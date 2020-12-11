//
// Created by yz on 18-10-20.
//

#ifndef LIBPUMP_BLOCK_HPP
#define LIBPUMP_BLOCK_HPP

#include <utility>

#if (defined __linux__)
//#include <linux/shm.h> // 暂时不包含这个，需要链接库文件
#include <sys/time.h>
#elif (defined WIN32)
#include <time.h>
#endif // (defined __linux__)

#include "pump_core/pump_core_noncopyable.h"
#include "pump_core/logger/pump_core_logger.h"

//namespace nsp_std = ::std;
//using namespace Pump::Core;

namespace Pump {
namespace Memory {

        /**
        * @enum BlockType
        * @brief 内存块类型
        */
        enum BlockType {
            BLOCK_TYPE_DEFAULT = 0,   //! 默认内存
            BLOCK_TYPE_HEAP,          //! 进程内堆内存
            BLOCK_TYPE_SHM,           //! 进程间共享内存
        };

        /**
        * @enum RelativeType
        * @brief 描述内存区域间关系
        */
        enum RelativeType {
            RELATIVE_DEFAULT,   //! 默认关系，无法判断关系
            RELATIVE_INTERSECT, //! 相交，不包含
            RELATIVE_INCLUDE,   //! 真包含，非重叠
            RELATIVE_EXCLUDE,   //! 互斥
            RELATIVE_EQUAL,     //! 重叠
        };

        /**
        * @class tagBlock
        * @brief 内存描述块 \
        * 注：此对象的每一个实例描述一块内存区域，必是堆上对象，且保证进程内全局唯一性
        */
        class Block
            : public Pump::Core::CNonCopyable {
        public:
            size_t block_segsz_;        //! 段的大小（以字节为单位）
            size_t block_usesz_;        //! 有效占用大小（以字节为单位）
            const time_t block_ntime_;  //! 构造该段的时间
            time_t block_ctime_;        //! 最后一个进程修改该段的时间
            BlockType block_type_;      //! 内存块类型
            void *p_;                   //! 内存地址

            Block()
                : block_segsz_(0),
                block_ntime_(time(NULL)),
                block_ctime_(0),
                block_type_(BLOCK_TYPE_DEFAULT),
                p_(0) {}

            explicit Block(enum BlockType block_type)
                : block_segsz_(0),
                block_ntime_(time(NULL)),
                block_ctime_(0),
                block_type_(block_type),
                p_(0) {}

            //protected: // FIXME 应该是protected
        public:
            void reset() {
                block_segsz_ = (0);
                block_ctime_ = (0);
                p_ = (0);
            }

            template<typename T>
            T *r_get() {
                if (p_ == 0 || sizeof(T) > block_segsz_) {
                    // FIXME 抛出异常, 或者其他识别错误的方案
                    throw 1;
                }
                return (static_cast< T * >(p_));
            }

        public:
            template<typename T>
            T *get() const {
                if (p_ == 0 || sizeof(T) > block_segsz_) {
                    // FIXME 抛出异常, 或者其他识别错误的方案
                    throw 1;
                }
                return (static_cast< T * >(p_));
            }

            void *get() const {
                if (p_ == 0) {
                    // FIXME 抛出异常, 或者其他识别错误的方案
                    throw 1;
                }
                return (static_cast< void * >(p_));
            }

            template< typename T >
            T &ref() {
                return *(this->r_get< T >());
            }

            template< typename T >
            T &c_ref() const {
                return *(this->get< T >());
            }

            bool empty() {
                return (p_ == 0 || block_segsz_ == 0);
            }

            enum RelativeType relative(Block const *r) const {
                if (r == 0 || this == 0) {
                    return RELATIVE_DEFAULT;
                }
                if (p_ == r->p_
                    && (char *)p_ + block_segsz_ ==
                    (char *)r->p_ + r->block_segsz_) {
                    return RELATIVE_EQUAL;
                }
                else if ((p_ <= r->p_
                    && (char *)p_ + block_segsz_ >=
                    (char *)r->p_ + r->block_segsz_)
                    || (p_ >= r->p_
                        && (char *)p_ + block_segsz_ <=
                        (char *)r->p_ + r->block_segsz_)) {
                    return RELATIVE_INCLUDE;
                }
                else if (((char *)p_ + block_segsz_ < r->p_)
                    || ((char *)r->p_ + r->block_segsz_ > p_)) {
                    return RELATIVE_EXCLUDE;
                }
                else {
                    return RELATIVE_INTERSECT;
                }
            }

            virtual bool allocate(size_t n, const void *hint = 0) = 0;

            virtual void deallocate() = 0;
        };

        /**
        * @class tagHeap
        * @brief 进程内堆内存描述快
        */
        template<typename _Ax = std::allocator<char>, unsigned char _Value = 0>
        class Heap
            : public Block {
        public:
            typedef typename _Ax::template rebind<char>::other _Alty;
            _Alty a_;

            Heap() : Block(BLOCK_TYPE_HEAP) {}

            explicit Heap(_Ax &a) : Block(BLOCK_TYPE_HEAP), a_(a) {}

            virtual bool allocate(size_t n, const void *hint = 0) {
                try {
                    // 1)申请原始内存
                    p_ = (void *)a_.allocate(n, hint);
                }
                catch (std::bad_alloc e) {
                    PUMP_CORE_ERR ( "Heap allocate() failed");
                    p_ = 0;
                    return false;
                }
                // 2)按初始值填充内存,并初始化其他属性
                memset(p_, _Value, n);
                block_segsz_ = (n);
                block_ctime_ = (time(NULL));
                return true;
            }

            virtual void deallocate() {
                // 1)归还原始内存
                a_.deallocate((typename _Alty::pointer) p_, block_segsz_);
                // 2)还原属性值
                this->reset();
            }
        };

        template<typename _Ax = std::allocator<char> >
        class SHeapPolicyGuider;

        /**
        * @class BlockGuider [基类]
        * @brief 不包含申请Block内存的操作，因为进程内堆和共享内存申请方式不同，交给派生类实现 \
        *        提供操作原始指针的接口
        *  \a注：对Block有构造权限
        */
        class BlockGuider {
        protected:
            /**
            * @var m_px
            * @brief 内存描述块的地址 \
            *    \a本对象及其派生只允许构造不允许释放
            */
            Block *m_px;
        public:
            friend class SHeapPolicyGuider<>;

            BlockGuider() : m_px(0) {}

            BlockGuider(const BlockGuider &r) : m_px(r.m_px) {}

            virtual ~BlockGuider() {} // 不允许释放m_px，BlockGuider派生对象只拥有构造的权利

            size_t capacity() const {
                return (this->empty() ? 0 : m_px->block_segsz_);
            }

            size_t size() const {
                return (this->empty() ? 0 : m_px->block_usesz_);
            }

            template<class T>
            T *r_get() {
                return (this->empty() ? 0 : m_px->r_get<T>());
            }

            template<typename T>
            T *get() const {
                return (this->empty() ? 0 : m_px->get<T>());
            }

            void *get() const {
                return (this->empty() ? 0 : m_px->get());
            }

            template<typename T>
            T &ref() {
                if (this->empty()) {
                    PUMP_CORE_ERR ( "Block pointer is null");
                    throw 1;
                }
                return m_px->ref<T>();
            }

            template<typename T>
            T &c_ref() const {
                if (this->empty()) {
                    PUMP_CORE_ERR ( "Block pointer is null");
                    throw 1;
                }
                return m_px->c_ref<T>();
            }

            bool empty() const {
                return (m_px == 0);
            }

            enum RelativeType relative(BlockGuider const &r) const {
                return (this->empty() ? RELATIVE_DEFAULT : m_px->relative(r.m_px));
            }

            BlockGuider &operator=(const BlockGuider &r) {
                m_px = (r.m_px);
                return *this;
            }

            void swap(BlockGuider &r) {
                std::swap(m_px, r.m_px);
            }
        };

        template<typename _Ax = std::allocator<char> >
        class HeapGuider
            : public BlockGuider {
        public:
            typedef Heap<_Ax> type_block;
            typedef typename _Ax::template rebind<type_block>::other _Alty;
        protected:
            _Ax ra_;
            _Alty a_;
        public:
            HeapGuider() : BlockGuider() {}

            HeapGuider(const HeapGuider &r) : BlockGuider(r), ra_(r.a_), a_(r.a_) {}

            virtual ~HeapGuider() {}

            explicit HeapGuider(size_t n, _Ax &a)
                : BlockGuider(), ra_(a), a_(a) {
                if (n > 0) {
                    try {
                        m_px = (type_block *)a_.allocate(1);
                    }
                    catch (std::bad_alloc e) {
                        PUMP_CORE_ERR ( "Heap allocate() failed");
                        m_px = 0;
                    }
                    if (m_px != 0) {
                        a_.construct((type_block *)m_px, a);
                        m_px->allocate(n, m_px);
                    }
                }
            }

            explicit HeapGuider(size_t n) : BlockGuider(), ra_(), a_() {
                if (n > 0) {
                    try {
                        // 1)为内存描述块分配内存
                        m_px = (Block *)a_.allocate(1);
                    }
                    catch (std::bad_alloc e) {
                        PUMP_CORE_ERR("Heap allocate() failed");
                        m_px = 0;
                    }
                    if (m_px != 0) {
                        _Ax ta = _Ax(a_);
                        // 2)构造内存描述块 Heap< _Ax >
                        a_.construct(static_cast<type_block *>(m_px), ta);
                        // 3)分配原始内存
                        static_cast<type_block *>(m_px)->allocate(n, (void *)m_px);
                    }
                }
            }

            HeapGuider &operator=(const HeapGuider &r) {
                BlockGuider::operator=(r);
                a_ = (r.a_);
                return *this;
            }

            void swap(HeapGuider &r) {
                BlockGuider::swap(r);
                std::swap(a_, r.a_);
            }

            _Ax &get_alloc() {
                return ra_;
            }

            type_block *get_block_priv() { return (type_block *)m_px; }
        };

    }
}

#endif //LIBPUMP_BLOCK_HPP
