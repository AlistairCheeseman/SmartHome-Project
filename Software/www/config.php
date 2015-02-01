<?php
/**
 * Created by PhpStorm.
 * User: ALi
 * Date: 10/12/2014
 * Time: 12:14
 */
date_default_timezone_set('Europe/London');
//if using mysql these variables must be set
define('DB_NAME','Smart_Home');
define('DB_USER','Smart_Home');
define('DB_PASSWORD', 'k3pR69J');
define('DB_HOST','192.168.3.1');
//if using sqlite then theese variables must be set
define('DB_CONNECTION', 'sqlite:/var/db/smarthome');
?>