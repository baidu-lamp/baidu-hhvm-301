<?php
$redis = new Redis();
$res = $redis->connect('10.26.225.16','6379');
$res = $redis->get('test');
var_dump($res);
