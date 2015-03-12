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
    /* need to convert this to a view or programatically create this table when called. */
    $db->exec("CREATE TABLE 'PowerLog' ('moment' DATETIME PRIMARY KEY NOT NULL, 'powerReading' REAL NOT NULL, 'powerFactor' REAL NOT NULL)");
 
    $db->exec("CREATE TABLE 'log' ('Id' INTEGER PRIMARY KEY NOT NULL, 'moment' DATETIME NOT NULL, 'topic' TEXT NOT NULL, 'payload' TEXT NOT NULL)");
    $db->exec("CREATE TABLE 'Dev_Types' ('Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 'Name' TEXT, 'TopicName' TEXT)");
    $db->exec("CREATE TABLE 'Rooms' ('Id' INTEGER PRIMARY KEY NOT NULL, 'Name' TEXT, 'TopicName' TEXT)");
    $db->exec("CREATE TABLE 'Dev_Controls' ('Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 'TypeId' INTEGER NOT NULL, 'ControlId' INTERGER NOT NULL, 'Name' TEXT, 'TopicName'  TEXT ,FOREIGN KEY(TypeId) REFERENCES Dev_Types(Id), UNIQUE (TypeId, ControlId))");
    $db->exec("CREATE TABLE 'Devices' ('Id' TEXT PRIMARY KEY NOT NULL, 'TypeId' INTEGER, 'Name' TEXT, 'RoomId' INTEGER, FOREIGN KEY(TypeId) REFERENCES Dev_Types(Id), FOREIGN KEY(RoomId) REFERENCES Rooms(Id))");
    $db->exec("CREATE TABLE 'Sensors' ('Id' INTEGER PRIMARY KEY NOT NULL, 'DevId' TEXT NOT NULL, 'ControlId' INTEGER NOT NULL, 'MapTopic' TEXT,'SRDevTopic' TEXT, 'CurrentValue' TEXT, FOREIGN KEY(DevId) REFERENCES Devices(Id), FOREIGN KEY(ControlId) REFERENCES Dev_Controls(Id), UNIQUE (ControlId, DevId), UNIQUE(MapTopic))");
    $db->exec("CREATE TABLE 'Sensor_Histories' ('Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 'SensorId' INTEGER NOT NULL, 'moment' DATETIME, 'value' TEXT, FOREIGN KEY(SensorId) REFERENCES Sensors(Id))");
    $db->exec("CREATE VIEW 'DefaultMaps' AS SELECT t1.Id, t2.TopicName as room, t3.TopicName as DeviceType, t4.TopicName as control, t4.ControlId as controlId FROM Devices t1 INNER JOIN Rooms t2 ON t1.RoomId = t2.Id INNER JOIN Dev_Types t3 on t1.TypeId = t3.Id INNER JOIN Dev_Controls t4 on t3.Id = t4.TypeId");
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

function checkTableexists($tablename, $db) {
    try {
        $query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" . $tablename . "'";
        $result = $db->query($query);
        $count = 0;
        foreach ($result as $row) {
            $count++;
        }
        if ($count > 0) {
            return TRUE;
        } else {
            return FALSE;
        }
    } catch (PDOException $ex) {
        echo $ex;
        return FALSE;
    }
}

?>