<?php
/**
 * Created by PhpStorm.
 * User: ALi
 * Date: 10/12/2014
 * Time: 12:15
 */
include "functions.php";
	//prevent SQL injections
	$powerfactor=intval($_GET['pf']);

	$power=intval($_GET['pow']);

	if($powerfactor != null && $power != null)
    {
        //create new mySQL connection
        $con=new mysqli(DB_HOST,DB_USER, DB_PASSWORD,DB_NAME);

        // Check if connection was established
        if (mysqli_connect_errno())
            echo "Failed to connect to MySQL: " . mysqli_connect_error();

        //execute SQL query
        mysqli_query($con,"INSERT INTO PowerLog (moment, powerReading, powerFactor)VALUES ('".date("Y-m-d H:i:s")."', '". $power ."','" . $powerfactor . "')");

        //close database connection
        mysqli_close($con);

        echo "success";
    }
    else
        echo "missing or wrong parameters";
?>
