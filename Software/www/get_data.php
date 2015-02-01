<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
include "functions.php";

        
     $db = connectDB();    
         //    $statement =    $db->prepare("SELECT DATE(moment) as moment, powerReading, powerFactor  FROM 'PowerLog'"); 
       $statement =    $db->prepare("SELECT moment, powerReading, powerFactor  FROM 'PowerLog'"); 
             $statement->execute();
             $results=$statement->fetchAll(PDO::FETCH_ASSOC);
             echo json_encode($results);
             
    //    echo "[";
    //          foreach ($result as $row)
    // {
    //             echo "[\"".$row['moment']."\",".$row['powerReading'].",".$row['powerFactor']."]"; 
    //          }
    //         echo "]";
      
     
         
             
             
             
             
$db = null;







?>