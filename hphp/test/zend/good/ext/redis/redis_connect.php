<?php
$redis = new Redis();
$res = $redis->connect('10.26.225.16','6380');
var_dump($res);
try{
    $res = $redis->set('1','test');
}catch(Exception $e){
    print $e->getMessage();
}
