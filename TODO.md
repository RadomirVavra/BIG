ToDo List
=========

* Implement cache to provide fast acces to data even if data cannot be in memory
    * Input works in three modes according to available memory
        1. **all in memory**, direct access to memory by method data(), operator(), operator[] (cannot be used in other modes, do not return exceptions)
        2. **something in cache**, whole outermost entities are stored in cache, whole entity can be returned by method at(index) (cannot be used in mode 3, returns exception)
        3. **all in file**, all data always read from file
    * Output works in three modes too
        1. **all in memory**, direct access to memory by method data(), operator(), operator[] (cannot be used in other modes, do not return exceptions, pointers and references valid until openFile, writeFile, closeFile, or destructor called)
        2. **something in cache**, whole outermost entities are stored in cache, whole entity can be accessed by method at(index) (cannot be used in mode 3, returns exception, pointer valid until next usage of object only)
        3. **all in file**, all data always written directly to file
* Allow insertion (and removal) of new entities while writing data. Sometimes we might not now number of images in advance. How to implement efficiently? Use its own CHUNK for each entity? In writing mode 1, we would need to re-allocate whole array. Use locking, if not locked, cannot be in mode 1, once locked, cannot be unlocked.
* Implement testing methods for everything from Core family.
* Do profiling, optimize critical methods
    * try to change std::map<DataOrderIds, uint64_t> orderMap to std::vector<uint64_t> orderMap indexed directly by DataOrderIds
* Use smart pointers
* Implement support for other meta-data (chunk ids) using inheritance
* Implement support for tiles using inheritance
* Implement BigViewer - image viewer that can show images contained in BIG format. Implement UI in QT.
* Implement BigCreator - more elaborate viewer that enables to add new images into BIG file.


## Data storage re-thinked ##
Save each entity in its own chunk. Each data chunk first contains index in first 8 bytes (uint64_t) and than raw data.



Done List
=========
