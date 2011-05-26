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
// Author: rws@google.com (Richard Sproat)
//
// Simple implementation of StrCat, needed in various places.  This version
// allows from 2 to 5 combinations of strings and ints.

#include <stdarg.h>
#include <stdio.h>

#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>
#include <fst/compat.h>
#include <thrax/compat/utils.h>

#ifdef WINDOWS //PaulD
#include <Windows.h>
const char kFileSeperator = '\\';
#else
#include <sys/stat.h> 
#include <sys/types.h>
const char kFileSeperator = '/';
#endif

namespace thrax {

using std::string;
using std::ifstream;
using std::ios;
using std::ios_base;
using std::vector;
using std::wstring;

string StrCat(const StringOrInt &s1, const StringOrInt &s2) {
  return s1.str_ + s2.str_;
}

string StrCat(const StringOrInt &s1,
              const StringOrInt &s2,
              const StringOrInt &s3) {
  return s1.str_ + s2.str_ + s3.str_;
}

string StrCat(const StringOrInt &s1,
              const StringOrInt &s2,
              const StringOrInt &s3,
              const StringOrInt &s4) {
  return s1.str_ + s2.str_ + s3.str_ + s4.str_;
}

string StrCat(const StringOrInt &s1,
              const StringOrInt &s2,
              const StringOrInt &s3,
              const StringOrInt &s4,
              const StringOrInt &s5) {
  return s1.str_ + s2.str_ + s3.str_ + s4.str_ + s5.str_;
}

string StringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  char buf[1024];
  vsnprintf(buf, 1024, format, ap);
  string retval(buf);
  va_end(ap);
  return retval;
}

// Split a string according to the delimiters, including consecutive
// delimiters as empty strings.

void SplitStringAllowEmpty(const string& full, const char* delim,
                           vector<string>* result) {
  size_t prev = 0;
  size_t found = full.find_first_of(delim);
  result->push_back(full.substr(prev, found - prev));
  while (found != string::npos) {
    prev = found + 1;
    found = full.find_first_of(delim, prev);
    result->push_back(full.substr(prev, found - prev));
  }
}

// Split a string according to the delimiters, skipping over
// consecutive delimiters.

void SplitStringUsing(const string& full, const char* delim,
                      vector<string>* result) {
  size_t prev = 0;
  size_t found = full.find_first_of(delim);
  size_t size = found - prev;
  if (size > 0) result->push_back(full.substr(prev, size));
  while (found != string::npos) {
    prev = found + 1;
    found = full.find_first_of(delim, prev);
    size_t size = found - prev;
    if (size > 0) result->push_back(full.substr(prev, size));
  }
}

// Operations on filenames

string JoinPath(const string& dirname, const string& basename) {
  if ((!basename.empty() && basename[0] == kFileSeperator ) || //PaulD
		dirname.empty()) {
    return basename;
  } else if (dirname[dirname.size() - 1] == kFileSeperator ) { //PaulD
    return dirname + basename;
  } else {
    return dirname + kFileSeperator + basename; //PaulD
  }
}

const char* Suffix(const char* filename) {
  const char* base = strrchr(filename, kFileSeperator ); //PaulD
  if (!base) base = filename; 
  const char* last_dot = strrchr(base, '.');
  return (last_dot ? last_dot + 1 : NULL);
}

string StripBasename(const char* filename) {
  const char* base = strrchr(filename, kFileSeperator ); //PaulD
  if (!base) return(string(""));
  string sfilename(filename);
  return sfilename.substr(0, base - filename);
}

string StripBasename(const string& filename) {
  return StripBasename(filename.c_str());
}

bool Readable(const string& f) {
#ifdef WINDOWS
	ifstream ifs(f); //PaulD
	if (ifs.is_open())
		return true;
	return false;
#else
  int fdes = open(f.c_str(), O_RDONLY);
  if (fdes == -1) return false;
  close(fdes);
  return true;
#endif
}

void ReadFileToStringOrDie(const string& file, string* store) {
  ifstream stream(file.c_str(), ios::in);
  if (stream.fail()) {
    LOG(FATAL) << "Can't open file " << file << " for reading";
  }
  stream.seekg(0, ios::end);
  size_t length = stream.tellg();
  stream.seekg(0, ios::beg);
  char* buf = new char[length];
  stream.read(buf, length);
  store->append(buf, length);
  delete[] buf;
  stream.close();
}


#ifdef WINDOWS
size_t WideCharsLen(const char *src) {
  const int len = ::MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
  if (len <= 0) {
    return 0;
  }
  return len - 1;  // -1 represents Null termination.
}

int UTF8ToWide(const char *input, wstring *output) {
  const int output_length = WideCharsLen(input);
  if (output_length == 0) {
    return 0;
  }
	
  wchar_t* input_wide = new wchar_t[output_length + 1];
  const int result = MultiByteToWideChar(CP_UTF8, 0, input, -1,
                                         input_wide, output_length + 1);
  if (result > 0) {
    output->assign(input_wide);
  }
	delete[] input_wide;
  return result;
}
#endif

// A partial (largely non-) implementation of this functionality.
bool RecursivelyCreateDirWithOptions(const string& path,
                                     const RecursiveCreateOptions& options) {
  if (path.empty()) return true;
  vector<string> path_comp;
  SplitStringUsing(path, "/", &path_comp);
  if (path[0] == '/') path_comp[0] = "/" + path_comp[0];  
  string rpath;
  for (vector<string>::iterator ix = path_comp.begin();
       ix != path_comp.end();
       ++ix) {
    rpath = rpath.empty() ? *ix : rpath + "/" + *ix;
    const char* crpath = rpath.c_str();
		LOG(FATAL) << "Not implemented";
#ifdef WINDOWS
		wstring wpath; 
		UTF8ToWide(crpath, &wpath);
		DWORD att = ::GetFileAttributes(wpath.c_str());	
		if (att != INVALID_FILE_ATTRIBUTES) {
			if (att & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			return false;
		} else {
			if (!::CreateDirectory(wpath.c_str(), NULL))
				return false;
		}
#else
		struct stat stat_buf;
    int statval = stat(crpath, &stat_buf); PaulD
    if (statval == 0) {
      if (S_ISDIR(stat_buf.st_mode)) continue;
      return false;
    } else {
      int mkval = mkdir(crpath, ACCESSPERMS);
      if (mkval == -1) return false;
    }
#endif
  }
  return true;
}

File* OpenOrDie(const string& filename, const string& mode) {
  ios_base::openmode m = static_cast<ios_base::openmode>(0);
  if (mode.find('r') != string::npos) m |= ios::in;
  if (mode.find('w') != string::npos) m |= ios::out;
  if (mode.find('a') != string::npos) m |= ios::app;
  ifstream *streamptr = new ifstream(filename.c_str(), m);
  if (streamptr->fail()) {
    delete streamptr;
    LOG(FATAL) << "Can't open file " << filename << " for reading";
  }
  File* file = new File(streamptr);
  return file;
}

}  // namespace thrax
