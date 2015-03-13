/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


function getid()
{
    var vars = {};
    vars = getvars();
    document.getElementById('id').value = vars['id'];
}
function getvars()
{
    var vars = {};
    var parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function (m, key, value) {
        vars[key] = value;
    });
    return vars;
}
function getRoomlist()
{
    $.getJSON('/get_data.php?view=Rooms', function (json)
    {
        $.each(json, function (key, value) {
            var row = $("<tr />");
            $("#listDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['Name'] + "</td>"));
            row.append($("<td>" + "<a href='/devices?roomid=" + value['Id'] + "'>" + value['sensorC'] + "</a>" + "</td>"));
            row.append($("<td>" + "<a href='/rooms/edit?id=" + value['Id'] + "'>" + "Edit" + "</a>" + "</td>"));
            row.append($("<td><a href='/rooms/delete?id=" + value['Id'] + "' >Delete</a></td>"));
        });
    });
}
function getDevList()
{
    var vars = {};
    vars = getvars();
    $roomid = vars['roomid'];
    $query = "";
    if ($roomid)
    {
        $query = "/get_data.php?view=Devs&roomid=" + $roomid.toString();
    }
    else
    {
        $query = "/get_data.php?view=Devs";
    }
    $.getJSON($query, function (json)
    {
        $.each(json, function (key, value) {
            var row = $("<tr />");
            $("#listDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['Name'] + "</td>"));
            row.append($("<td>" + value['Room'] + "</td>"));
            row.append($("<td>" + value['Type'] + "</td>"));
            row.append($("<td>" + "<a href='/devices/edit?id=" + value['Id'] + "'>" + "Edit" + "</a>" + "</td>"));
            row.append($("<td><a href='/devices/delete?id=" + value['Id'] + "' >Delete</a></td>"));
        });
    });
}
function getroomdata() {
    var vars = {};
    vars = getvars();
    $id = vars['id'];
    document.getElementById('id').value
    $url = '/get_data.php?view=Room&id=' + $id.toString();
    $.getJSON($url, function (json)
    {
        $.each(json, function (key, value) {
            vars[key] = value;
        });
        $tn = vars['TopicName'];
        $nam = vars['Name'];
        document.getElementById('topicName').value = $tn;
        document.getElementById('roomName').value = $nam;
    }).error(function (error) {
        alert(error);
    });

}
function populateRoomList()
{
    $.getJSON('/get_data.php?view=Rooms', function (json)
    {
        var $select = $("#roomidlist");
        $.each(json, function (key, value) {
            $select.append("<option value='" + value['Id'] + "'>" + value['Name'] + "</option>");
        });
    });
}
function populateDevTypeList()
{
    $.getJSON('/get_data.php?view=DevTypes', function (json)
    {
        var $select = $("#devidlist");
        $.each(json, function (key, value) {
            $select.append("<option value='" + value['Id'] + "'>" + value['Name'] + "</option>");
        });
    });
}
function getDevInfo() {
    var vars = {};
    vars = getvars();
    $id = vars['id'];
    document.getElementById('id').value
    $url = '/get_data.php?view=Dev&id=' + $id.toString();
    $.getJSON($url, function (json)
    {
        $.each(json, function (key, value) {
            vars[key] = value;
        });
        $Id = vars['Id'];
        $tid = vars['TypeId'];
        $name = vars['Name'];
        $rid = vars['RoomId'];
        document.getElementById('nameid').value = $name;
        for (var i = 0; i < document.getElementById("devidlist").length; i++) {
            if (document.getElementById("devidlist").options[i].value == $tid) {
                document.getElementById("devidlist").selectedIndex = i;
            }
        }
        for (var i = 0; i < document.getElementById("roomidlist").length; i++) {
            if (document.getElementById("roomidlist").options[i].value == $rid) {
                document.getElementById("roomidlist").selectedIndex = i;
            }
        }

        // document.getElementById('devidlist').selectedvalue = $tid;
        //document.getElementById('roomidlist').selectedvalue = $rid;
    }).error(function (error) {
        alert(error);
    });

}
function getSensorList()
{
    var vars = {};
    vars = getvars();
    var devid = vars['devid'];
    $query = "";
    if (devid)
    {
        $query = "/get_data.php?view=Sensors&devid=" + devid.toString();
    }
    else
    {
        $query = "/get_data.php?view=Sensors";
    }
    $.getJSON($query, function (json)
    {
        $.each(json, function (key, value) {
            var row = $("<tr />");
            $("#listDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['Name'] + "</td>"));
            row.append($("<td>" + value['Device'] + "</td>"));
             row.append($("<td>" + value['Type'] + "</td>"));
            row.append($("<td>" + value['CurrentValue'] + "</td>"));
            if (value['SRDevTopic'])
            {
                row.append($("<td>" + value['SRDevTopic'] + "</td>"));
            }
            else
            {
                row.append($("<td>" + value['MapTopic'] + "</td>"));
            }
            row.append($("<td>" + "<a href='/sensors/edit?id=" + value['Id'] + "'>" + "Edit" + "</a>" + "</td>"));
            row.append($("<td><a href='/sensors/delete?id=" + value['Id'] + "' >Delete</a></td>"));
        });
    });
}