/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-2014 Facebook, Inc. (http://www.facebook.com)     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/
#ifndef incl_HPHP_BASE_FASTSTATCACHE_H_
#define incl_HPHP_BASE_FASTSTATCACHE_H_

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <unordered_map>

namespace HPHP {
  struct ResultMapCount {
	int stat_cache_count;
	int lstat_cache_count;
	int access_cache_count;
  };
  
  class FastStatCache {
    public:
      static int stat(const std::string& path, struct stat* buf);
      static int lstat(const std::string& path, struct stat* buf);
      static int access(const std::string& path, int mode);
	  static void clearFastStatCache();
	  static void getResultMapCount(ResultMapCount & result_map_count);
  };
}

#endif
