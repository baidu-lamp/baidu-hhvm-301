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

#include "hphp/runtime/base/fast-stat-cache.h"
#include "hphp/runtime/base/runtime-option.h"
#include "hphp/util/thread-local.h"
#include "hphp/util/lock.h"
#include "hphp/util/logger.h"

namespace HPHP {

  static ReadWriteMutex g_stat_cache_lock,
                        g_lstat_cache_lock,
						g_access_cache_lock;
						
  static inline uint64_t get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_usec + (uint64_t)tv.tv_sec * 1000000;
  }

  class StatCacheData {
    private:
      struct StatResult {
        struct stat m_stat;
        int m_ret;
      };

      typedef std::unordered_map<int, int> AccessResult;
      typedef std::unordered_map<std::string, StatResult> StatResultMap;
      typedef std::unordered_map<std::string, AccessResult> AccessResultMap;

    public:
      StatCacheData() : m_last_update_time_us(get_time_us()) {
      }

      int stat(const std::string& path, struct stat* buf) {
        checkUpdate();
        return statImpl(path, buf);
      }

      int lstat(const std::string& path, struct stat* buf) {
        checkUpdate();
        return lstatImpl(path, buf);
      }

      int access(const std::string& path, int mode) {
        checkUpdate();
        return accessImpl(path, mode);
      }
	  
	  void clearResultMap() {
		WriteLock wl1(g_stat_cache_lock);
		WriteLock wl2(g_lstat_cache_lock);
		WriteLock wl3(g_access_cache_lock);
		m_stat_cache.clear();
        m_lstat_cache.clear();
        m_access_cache.clear();
	  }
	  
	  void getCacheMapCount(ResultMapCount & result_map_count) {
		result_map_count.stat_cache_count   = m_stat_cache.size();
		result_map_count.lstat_cache_count  = m_lstat_cache.size();
		result_map_count.access_cache_count = m_access_cache.size();
	  }

    private:
      int statImpl(const std::string& path, struct stat* buf) {
        g_stat_cache_lock.acquireRead();
        auto it = m_stat_cache.find(path);
        if (it != m_stat_cache.end()) {
		  int ret = it->second.m_ret ;
          if (ret == 0) {
            memcpy(buf, &it->second.m_stat, sizeof(struct stat));
          }
		  g_stat_cache_lock.release(); 
          return ret;
        } else {
          int ret = ::stat(path.c_str(), buf);
		  g_stat_cache_lock.release();
		  WriteLock wl(g_stat_cache_lock);
          auto &stat_res = m_stat_cache[path];
          if (ret == 0) {
            memcpy(&stat_res.m_stat, buf, sizeof(struct stat));
          }
          stat_res.m_ret = ret;
          return ret;
        }
      }
	  
	  int lstatImpl(const std::string& path, struct stat* buf) {
        g_lstat_cache_lock.acquireRead();
        auto it = m_lstat_cache.find(path);
        if (it != m_lstat_cache.end()) {
		  int ret = it->second.m_ret;
          if (ret == 0) {
            memcpy(buf, &it->second.m_stat, sizeof(struct stat));
          }
		  g_lstat_cache_lock.release(); 
          return ret;
        } else {
          int ret = ::lstat(path.c_str(), buf);
		  g_lstat_cache_lock.release(); 
		  WriteLock wl(g_lstat_cache_lock);
          auto &stat_res = m_lstat_cache[path];
          if (ret == 0) {
            memcpy(&stat_res.m_stat, buf, sizeof(struct stat));
          }
          stat_res.m_ret = ret;
          return ret;
        }
      }

      int accessImpl(const std::string& path, int mode) {
		g_access_cache_lock.acquireRead();
        auto it = m_access_cache.find(path);
        if (it != m_access_cache.end()) {
          auto &access_res = it->second;
          auto it2 = access_res.find(mode);
          if (it2 != access_res.end()) {
			int ret = it2->second;
			g_access_cache_lock.release(); 
            return ret;
          } else {
            int ret = ::access(path.c_str(), mode);
			g_access_cache_lock.release(); 
			WriteLock wl(g_access_cache_lock);
            access_res[mode] = ret;
            return ret;
          }
        } else {
          int ret = ::access(path.c_str(), mode);
		  g_access_cache_lock.release(); 
		  WriteLock wl(g_access_cache_lock);
		  auto &access_res = m_access_cache[path];
          access_res[mode] = ret;
          return ret;
        }
      }

      void checkUpdate() {
		if (RuntimeOption::ServerFastStatCacheTTL <= 0) return;
        uint64_t now = get_time_us();
        if (now > m_last_update_time_us + RuntimeOption::ServerFastStatCacheTTL * 1000000) {
          m_last_update_time_us = now;
          clearResultMap();
        }
      }

      uint64_t m_last_update_time_us;
      StatResultMap m_stat_cache;
      StatResultMap m_lstat_cache;
      AccessResultMap m_access_cache;
  };

  StatCacheData g_statCacheData;
  int FastStatCache::stat(const std::string& path, struct stat* buf) {
	if (!RuntimeOption::ServerFastStatCache) return ::stat(path.c_str(), buf);
    return g_statCacheData.stat(path, buf);
  }

  int FastStatCache::lstat(const std::string& path, struct stat* buf) {
	if (!RuntimeOption::ServerFastStatCache) return ::lstat(path.c_str(), buf);
    return g_statCacheData.lstat(path, buf);
  }

  int FastStatCache::access(const std::string& path, int mode) {
	if (!RuntimeOption::ServerFastStatCache) return ::access(path.c_str(), mode);
    return g_statCacheData.access(path, mode);
  }
  
  void FastStatCache::clearFastStatCache() {
	g_statCacheData.clearResultMap();
	Logger::Info("fast stat cache is clear!");
  }
  
  void FastStatCache::getResultMapCount(ResultMapCount & result_map_count) {
	g_statCacheData.getCacheMapCount(result_map_count);
  }
}
