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
// Loads up an FST from a single FAR archive.

#ifndef THRAX_LOADFSTFROMFAR_H_
#define THRAX_LOADFSTFROMFAR_H_

#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>
#include <fst/extensions/far/far.h>
#include <fst/fst.h>
#include <thrax/datatype.h>
#include <thrax/function.h>

DECLARE_string(indir);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class LoadFstFromFar : public Function<Arc> {
 public:
  typedef fst::VectorFst<Arc> MutableTransducer;

  LoadFstFromFar() {}
  virtual ~LoadFstFromFar() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() != 2) {
      cout << "LoadFstFromFar: Expected 2 arguments but got " << args.size()
           << endl;
      return NULL;
    }

    if (!args[0]->is<string>()) {
      cout << "LoadFstFromFar: Expected string (path) for argument 1" << endl;
      return NULL;
    }
    const string& far_file =
        JoinPath(FLAGS_indir, *args[0]->get<string>());

    if (!args[1]->is<string>()) {
      cout << "LoadFstFromFar: Expected string (FST name) for argument 2"
           << endl;
      return NULL;
    }
    const string& fst_name = *args[1]->get<string>();

    VLOG(2) << "Loading FST " << fst_name << " from " << far_file;
    fst::FarReader<Arc>* reader(fst::FarReader<Arc>::Open(far_file));
    if (!reader) {
      cout << "LoadFstFromFar: Unable to open FAR: " << far_file << endl;
      return NULL;
    }

    if (!reader->Find(fst_name)) {
      cout << "LoadFstFromFar: Unable to find FST: " << fst_name << endl;
      delete reader;
      return NULL;
    }
    MutableTransducer* fst = new MutableTransducer(reader->GetFst());
    delete reader;
    return new DataType(fst);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(LoadFstFromFar<Arc>);
};

}  // namespace function
}  // namespace thrax

#endif  // THRAX_LOADFSTFROMFAR_H_
