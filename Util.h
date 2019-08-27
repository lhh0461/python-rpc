#ifndef __UTIL__
#define __UTIL__


#include <string>
#include <vector>

using std::vector;
using std::string;

#ifdef __cplusplus
extern "C" {
#endif

int GetPathFiles(const char *basePath, vector<string> & filelist);

#ifdef __cplusplus
}
#endif

#endif //__UTIL__

