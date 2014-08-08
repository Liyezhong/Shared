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

/*
 * \brief defining a heap allocator for memory leak detection; see GlobalDeclaration.h
 */
class MemLeakDetection
{

public:
	MemLeakDetection();
	~MemLeakDetection();
	void* allocate(size_t size, void* ptr = NULL);
	void deallocate(const void* ptr);
	static MemLeakDetection& getInstance();

    enum Elements
    {
       Element0Byte,
       Element4Byte,
       Element8Byte,
       Element16Byte,
       Element32Byte,
       Element64Byte,
       Element128Byte,
       Element256Byte,
       Element512Byte,
       Element1024Byte,
       Element2048Byte,
       Element4096Byte,
       Element8192Byte,
       Element16384Byte,
       Element32768Byte,
       Element65536Byte,
       Element131072Byte,
       Element262144Byte,
       Element524288Byte,
       Element1048576Byte,
       Element5242880Byte,
       ElementCount
    };

    enum ElementNo
    {
       ElementNo0Byte       =  4000,
       ElementNo4Byte       =  1024,
       ElementNo8Byte       =  4000,
       ElementNo16Byte      =  6000,
       ElementNo32Byte      = 10000,
       ElementNo64Byte      = 12000,
       ElementNo128Byte     = 20000,
       ElementNo256Byte     =  8000,
       ElementNo512Byte     =  4000,
       ElementNo1024Byte    =  4000,
       ElementNo2048Byte    = 20000,
       ElementNo4096Byte    =   200,
       ElementNo8192Byte    =    20,
       ElementNo16384Byte   =    32,
       ElementNo32768Byte   =    10,
       ElementNo65536Byte   =     6,
       ElementNo131072Byte  =     6,
       ElementNo262144Byte  =     6,
       ElementNo524288Byte  =     6,
       ElementNo1048576Byte =     3,
       ElementNo5242880Byte =     4
    };

    enum ElementSize
    {
       ElementSize0Byte       =       sizeof(void*),
       ElementSize4Byte       =       4,
       ElementSize8Byte       =       8,
       ElementSize16Byte      =      16,
       ElementSize32Byte      =      32,
       ElementSize64Byte      =      64,
       ElementSize128Byte     =     128,
       ElementSize256Byte     =     256,
       ElementSize512Byte     =     512,
       ElementSize1024Byte    =    1024,
       ElementSize2048Byte    =    2048,
       ElementSize4096Byte    =    4096,
       ElementSize8192Byte    =    8192,
       ElementSize16384Byte   =   16384,
       ElementSize32768Byte   =   32768,
       ElementSize65536Byte   =   65536,
       ElementSize131072Byte  =  131072,
       ElementSize262144Byte  =  262144,
       ElementSize524288Byte  =  524288,
       ElementSize1048576Byte = 1048576,
       ElementSize5242880Byte = 5242880
    };

    struct StackEntry
    {
//        void * instruction;
        unsigned int instruction;
    };

    enum Constants
    {
       StackEntries   = 16,
       ElementHeaderSize = align32 (sizeof (bool)) + StackEntries * sizeof (StackEntry),
       MemSize0       = (ElementHeaderSize + ElementSize0Byte)       * ElementNo0Byte,
       MemSize4       = (ElementHeaderSize + ElementSize4Byte)       * ElementNo4Byte,
       MemSize8       = (ElementHeaderSize + ElementSize8Byte)       * ElementNo8Byte,
       MemSize16      = (ElementHeaderSize + ElementSize16Byte)      * ElementNo16Byte,
       MemSize32      = (ElementHeaderSize + ElementSize32Byte)      * ElementNo32Byte,
       MemSize64      = (ElementHeaderSize + ElementSize64Byte)      * ElementNo64Byte,
       MemSize128     = (ElementHeaderSize + ElementSize128Byte)     * ElementNo128Byte,
       MemSize256     = (ElementHeaderSize + ElementSize256Byte)     * ElementNo256Byte,
       MemSize512     = (ElementHeaderSize + ElementSize512Byte)     * ElementNo512Byte,
       MemSize1024    = (ElementHeaderSize + ElementSize1024Byte)    * ElementNo1024Byte,
       MemSize2048    = (ElementHeaderSize + ElementSize2048Byte)    * ElementNo2048Byte,
       MemSize4096    = (ElementHeaderSize + ElementSize4096Byte)    * ElementNo4096Byte,
       MemSize8192    = (ElementHeaderSize + ElementSize8192Byte)    * ElementNo8192Byte,
       MemSize16384   = (ElementHeaderSize + ElementSize16384Byte)   * ElementNo16384Byte,
       MemSize32768   = (ElementHeaderSize + ElementSize32768Byte)   * ElementNo32768Byte,
       MemSize65536   = (ElementHeaderSize + ElementSize65536Byte)   * ElementNo65536Byte,
       MemSize131072  = (ElementHeaderSize + ElementSize131072Byte)  * ElementNo131072Byte,
       MemSize262144  = (ElementHeaderSize + ElementSize262144Byte)  * ElementNo262144Byte,
       MemSize524288  = (ElementHeaderSize + ElementSize524288Byte)  * ElementNo524288Byte,
       MemSize1048576 = (ElementHeaderSize + ElementSize1048576Byte) * ElementNo1048576Byte,
       MemSize5242880 = (ElementHeaderSize + ElementSize5242880Byte) * ElementNo5242880Byte,
       MemSize        =                     MemSize0 +       MemSize4 +    MemSize8 +    MemSize16 +    MemSize32 +    MemSize64 +    MemSize128 +    MemSize256 +    MemSize512
                      +    MemSize1024 + MemSize2048 +    MemSize4096 + MemSize8192 + MemSize16384 + MemSize32768 + MemSize65536 + MemSize131072 + MemSize262144 + MemSize524288
                      + MemSize1048576               + MemSize5242880
    };
private:
    class MemLog
    {
    public:
       MemLog();
       ~MemLog();
       static void doLog         (const void *ptr, size_t size, bool allocated);
       static void doLogConflict (const void *ptr, size_t size, const char *comment);
    private:
       void flush       ();
       void log         (const void *ptr, size_t size, bool allocated);
       void logConflict (const void *ptr, size_t size, const char *comment);

       char         m_buffer[ElementSize32768Byte];
       char         m_fileName[MAX_PATH];
       unsigned int m_position;
       void        *m_criticalSection;

       static MemLog *s_instance;
    };

    struct ElementStruct
    {
       int        inUse;
       StackEntry stackTrace[StackEntries];
	   char       elementBuffer;
//	   char       elementBuffer[];
    };

    //lint -save -e1554 -e1555
    class Counter
    {
    public:
       Counter  ()
          : m_elementSize (0U),
            m_elementNo   (0U),
            m_elements    (NULL),
            m_usage       (0U),
            m_usageMax    (0U),
            m_section     (NULL)
       {}
       Counter  (unsigned int elementSize,unsigned int elementNo, char* elements, void* section)
          : m_elementSize (elementSize),
            m_elementNo   (elementNo),
            m_elements    (elements),
            m_usage       (0U),
            m_usageMax    (0U),
            m_section     (section)
       {
       }
       Counter  (const Counter &right)
          : m_elementSize (right.m_elementSize),
            m_elementNo   (right.m_elementNo),
            m_elements    (const_cast<char*>(right.m_elements)),
            m_usage       (right.m_usage),
            m_usageMax    (right.m_usageMax),
            m_section     (right.m_section)
       {
       }
       ~Counter ()
       {
//          m_section  = NULL;
          m_elements = NULL;
       }
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
       void              *allocate       (void* ptr);
       void               free           (const void  *ptr);
       void*              search		 (const void  *ptr);
       unsigned int getElementSize () const {return m_elementSize;}
       const void        *getFirstPtr    () const {return reinterpret_cast<void*>(m_elements);}
       //lint -save -e613
       const void        *getLastPtr     () const {return reinterpret_cast<void*>(m_elements + (ElementHeaderSize + m_elementSize) * m_elementNo);}
       //lint -restore
       const char        *toString       (char* buffer, size_t bufferSize) const;
       const char        *printLeakText  (char* buffer, size_t bufferSize) const;
    private:

       unsigned int     m_elementSize;
       unsigned int     m_elementNo;
       char            *m_elements; 
       unsigned int     m_usage;
       unsigned int     m_usageMax;
       void            *m_section;
    };
    //lint -restore
    char    m_memoryArea[MemSize];
    Counter m_counters[ElementCount];
    MemLog  m_log;
    static  MemLeakDetection *s_memLeakDetectionP;
};

#endif // _DEBUG

#endif // !defined(EA_B858CDC6_1259_4815_9247_63414D675826__INCLUDED_)
