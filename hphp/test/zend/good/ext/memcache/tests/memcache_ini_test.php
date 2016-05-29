<?php
var_dump(ini_get( 'memcache.hash_function'));
var_dump(ini_get( 'memcache.hash_strategy'));
var_dump(ini_set( 'memcache.hash_strategy', 'consistent' ));
var_dump(ini_get( 'memcache.hash_strategy'));
var_dump(ini_set( 'memcache.hash_function', 'crc32' ));
var_dump(ini_get( 'memcache.hash_function'));
var_dump(ini_get( 'memcache.hash_strategy'));
?>
