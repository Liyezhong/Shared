///////////////////////////////////////////////////////////
//  MemLeakDetection.h
//  Implementation of the Class MemLeakDetection
//  Created on:      29-Sep-2009 07:08:44
//  Original author: Hans-Joerg Gruenthal
///////////////////////////////////////////////////////////

#if !defined(EA_B858CDC6_1259_4815_9247_63414D675826__INCLUDED_)
#define EA_B858CDC6_1259_4815_9247_63414D675826__INCLUDED_

#ifndef QT_NO_DEBUG

#include <stddef.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef align32
#define align32(value) ((((value) + 3) >> 2) << 2)
#endif

/**
* \brief mememory leak detection
*/
class MemLeakDetection
{

public:
    /****************************************************************************/
    /**
     * \brief Construction of MemLeakDetection.
     */
    /****************************************************************************/
    MemLeakDetection();

    /****************************************************************************/
    /**
     * \brief destruction of MemLeakDetection.
     */
    /****************************************************************************/
    ~MemLeakDetection();

    /****************************************************************************/
    /**
     * \brief allocate memory.
     *
     * \iparam size The size to be allocated.
     * \iparam ptr Pointer to memory.
     * \return return allocated memory.
     */
    /****************************************************************************/
    void* allocate(size_t size, void* ptr = NULL);

    /**
     * \brief release memory.
     * \iparam ptr pointer to release.
    */
    void deallocate(const void* ptr);

    /**
     * \brief get instance.
     * \return class instance
    */
    static MemLeakDetection& getInstance();

    /**
      * \brief element enum
    */
    enum Elements
    {
        Element0Byte, ///< 0Byte
        Element4Byte, ///< 4Byte
        Element8Byte, ///< 8Byte
        Element16Byte, ///< 16Byte
        Element32Byte, ///< 32Byte
        Element64Byte, ///< 64Byte
        Element128Byte, ///< 128Byte
        Element256Byte, ///< 256Byte
        Element512Byte, ///< 512Byte
        Element1024Byte, ///< 1024Byte
        Element2048Byte, ///< 2048Byte
        Element4096Byte, ///< 4096Byte
        Element8192Byte, ///< 8192Byte
        Element16384Byte, ///< 16384Byte
        Element32768Byte, ///< 32768Byte
        Element65536Byte, ///< 65536Byte
        Element131072Byte, ///< 131072Byte
        Element262144Byte, ///< 262144Byte
        Element524288Byte, ///< 524288Byte
        Element1048576Byte, ///< 1048576Byte
        Element5242880Byte, ///< 5242880Byte
        ElementCount ///< element count
    };

    /**
      * \brief element number enum
    */
    enum ElementNo
    {
        ElementNo0Byte       =  4000, ///< ElementNo0Byte
        ElementNo4Byte       =  1024, ///< ElementNo4Byte
        ElementNo8Byte       =  4000, ///< ElementNo8Byte
        ElementNo16Byte      =  6000, ///< ElementNo16Byte
        ElementNo32Byte      = 10000, ///< ElementNo32Byte
        ElementNo64Byte      = 12000, ///< ElementNo64Byte
        ElementNo128Byte     = 20000, ///< ElementNo128Byte
        ElementNo256Byte     =  8000, ///< ElementNo256Byte
        ElementNo512Byte     =  4000, ///< ElementNo512Byte
        ElementNo1024Byte    =  4000, ///< ElementNo1024Byte
        ElementNo2048Byte    = 20000, ///< ElementNo2048Byte
        ElementNo4096Byte    =   200, ///< ElementNo4096Byte
        ElementNo8192Byte    =    20, ///< ElementNo8192Byte
        ElementNo16384Byte   =    32, ///< ElementNo16384Byte
        ElementNo32768Byte   =    10, ///< ElementNo32768Byte
        ElementNo65536Byte   =     6, ///< ElementNo65536Byte
        ElementNo131072Byte  =     6, ///< ElementNo131072Byte
        ElementNo262144Byte  =     6, ///< ElementNo262144Byte
        ElementNo524288Byte  =     6, ///< ElementNo524288Byte
        ElementNo1048576Byte =     3, ///< ElementNo1048576Byte
        ElementNo5242880Byte =     4 ///< ElementNo5242880Byte
    };

    /**
      * \brief element size enum
    */
    enum ElementSize
    {
        ElementSize0Byte       =       sizeof(void*), ///< ElementSize0Byte
        ElementSize4Byte       =       4, ///< ElementSize4Byte
        ElementSize8Byte       =       8, ///< ElementSize8Byte
        ElementSize16Byte      =      16, ///< ElementSize16Byte
        ElementSize32Byte      =      32, ///< ElementSize32Byte
        ElementSize64Byte      =      64, ///< ElementSize64Byte
        ElementSize128Byte     =     128, ///< ElementSize128Byte
        ElementSize256Byte     =     256, ///< ElementSize256Byte
        ElementSize512Byte     =     512, ///< ElementSize512Byte
        ElementSize1024Byte    =    1024, ///< ElementSize1024Byte
        ElementSize2048Byte    =    2048, ///< ElementSize2048Byte
        ElementSize4096Byte    =    4096, ///< ElementSize4096Byte
        ElementSize8192Byte    =    8192, ///< ElementSize8192Byte
        ElementSize16384Byte   =   16384, ///< ElementSize16384Byte
        ElementSize32768Byte   =   32768, ///< ElementSize32768Byte
        ElementSize65536Byte   =   65536, ///< ElementSize65536Byte
        ElementSize131072Byte  =  131072, ///< ElementSize131072Byte
        ElementSize262144Byte  =  262144, ///< ElementSize262144Byte
        ElementSize524288Byte  =  524288, ///< ElementSize524288Byte
        ElementSize1048576Byte = 1048576, ///< ElementSize1048576Byte
        ElementSize5242880Byte = 5242880 ///< ElementSize5242880Byte
    };

    /**
      * \brief stack entry
    */
    struct StackEntry
    {
        //        void * instruction;
        unsigned int instruction; ///< instruction definition
    };

    /**
      * \brief constants enum
    */
    enum Constants
    {
        StackEntries   = 16, ///< stack entries
        ElementHeaderSize = align32 (sizeof (bool)) + StackEntries * sizeof (StackEntry), ///< header size
        MemSize0       = (ElementHeaderSize + ElementSize0Byte)       * ElementNo0Byte, ///< MemSize0
        MemSize4       = (ElementHeaderSize + ElementSize4Byte)       * ElementNo4Byte, ///< MemSize4
        MemSize8       = (ElementHeaderSize + ElementSize8Byte)       * ElementNo8Byte, ///< MemSize8
        MemSize16      = (ElementHeaderSize + ElementSize16Byte)      * ElementNo16Byte, ///< MemSize16
        MemSize32      = (ElementHeaderSize + ElementSize32Byte)      * ElementNo32Byte, ///< MemSize32
        MemSize64      = (ElementHeaderSize + ElementSize64Byte)      * ElementNo64Byte, ///< MemSize64
        MemSize128     = (ElementHeaderSize + ElementSize128Byte)     * ElementNo128Byte, ///< MemSize128
        MemSize256     = (ElementHeaderSize + ElementSize256Byte)     * ElementNo256Byte, ///< MemSize256
        MemSize512     = (ElementHeaderSize + ElementSize512Byte)     * ElementNo512Byte, ///< MemSize512
        MemSize1024    = (ElementHeaderSize + ElementSize1024Byte)    * ElementNo1024Byte, ///< MemSize1024
        MemSize2048    = (ElementHeaderSize + ElementSize2048Byte)    * ElementNo2048Byte, ///< MemSize2048
        MemSize4096    = (ElementHeaderSize + ElementSize4096Byte)    * ElementNo4096Byte, ///< MemSize4096
        MemSize8192    = (ElementHeaderSize + ElementSize8192Byte)    * ElementNo8192Byte, ///< MemSize8192
        MemSize16384   = (ElementHeaderSize + ElementSize16384Byte)   * ElementNo16384Byte, ///< MemSize16384
        MemSize32768   = (ElementHeaderSize + ElementSize32768Byte)   * ElementNo32768Byte, ///< MemSize32768
        MemSize65536   = (ElementHeaderSize + ElementSize65536Byte)   * ElementNo65536Byte, ///< MemSize65536
        MemSize131072  = (ElementHeaderSize + ElementSize131072Byte)  * ElementNo131072Byte, ///< MemSize131072
        MemSize262144  = (ElementHeaderSize + ElementSize262144Byte)  * ElementNo262144Byte, ///< MemSize262144
        MemSize524288  = (ElementHeaderSize + ElementSize524288Byte)  * ElementNo524288Byte, ///< MemSize524288
        MemSize1048576 = (ElementHeaderSize + ElementSize1048576Byte) * ElementNo1048576Byte, ///< MemSize1048576
        MemSize5242880 = (ElementHeaderSize + ElementSize5242880Byte) * ElementNo5242880Byte, ///< MemSize5242880
        MemSize        = MemSize0 + MemSize4 + MemSize8 + MemSize16 + MemSize32 + MemSize64 + MemSize128 + MemSize256 + MemSize512
        +    MemSize1024 + MemSize2048 +    MemSize4096 + MemSize8192 + MemSize16384 + MemSize32768 + MemSize65536 + MemSize131072 + MemSize262144 + MemSize524288
        + MemSize1048576 + MemSize5242880  ///< MemSize
    };
private:
    class MemLog
    {
    public:
        /**
          * construction
        */
        MemLog();
        /**
         * \brief destruction
        */
        ~MemLog();

        /**
          * \brief do log
          * \iparam ptr ptr
          * \iparam size size
          * \iparam allocated allocated
        */
        static void doLog(const void *ptr, size_t size, bool allocated);

        /**
          * \brief do log conflict
          * \iparam ptr ptr
          * \iparam size size
          * \iparam comment comment
        */
        static void doLogConflict (const void *ptr, size_t size, const char *comment);

      private:
        /**
          * \brief flush
        */
        void flush();
        /**
          * \brief log function
          * \iparam ptr ptr
          * \iparam size size
          * \iparam allocated allocated
        */
        void log(const void *ptr, size_t size, bool allocated);

        /**
          * \brief log conflict
          * \iparam ptr ptr
          * \iparam size size
          * \iparam comment comment
        */
        void logConflict (const void *ptr, size_t size, const char *comment);

        char         m_buffer[ElementSize32768Byte]; ///< buffer
        char         m_fileName[MAX_PATH]; ///< file name
        unsigned int m_position; ///< position
        void        *m_criticalSection; ///< critical section

        static MemLog *s_instance; ///< log instance
    };

    struct ElementStruct
    {
        int        inUse; ///< in use
        StackEntry stackTrace[StackEntries]; ///< stack trace
        char       elementBuffer; ///< element buffer
        //	   char       elementBuffer[];
    };

    //lint -save -e1554 -e1555
    class Counter
    {
    public:
        /**
          * construction
        */
        Counter  ()
            : m_elementSize (0U),
              m_elementNo   (0U),
              m_elements    (NULL),
              m_usage       (0U),
              m_usageMax    (0U),
              m_section     (NULL)
        {}

        /**
          * \brief construction
          * \iparam elementSize element size
          * \iparam elementNo  element number
          * \iparam elements   elements
          * \iparam section    section
        */
        Counter  (unsigned int elementSize, unsigned int elementNo, char* elements, void* section)
            : m_elementSize (elementSize),
              m_elementNo   (elementNo),
              m_elements    (elements),
              m_usage       (0U),
              m_usageMax    (0U),
              m_section     (section)
        {
        }

        /**
         * \brief construction
         * \iparam right counter ref
        */
        Counter  (const Counter &right)
            : m_elementSize (right.m_elementSize),
              m_elementNo   (right.m_elementNo),
              m_elements    (const_cast<char*>(right.m_elements)),
              m_usage       (right.m_usage),
              m_usageMax    (right.m_usageMax),
              m_section     (right.m_section)
        {
        }

        /**
          * \brief destruction
        */
        ~Counter ()
        {
            //          m_section  = NULL;
            m_elements = NULL;
        }

        /**
         * \brief operator =
         * \iparam right counter ref
         * \return Counter ref
        */
        Counter& operator= (const Counter &right)
        {
            if (this != &right)
            {
                m_elementSize = right.m_elementSize;
                m_elementNo   = right.m_elementNo;
                m_elements    = const_cast<char*>(right.m_elements);
                m_usage       = right.m_usage;
                m_usageMax    = right.m_usageMax;
                m_section     = right.m_section;
            }
            return *this;
        }

        /**
        * \brief allocate
        * \return allocated pointer
        */
        void *allocate(void* ptr);

        /**
        * \brief free ptr
        * \iparam ptr to free
        */
        void free(const void *ptr);

        /**
        * \brief search
        * \iparam ptr ptr to search
        * \return ptr
        */
        void* search(const void *ptr);

        /**
        * \brief get element size
        * \return element size
        */
        unsigned int getElementSize() const {return m_elementSize;}

        /**
        *  \brief get first pointer
        *  \return first ptr
        */
        const void *getFirstPtr() const {return reinterpret_cast<void*>(m_elements);}

        //lint -save -e613
        /**
       * \brief get last pointer
       * \return last ptr
       */
        const void *getLastPtr() const {return reinterpret_cast<void*>(m_elements + (ElementHeaderSize + m_elementSize) * m_elementNo);}

        //lint -restore
        /**
       * \brief to string
       * \return  class string
       */
        const char *toString(char* buffer, size_t bufferSize) const;

        /**
       * \brief print leak text
       * \return leak text
       */
        const char *printLeakText  (char* buffer, size_t bufferSize) const;

    private:
        unsigned int     m_elementSize; ///< element size
        unsigned int     m_elementNo; ///< element number
        char            *m_elements; ///< elements
        unsigned int     m_usage; ///< usage
        unsigned int     m_usageMax; ///< max usage
        void            *m_section; ///< section
    };
    //lint -restore
    char    m_memoryArea[MemSize]; ///< memory area
    Counter m_counters[ElementCount]; ///< counters
    MemLog  m_log; ///< mem log
    static  MemLeakDetection *s_memLeakDetectionP; ///< MemLeakDetection instance
};

#endif // _DEBUG

#endif // !defined(EA_B858CDC6_1259_4815_9247_63414D675826__INCLUDED_)
