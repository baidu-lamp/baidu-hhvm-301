<?php
$r = new Redis();
$conn = $r->connect('10.58.41.51', 9003, 10 );
$res = $r->mget(array());
var_dump($res);
