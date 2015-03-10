<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
include "functions.php";

 $DBView = $_GET['view'];
    $db = connectDB();
    if ($DBView == "power") {
        //    $statement =    $db->prepare("SELECT DATE(moment) as moment, powerReading, powerFactor  FROM 'PowerLog'"); 
        $statement = $db->prepare("SELECT moment, powerReading, powerFactor  FROM 'PowerLog'");
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        echo json_encode($results);
    } elseif ($DBView == "Rooms") {
        $statement = $db->prepare("SELECT Name, '3' as sensorC, 1 as Id  FROM 'Rooms'");
        $statement->execute();
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
        echo json_encode($results);
    }
