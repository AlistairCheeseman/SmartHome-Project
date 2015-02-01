<?php

/**
 * Created by PhpStorm.
 * User: ALi
 * Date: 10/12/2014
 * Time: 12:15
 */
include "functions.php";
//prevent SQL injections
$powerfactor = intval($_GET['pf']);
$power = intval($_GET['pow']);
if ($powerfactor != null && $power != null) {
    try {
        $db = connectDB();
        $db->exec("INSERT INTO PowerLog (moment, powerReading, powerFactor) VALUES ('" . date("Y-m-d H:i:s") . "', '" . $power . "', '" . $powerfactor . "')");
        $db = null;
        echo "success";
    } catch (Exception $ex) {
        echo $ex;
    }
} else
    echo "missing or wrong parameters";
?>
