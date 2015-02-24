<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
include "functions.php";
$DBView = gettext($_GET['view']);
if ($DBview != NULL) {
    $db = connectDB();
    if ($DBView == "power") {
        //    $statement =    $db->prepare("SELECT DATE(moment) as moment, powerReading, powerFactor  FROM 'PowerLog'"); 
        $statement = $db->prepare("SELECT moment, powerReading, powerFactor  FROM 'PowerLog'");
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        echo json_encode($results);
    } elseif ($DBView == "Rooms") {
        $statement = $db->prepare("SELECT Name  FROM 'Rooms'");
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        echo json_encode($results);
    }
    $db = null;
} else {
    echo "missing or wrong parameters";
}
?>