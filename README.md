lousy-leveldb
=============

Lousy Leveldb is leveldb with "lousy" features. Currently it supports delta updates. The delta updates are merged in the Get() operation and during background compaction.


Usage
-----

When opening a db, you need to specify a Value Merger, in which two methods need to be implemented.

    virtual int Merge(std::string *value, bool partial, const Slice& new_value) const = 0;
    virtual bool Shrink(std::string *value) const = 0;

For a sample of Value Merger, take a look at `db/db_test.cc`.

To insert delta updates, use `Append()`. And `Get()` will return the whole record.


