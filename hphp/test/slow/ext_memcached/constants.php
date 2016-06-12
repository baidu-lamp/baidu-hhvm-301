<?php
  $refl = new ReflectionClass("Memcached");
  $constants = $refl->getConstants();
  var_dump($constants["OPT_DEAD_TIMEOUT"]);
  var_dump($constants["OPT_REMOVE_FAILED_SERVERS"]);
  var_dump($constants["RES_SERVER_MARKED_DEAD"]);
  var_dump($constants["RES_SERVER_TEMPORARILY_DISABLED"]);
