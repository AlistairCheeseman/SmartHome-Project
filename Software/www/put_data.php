<?php

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


include "functions.php";

$action = $_GET['action'];
$table = $_GET['table'];
$db = connectDB();
try {
    if ($table == "rooms") {
        if ($action == "new") {
            $roomName = $_GET['roomName'];
            $topicName = $_GET['topicName'];
            $roomName = str_replace("'", "''", $roomName);

            $db->exec("INSERT INTO 'Rooms' ('Name', 'TopicName') VALUES ('" . $roomName . "','" . $topicName . "')");
            $db = null;
            if (REDIRECTS) {
                header('Location: /rooms');
            }
            echo "success";
        } elseif ($action == "delete") {
            $id = intval($_GET['id']);
            $db->exec("DELETE FROM 'Rooms' where Id = '" . $id . "'");
            $db = null;
            if (REDIRECTS) {
                header('Location: /rooms');
            }
            echo "success";
        } elseif ($action == "edit") {
            $roomName = $_GET['roomName'];
            $topicName = $_GET['topicName'];
            $id = intval($_GET['id']);
            $roomName = str_replace("'", "''", $roomName);
            $db->exec("UPDATE 'Rooms' SET Name='" . $roomName . "', TopicName='" . $topicName . "' WHERE Id='" . $id . "'");
            $db = null;
            if (REDIRECTS) {
                header('Location: /rooms');
            }
            echo "success";
        }
    } elseif ($table == "devs") {
        if ($action == "new") {
             $id = $_GET['Id'];
            $Name = $_GET['Name'];
            $Roomid = intval($_GET['roomid']);
            $devtypeid = intval($_GET['devid']);

           $db->exec("INSERT INTO 'Devices' ('Id', 'TypeId', 'Name', 'RoomId') VALUES ('" . $id . "','" . $devtypeid . "','" . $Name . "','". $Roomid ."')");
            $db = null;
            if (REDIRECTS) {
                header('Location: /devices');
            }
            echo "success";
        } elseif ($action == "delete") {
            $id = $_GET['id'];
            $db->exec("DELETE FROM 'Devices' where Id = '" . $id . "'");
            $db = null;
            if (REDIRECTS) {
                header('Location: /devices');
            }
            echo "success";
        } elseif ($action == "edit") {
            $id = $_GET['id']; 
            $Name = $_GET['Name'];
            $Roomid = intval($_GET['roomid']);
            $devtypeid = intval($_GET['devid']);
            $db->exec("UPDATE 'Devices' SET TypeId='" . $devtypeid . "', Name='" . $Name . "', RoomId='" . $Roomid . "' WHERE Id='" . $id . "'");
            $db = null;
            if (REDIRECTS) {
                header('Location: /devices');
            }
            echo "success";
        }
    } else
        echo "error";
} catch (Exception $ex) {
    echo $ex;
}





