<?php
class MyClass {
    public function __construct() {
        $this->prop1 = 'something';
        $this->prop2 = 'somethingElse';
        $this->prop3 = 'somethingElseThen';

        //Increment refcount for $this
        set_error_handler(array($this, 'errorHandler'));
    }

    public function errorHandler($severity, $message, $file = NULL, $line = NULL) {
        return TRUE;
    }

    public function fail() {
        //Force object destruction
        user_error('Try to cause an exception', E_USER_ERROR);
        throw new Exception('Try to cause an exception');
    }
}

function main() {
    $x = new MyClass();
    $x->fail();
}

main();
?>
