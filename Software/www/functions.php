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
 $db->exec(
"CREATE TABLE 'Dev_ControlType' (
	'Id' INTEGER PRIMARY KEY NOT NULL,
	'Name' TEXT,
	'DataType' TEXT,
	'category' TEXT,
	'Type' TEXT);
CREATE TABLE 'Dev_Types' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'Name' TEXT,
	'TopicName' TEXT);
CREATE TABLE 'log' (
	'Id' INTEGER PRIMARY KEY NOT NULL,
	'moment' DATETIME NOT NULL,
	'topic' TEXT NOT NULL,
	'payload' TEXT NOT NULL);
CREATE TABLE 'settings' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'Key' TEXT,
	'Value' TEXT,
	'Type' TEXT);
CREATE TABLE 'auto_Types' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'Description' INTEGER);
CREATE TABLE 'auto_state' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'Description' TEXT);
CREATE TABLE 'Rooms' (
	'Id' INTEGER PRIMARY KEY NOT NULL,
	'Name' TEXT,
	'TopicName' TEXT);
CREATE TABLE 'Dev_Controls' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'TypeId' INTEGER NOT NULL,
	'ControlId' INTEGER NOT NULL,
	'Name' TEXT,
	'TopicName'  TEXT ,
	'ControlTypeId' INTEGER,
	FOREIGN KEY(TypeId) REFERENCES Dev_Types(Id),
	FOREIGN KEY(ControlTypeId) REFERENCES Dev_ControlType(Id)
	UNIQUE (TypeId, ControlId));
CREATE TABLE 'Devices' (
	'Id' TEXT PRIMARY KEY NOT NULL,
	'TypeId' INTEGER,
	'Name' TEXT,
	'RoomId' INTEGER,
	FOREIGN KEY(TypeId) REFERENCES Dev_Types(Id),
	FOREIGN KEY(RoomId) REFERENCES Rooms(Id));
CREATE TABLE 'Sensors' (
	'Id' INTEGER PRIMARY KEY NOT NULL,
	'DevId' TEXT NOT NULL,
	'ControlId' INTEGER NOT NULL,
	'MapTopic' TEXT,
	'SRDevTopic' TEXT,
	'CurrentValue' TEXT,
	'Name' TEXT,
	'duration' INTEGER,
	FOREIGN KEY(DevId) REFERENCES Devices(Id),
	FOREIGN KEY(ControlId) REFERENCES Dev_Controls(Id),
	UNIQUE (ControlId, DevId),
	UNIQUE(MapTopic));
CREATE TABLE 'Sensor_Histories' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'SensorId' INTEGER NOT NULL,
	'moment' DATETIME,
	'value' TEXT,
	FOREIGN KEY(SensorId) REFERENCES Sensors(Id));
CREATE TABLE 'automation' (
	'Id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
	'Name' INTEGER,
	'Condition' INTEGER,
	'TypeId' INTEGER,
	'Topic' TEXT,
	'Payload' TEXT,
	'lastExec' DATETIME,
	'stateId' INTEGER,
	FOREIGN KEY(TypeId) REFERENCES auto_Types(Id)
	FOREIGN KEY(stateId) REFERENCES auto_state(Id));


CREATE VIEW 'DefaultMaps' AS
SELECT t1.Id, t2.TopicName as room, t3.TopicName as DeviceType, t4.TopicName as control, t4.ControlId as controlId
FROM Devices t1
INNER JOIN Rooms t2 ON t1.RoomId = t2.Id
INNER JOIN Dev_Types t3 on t1.TypeId = t3.Id
INNER JOIN Dev_Controls t4 on t3.Id = t4.TypeId;

INSERT INTO 'Dev_ControlType' VALUES(1,'Relay Control','digital','Output','OnOff');
INSERT INTO 'Dev_ControlType' VALUES(2,'Power','value','Sensor','W,Pf');
INSERT INTO 'Dev_ControlType' VALUES(3,'Button','digital','Request','Change State');
INSERT INTO 'Dev_ControlType' VALUES(4,'Temperature (+100 Offset)','value','Sensor','Temperature');
INSERT INTO 'Dev_ControlType' VALUES(5,'Humidity','value','Sensor','Humidity');
INSERT INTO 'Dev_ControlType' VALUES(6,'Light Levels','value','Sensor','Lux');
INSERT INTO 'Dev_ControlType' VALUES(7,'scale','scale','Output','scale value');

INSERT INTO 'Dev_Types' VALUES(1,'Heating Controller','Heating');
INSERT INTO 'Dev_Types' VALUES(2,'Power Outlet','PowerOutlet');
INSERT INTO 'Dev_Types' VALUES(3,'Environment Sensor','env_sense');
INSERT INTO 'Dev_Types' VALUES(4,'Energy Sensor','energymeter');
INSERT INTO 'Dev_Types' VALUES(5,'Light Switch','light');
INSERT INTO 'Dev_Types' VALUES(6,'Computer','Computer');

INSERT INTO 'settings' VALUES(1,'Populate on Power Fail','0','bool');

INSERT INTO 'auto_Types' VALUES(1,'start/stop');
INSERT INTO 'auto_Types' VALUES(3,'conditional on/off');
INSERT INTO 'auto_Types' VALUES(5,'Mirror');
INSERT INTO 'auto_Types' VALUES(6,'Duration');

INSERT INTO 'auto_state' VALUES(1,'Active');
INSERT INTO 'auto_state' VALUES(2,'Disabled');
INSERT INTO 'auto_state' VALUES(3,'Suggested - Not Active');
INSERT INTO 'auto_state' VALUES(4,'Temporary(one time auto generated)');
INSERT INTO 'auto_state' VALUES(5,'Suggested - Active');

INSERT INTO 'Dev_Controls' VALUES(1,1,1,'Hot Water','HotWater',1);
INSERT INTO 'Dev_Controls' VALUES(2,1,2,'Central Heating','Heating',1);
INSERT INTO 'Dev_Controls' VALUES(3,2,1,'Outlet 1 Button',NULL,3);
INSERT INTO 'Dev_Controls' VALUES(4,2,2,'Outlet 2 Button',NULL,3);
INSERT INTO 'Dev_Controls' VALUES(5,2,3,'Power Outlet 1','Outlet1',1);
INSERT INTO 'Dev_Controls' VALUES(6,2,4,'Power Outlet 2','Outlet2',1);
INSERT INTO 'Dev_Controls' VALUES(7,3,1,'Humidity Level','humidity',5);
INSERT INTO 'Dev_Controls' VALUES(8,3,2,'Temperature','Temp',4);
INSERT INTO 'Dev_Controls' VALUES(9,3,3,'Ambient Light','lightlevel',6);
INSERT INTO 'Dev_Controls' VALUES(10,4,1,'Power Consumption','power',2);
INSERT INTO 'Dev_Controls' VALUES(12,1,3,'Hot Water Boost',NULL,3);
INSERT INTO 'Dev_Controls' VALUES(13,1,4,'Central Heating Boost',NULL,3);
INSERT INTO 'Dev_Controls' VALUES(14,5,1,'Light Switch Button',NULL,3);
INSERT INTO 'Dev_Controls' VALUES(15,6,1,'Volume','vol',7);

INSERT INTO 'Rooms' VALUES(1,'House - Global','house');

");
}

function connectDB() {
    try {

        $db = new PDO(DB_CONNECTION);
        $exists = checkTableexists("Devices", $db);
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
