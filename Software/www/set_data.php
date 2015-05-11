<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



include "functions.php";

$action = $_GET['action'];
$sensorId = $_GET['id'];
$db = connectDB();
try {
    $statement = $db->prepare("SELECT * from Sensors where Id =" . $sensorId);
    $statement->execute();
    $results = $statement->fetch(PDO::FETCH_ASSOC);
    $topic = "map/" . $results['MapTopic'] . "/R";
    if ($action == "off") {
        $payload = "0";
    } elseif ($action == "on") {
        $payload = "1";
    } else
        echo "error";


    unset($results);
    unset($statement);
    unset($db);
    //the database MUST be closed before message send or it will kill the logic layer!
    // NOTE NOTE NOTE HOW WE ARE NOT ALLOWED TO DIRECTLY SET THE COMMAND, THIS IS SECURITY FEATURE DO NOT CHANGE TO HAVE A VARIABLE DIRECTLY SET ON THE COMMAND LINE!
    $cmd = escapeshellcmd("mosquitto_pub -h 127.0.0.1 -p 1883 -m '" . $payload . "' -t " . $topic);
    $output = shell_exec($cmd);


    if (REDIRECTS) {
        header('Location: /sensors');
    }
    echo "<pre>$output</pre>";
    echo "published to: " . $topic . "<br />";
    echo "a payload of: " . $payload;
} catch (Exception $ex) {
    echo $ex;
}





