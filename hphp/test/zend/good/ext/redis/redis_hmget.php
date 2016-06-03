<?php
$redis = new Redis();
$res = $redis->connect('10.26.225.16','6379');
$res = $redis->hmGet('h', array('field1', 'field2'));
var_dump($res);
