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
    $statement = $db->prepare("SELECT t1.Id, (SELECT count (*) from Devices t2 where t2.RoomId = t1.Id) as sensorC, t1.Name  from Rooms t1");
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "Room") {
    $id = intval($_GET['id']);
    $statement = $db->prepare("SELECT Name, TopicName, Id  FROM 'Rooms' where id=" . $id);
    $statement->execute();
    $results = $statement->fetch(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "Devs") {
    $Roomid = intval($_GET['roomid']);
    if ($Roomid > 0) {
        $statement = $db->prepare("SELECT t1.Name as Name, t1.Id, t2.Name as Room, t3.Name as Type FROM Devices t1 INNER JOIN Rooms t2 ON t2.Id = t1.RoomId INNER JOIN Dev_types t3 ON t3.Id = t1.TypeId where t1.RoomId = " . $Roomid);
    } else {
        $statement = $db->prepare("SELECT t1.Name as Name, t1.Id, t2.Name as Room, t3.Name as Type FROM Devices t1 INNER JOIN Rooms t2 ON t2.Id = t1.RoomId INNER JOIN Dev_types t3 ON t3.Id = t1.TypeId");
    }
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
}
    elseif ($DBView == "DevTypes") {
        $statement = $db->prepare("SELECT * FROM Dev_Types");
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
}elseif ($DBView == "Dev") {
    $devid = $_GET['id'];
    $query = "SELECT * FROM Devices where Id='" . $devid. "'";
        $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetch(PDO::FETCH_ASSOC);
    echo json_encode($results);
}elseif ($DBView == "Sensors") {
    $devid = $_GET['devid'];
    if ($devid)
    {
         $query = "SELECT t1.Id, t1.Name as Name, t2.Name as Device, t1.CurrentValue, t1.SRDevTopic, t1.MapTopic, t3.Name as Type FROM Sensors t1 INNER JOIN Devices t2 ON t2.id = t1.DevId INNER JOIN Dev_Controls t3 ON t3.ControlId = t1.ControlId AND t3.TypeId = t2.TypeId where DevId='" . $devid. "'";
    }
    else
    {
    $query = "SELECT t1.Id, t1.Name as Name, t2.Name as Device, t1.CurrentValue, t1.SRDevTopic, t1.MapTopic, t3.Name as Type FROM Sensors t1 INNER JOIN Devices t2 ON t2.id = t1.DevId INNER JOIN Dev_Controls t3 ON t3.ControlId = t1.ControlId AND t3.TypeId = t2.TypeId";
    }
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
}
