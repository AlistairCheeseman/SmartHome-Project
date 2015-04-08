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
} elseif ($DBView == "DevTypes") {
    $statement = $db->prepare("SELECT * FROM Dev_Types");
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "Dev") {
    $devid = $_GET['id'];
    $query = "SELECT * FROM Devices where Id='" . $devid . "'";
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetch(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "Sensors") {
    $filter = $_GET['filter'];

    $devid = $_GET['devid'];
    if ($devid) {
        $query = "SELECT t1.Id, t1.Name as Name, t2.Name as Device, t1.CurrentValue, t1.SRDevTopic, t1.MapTopic, t3.Name as Type, t1.DevId as DevId, t1.ControlId as ControlId, t3.ControlTypeId, t4.Category as ControlCategory  FROM Sensors t1 INNER JOIN Devices t2 ON t2.id = t1.DevId INNER JOIN Dev_Controls t3 ON t3.ControlId = t1.ControlId AND t3.TypeId = t2.TypeId INNER JOIN Dev_ControlType t4 ON t4.Id = t3.ControlTypeId  where DevId='" . $devid . "'";
    } else {
        $query = "SELECT t1.Id, t1.Name as Name, t2.Name as Device, t1.CurrentValue, t1.SRDevTopic, t1.MapTopic, t3.Name as Type, t1.DevId as DevId, t1.ControlId as ControlId, t3.ControlTypeId, t4.Category as ControlCategory  FROM Sensors t1 INNER JOIN Devices t2 ON t2.id = t1.DevId INNER JOIN Dev_Controls t3 ON t3.ControlId = t1.ControlId AND t3.TypeId = t2.TypeId INNER JOIN Dev_ControlType t4 ON t4.Id = t3.ControlTypeId ";
    }

    if ($filter) {
        if ($filter == "output") {
            $query = $query . " where t4.Category = 'Output'";
        }
        if ($filter == "sensors") {
            $query = $query . " where t4.Category = 'Sensor'";
        }
        if ($filter == "request") {
            $query = $query . " where t4.Category = 'Request'";
        }
    }
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "SensorHist") {
    $sensorid = $_GET['id'];
    $filter = $_GET['filter'];
    $query = "SELECT * FROM Sensor_Histories where SensorId='" . $sensorid . "'";
    if ($filter == "date-day") {
        $date = date_create($_GET['date']);
        $datebegin = date_format($date, 'Y-m-d');
        date_add($date, date_interval_create_from_date_string('1 days'));
        $dateend = date_format($date, 'Y-m-d');
        $query = $query . " AND strftime('%s', moment) BETWEEN strftime('%s', '" . $datebegin . "') AND strftime('%s', '" . $dateend . "')";
    }
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "MDevMaps") {
    $query = "select dm.Id as Id, dm.room as room, dm.DeviceType as DeviceType, dm.control as control, dm.controlId as controlId, DC.ControlTypeId as ControlTypeId from DefaultMaps dm INNER JOIN Devices devs ON devs.Id = dm.Id INNER JOIN Dev_Controls DC ON (DC.ControlId = dm.controlId AND DC.TypeId = devs.TypeId) where not exists (select 1 from sensors s where s.DevId = dm.Id AND s.ControlId = dm.ControlId)";
    $filter = $_GET['filter'];
    if ($filter == "true") {
        $devId = $_GET['devId'];
        $controlId = $_GET['controlId'];
        $query = $query . " AND dm.Id = '" . $devId . "' AND dm.controlId = " . $controlId;
    }
    $statement = $db->prepare($query);
    $statement->execute();
    if ($filter == "true") {
        $results = $statement->fetch(PDO::FETCH_ASSOC);
    } else {
        $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    }
    echo json_encode($results);
} elseif ($DBView == "Sensor") {
    $sensorId = $_GET['Id'];
    $query = "select sen.Id, sen.DevId, sen.ControlId, sen.MapTopic, sen.SRDevTopic, sen.CurrentValue, sen.Name, DC.ControlTypeId from Sensors sen INNER JOIN Devices dev ON (sen.DevId = dev.Id) INNER JOIN Dev_Controls DC ON (sen.ControlId = DC.ControlId  AND dev.TypeId = DC.TypeId) where sen.Id = " . $sensorId;
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetch(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "Rules") {
    $filter = $_GET['filter'];

    $query = "select ato.Id, ato.Name, ato.Condition, at.Description as Type, ato.Topic, ato.Payload, ato.lastExec, ast.Description as state from automation ato 
INNER JOIN auto_Types at ON at.Id = ato.TypeId 
INNER JOIN auto_state ast ON ast.Id = ato.stateId";
    if ($filter == "user") {
        $query = $query . " where ato.stateId == 1 OR ato.stateId == 0";
    }
    if ($filter == "suggested") {
        $query = $query . " where ato.stateId != 1 AND ato.stateId != 0";
    }
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "Rule") {
    $ruleId = $_GET['Id'];
    $query = "select * from automation where Id = " . $ruleId;
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetch(PDO::FETCH_ASSOC);
    echo json_encode($results);
} elseif ($DBView == "ActiveDevices") {
    $query = "SELECT t1.Id, t1.Name as Name, t2.Name as Device, t1.CurrentValue, t1.SRDevTopic, t1.MapTopic, t3.Name as Type, t1.DevId as DevId, t1.ControlId as ControlId, t3.ControlTypeId, t4.Category as ControlCategory, t5.Name as RoomName  FROM Sensors t1 INNER JOIN Devices t2 ON t2.id = t1.DevId INNER JOIN Dev_Controls t3 ON t3.ControlId = t1.ControlId AND t3.TypeId = t2.TypeId INNER JOIN Dev_ControlType t4 ON t4.Id = t3.ControlTypeId INNER JOIN Rooms t5 on t5.Id = t2.RoomId where t4.Category = 'Output' AND t1.CurrentValue != '0'";
    $statement = $db->prepare($query);
    $statement->execute();
    $results = $statement->fetchAll(PDO::FETCH_ASSOC);
    echo json_encode($results);
}