<?php
/**
 * Created by PhpStorm.
 * User: ALi
 * Date: 10/12/2014
 * Time: 12:14
 */
include "config.php";
// replace non-ascii character with its hex code without connecting to db.
//http://stackoverflow.com/questions/1162491/alternative-to-mysql-real-escape-string-without-connecting-to-db
function escape($value) {
    $return = '';
    for($i = 0; $i < strlen($value); ++$i) {
        $char = $value[$i];
        $ord = ord($char);
        if($char !== "'" && $char !== "\"" && $char !== '\\' && $ord >= 32 && $ord <= 126)
            $return .= $char;
        else
            $return .= '\\x' . dechex($ord);
    }
    return $return;
}
?>