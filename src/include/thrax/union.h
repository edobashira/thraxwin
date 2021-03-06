// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2005-2011 Google, Inc.
// Author: ttai@google.com (Terry Tai)
//
// Wrapper for the union function, which expands the first argument and
// unions into there (destructive-mode) or just uses UnionFst (delayed-mode).

#ifndef THRAX_UNION_H_
#define THRAX_UNION_H_

#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fst.h>
#include <fst/union.h>
#include <fst/vector-fst.h>
#include <thrax/function.h>
#include <thrax/datatype.h>

namespace thrax {
namespace function {

template <typename Arc>
class Union : public BinaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;

  Union() {}
  virtual ~Union() {}

 protected:
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) {
    if (args.size() != 2) {
      cout << "Union: Expected 2 arguments but got " << args.size() << endl;
      return NULL;
    }

    MutableTransducer* mutable_left = new MutableTransducer(left);
    fst::Union(mutable_left, right);
    return mutable_left;
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(Union<Arc>);
};

template <typename Arc>
class UnionDelayed : public BinaryFstFunction<Arc> {
 public:
  typedef fst::Fst<Arc> Transducer;

  UnionDelayed() {}
  virtual ~UnionDelayed() {}

 protected:
  virtual Transducer* BinaryFstExecute(const Transducer& left,
                                       const Transducer& right,
                                       const vector<DataType*>& args) {
    if (args.size() != 2) {
      cout << "UnionDelayed: Expected 2 arguments but got " << args.size()
           << endl;
      return NULL;
    }

    return new fst::UnionFst<Arc>(left, right);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(UnionDelayed<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_UNION_H_
