- J.Bugariu, 2010-11-03 -

The following classes (including some test classes) were removed in svn revision 1808.
They wre implemented in a time when it was not sure Qt's timout behaviour on the 4.6.3
target was OK. Basicly they are low level implementation of the posix threads and mutex
functionality:

 PCyclicThread
 PFunctionThread
 PMutex
 PMutexLocker
 PThread
 PWaitCondition
 PWorkRequestThread
