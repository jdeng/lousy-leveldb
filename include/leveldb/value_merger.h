// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_VALUE_MERGER_H_
#define STORAGE_LEVELDB_INCLUDE_VALUE_MERGER_H_

#include <string>

namespace leveldb {

class Slice;

class ValueMerger {
 public:
  virtual ~ValueMerger();

  virtual int Merge(std::string *value, bool partial, const Slice& new_value) const = 0;
  virtual bool Shrink(std::string *value) const = 0;

  // The name of the merger.  Used to check for merger
  // mismatches (i.e., a DB created with one merger is
  // accessed using a different merger.
  //
  // The client of this package should switch to a new name whenever
  // the merger implementation changes in a way that will cause
  // the relative ordering of any two keys to change.
  //
  // Names starting with "leveldb." are reserved and should not be used
  // by any clients of this package.
  virtual const char* Name() const = 0;
};

extern const ValueMerger* ConcatValueMerger();

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_VALUE_MERGER_H_
