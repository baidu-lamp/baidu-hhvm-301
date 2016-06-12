<?php
$redis = new redis();
$redis->connect('10.94.43.14', 9999);  
$redis->zAdd('xiongmeimei1', 10, 'xiong1');
var_dump($redis->zScore('xiongmeimei1', 'xiong1'));
var_dump($redis->zincrby('xiongmeimei1', 10, 'xiong1'));
