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
    for ($i = 0; $i < strlen($value); ++$i) {
        $char = $value[$i];
        $ord = ord($char);
        if ($char !== "'" && $char !== "\"" && $char !== '\\' && $ord >= 32 && $ord <= 126)
            $return .= $char;
        else
            $return .= '\\x' . dechex($ord);
    }
    return $return;
}

function createDB($db) {
    $db->exec("CREATE TABLE 'PowerLog' ('moment' DATETIME PRIMARY KEY NOT NULL, 'powerReading' REAL NOT NULL, 'powerFactor' REAL NOT NULL)");
       }

function connectDB() {
    try {
    
        $db = new PDO(DB_CONNECTION);
        $exists = checkTableexists("PowerLog", $db);
        if ($exists == FALSE) {
            createDB($db);
        }
    } catch (PDOException $ex) {
        echo $ex;
    }
    return $db;
}

function checkTableexists($tablename,$db) {
    try {
        $query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" . $tablename . "'";
     $result = $db->query($query);
    $count = 0;
     foreach ($result as $row)
     {
         $count++;
     }
       if ($count > 0)
       {
            return TRUE;
       }
       else
       { 
           return FALSE;
       }
       } 
       catch (PDOException $ex) {
        echo $ex;
        return FALSE;
    }
}


?>