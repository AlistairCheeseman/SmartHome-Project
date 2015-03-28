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
        $query = "/get_data.php?view=Sensors&filter=sensors&devid=" + devid.toString();
    }
    else
    {
        $query = "/get_data.php?view=Sensors&filter=sensors";
    }
    $.getJSON($query, function (json)
    {
        $.each(json, function (key, value) {
            var row = $("<tr />");
            $("#listSenDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['Name'] + "</td>"));
            row.append($("<td>" + value['Device'] + "</td>"));
            row.append($("<td>" + value['Type'] + "</td>"));
            row.append($("<td>" + "<a href='/sensors/history?id=" + value['Id'] + "'>" + value['CurrentValue'] + "</a>" + "</td>"));
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
function getOutSensorList()
{
    var vars = {};
    vars = getvars();
    var devid = vars['devid'];
    $query = "";
    if (devid)
    {
        $query = "/get_data.php?view=Sensors&filter=output&devid=" + devid.toString();
    }
    else
    {
        $query = "/get_data.php?view=Sensors&filter=output";
    }
    $.getJSON($query, function (json)
    {
        $.each(json, function (key, value) {
            var row = $("<tr />");
            $("#listOutDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['Name'] + "</td>"));
            row.append($("<td>" + value['Device'] + "</td>"));
            row.append($("<td>" + value['Type'] + "</td>"));
            row.append($("<td>" + "<a href='/sensors/history?id=" + value['Id'] + "'>" + value['CurrentValue'] + "</a>" + "</td>"));
            row.append($("<td>" + "<a href='/set_data?action=on&amp;id=" + value['Id'] + "'>on</a>/" + "<a href='/set_data?action=off&amp;id=" + value['Id'] + "'>off</a>" + "</td>"));

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
function getReqSensorList()
{
    var vars = {};
    vars = getvars();
    var devid = vars['devid'];
    $query = "";
    if (devid)
    {
        $query = "/get_data.php?view=Sensors&filter=request&devid=" + devid.toString();
    }
    else
    {
        $query = "/get_data.php?view=Sensors&filter=request";
    }
    $.getJSON($query, function (json)
    {
        $.each(json, function (key, value) {
            var row = $("<tr />");
            $("#listReqDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['Name'] + "</td>"));
            row.append($("<td>" + value['Device'] + "</td>"));
            row.append($("<td>" + value['Type'] + "</td>"));
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

function showSensorHistory()
{
    var vars = {};
    vars = getvars();
    var sensorid = vars['id'];
    var sensortype = vars['type'];
    //sensorGraph
    //listDataTable
    data = [];

    $.getJSON("/get_data.php?view=SensorHist&id=" + sensorid, function (json)
    {
        $.each(json, function (key, value) {
            moment = stringToDate(value['moment']).getTime();
            // moment = Date.parse(value['moment']); 
            data.push([moment, parseInt(value['value'])]);



            var row = $("<tr />");
            $("#listDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
            row.append($("<td>" + value['moment'] + "</td>"));
            row.append($("<td>" + value['value'] + "</td>"));
        });
        $('#sensorGraph').highcharts(
                {
                    title: {
                        text: 'Usage'
                    },
                    xAxis: {
                        type: 'datetime'
                    },
                    yAxis: [{// Primary yAxis
                            labels: {
                                format: '{value}',
                                style: {
                                    color: Highcharts.getOptions().colors[1]
                                }
                            },
                            title: {
                                text: 'On/Off',
                                style: {
                                    color: Highcharts.getOptions().colors[1]
                                }

                            },
                            min: 0,
                            max: 1

                        }],
                    series: [{
                            name: 'Power Usage',
                            yAxis: 0,
                            data: data,
                            step: true
                        }]
                });
    });

}



function stringToDate(s) {
    s = s.split(/[-: ]/);
    return new Date(s[0], s[1] - 1, s[2], s[3], s[4], s[5]);
}
function stringToUnixDate(s) {
    s = s.split(/[-: ]/);
    return dateToUnix(s[0], s[1], s[2], s[3], s[4], s[5]);
}
function dateToUnix(year, month, day, hour, minute, second) {
    return ((new Date(Date.UTC(year, month - 1, day, hour, minute, second))).getTime() / 1000.0);
}


function populateMDevMapSelectLists()
{
    //  var vars = {};
    // vars = getvars();
    // $roomid = vars['roomid'];
    document.getElementById("outputlist").style.display = 'none';
    document.getElementById("selectlistlbl").style.display = 'none';
    document.getElementById('topicMaptxt').type = "hidden";
    document.getElementById('topicMaplbl').style.display = 'none';

    $.getJSON('/get_data.php?view=MDevMaps', function (json)
    {
        var $selectdev = $("#Devidlist");
        $.each(json, function (key, value) {
            $selectdev.append("<option value='" + value['Id'] + "/" + value['controlId'] + "'>" + value['Id'] + "/" + value['controlId'] + " (" + value['DeviceType'] + "/" + value['control'] + ")</option>");
        });
        var $change = $selectdev.on('change', function () {
            var vars = {};
            var $selectId = document.getElementById("Devidlist").selectedIndex;
            var $rawid = document.getElementById("Devidlist").options[$selectId].value;
            var $keys = $rawid.split("/");
            $.getJSON('/get_data.php?view=MDevMaps&filter=true&devId=' + $keys[0] + '&controlId=' + $keys[1], function (json)
            {
                $.each(json, function (key, value) {
                    vars[key] = value;
                });
                var $room = vars['room'];
                var $deviceType = vars['DeviceType'];
                var $control = vars['control'];
                var $controlType = vars['ControlTypeId'];
                document.getElementById('controlTId').value = $controlType;
                if ($controlType == 3)
                {

// load up all the devices that can be controlled.
                    $.getJSON("/get_data.php?view=Sensors&filter=output", function (json)
                    {
                        var $select = $("#outputlist");
                        $.each(json, function (key, value) {
                            $select.append("<option value='" + value['DevId'] + "/" + value['ControlId'] + "'>" + value['Name'] + "</option>");
                        });
                    });
                    //hide manual entry of the topic map as it will be handled by the multiple select list.
                    document.getElementById('topicMaptxt').type = "hidden";
                    document.getElementById('topicMaplbl').style.display = 'none';

                    //make sure we can see the select list.
                    document.getElementById("outputlist").style.display = 'block';
                    document.getElementById("selectlistlbl").style.display = 'block';
                }
                else
                {
                    //load up the default value for a mapping layer value.
                    document.getElementById('topicMaptxt').value = $room + "/" + $deviceType + "/" + $control;
                    //ensure the text box is visible
                    document.getElementById('topicMaplbl').style.display = 'block';
                    document.getElementById('topicMaptxt').type = "text";
                    //hide the multiple select list
                    document.getElementById("outputlist").style.display = 'none';
                    document.getElementById("selectlistlbl").style.display = 'none';
                }
            });
        });

    });


}

function  getsensordata()
{
    var vars = {};
    vars = getvars();
    $id = vars['id'];
    document.getElementById('id').value
    $url = '/get_data.php?view=Sensor&Id=' + $id.toString();
    $.getJSON($url, function (json)
    {
        $.each(json, function (key, value) {
            vars[key] = value;
        });
    }).error(function (error) {
        alert(error);
    });


    if (vars['MapTopic'] == null)//SR TOPIC
    {
        //hide the topic map box and ensure the value is set to default.
        //todo: need to make it so that the SRDevTopic is set by selecting a device from a drop down box.
        //then need to make it so that you can select multiple devices.
        document.getElementById('topicMaptxt').type = "hidden";
        document.getElementById('topicMaplbl').style.display = 'none';
        document.getElementById("outputlist").style.display = 'block';
        document.getElementById("selectlistlbl").style.display = 'block';

        //  document.getElementById('topicMaptxt').value = vars['SRDevTopic'];
        // load up all the devices that can be controlled.
        $.getJSON("/get_data.php?view=Sensors&filter=output", function (json)
        {
            var $select = $("#outputlist");
            $.each(json, function (key, value) {
                $select.append("<option value='" + value['DevId'] + "/" + value['ControlId'] + "'>" + value['Name'] + "</option>");
            });
        });


        var $longSRTopic = vars['SRDevTopic'];
        var $SRArray = $longSRTopic.split(";");



        $.each($SRArray, function (index, item) {
            //you're filtering options, not the list itself
            $("#outputlist > option").filter(function () {
                return $(this).val() == item;
            }).prop('selected', true); //use .prop, not .attr
        });




        document.getElementById('TopicTypetxt').value = "SR";

    }
    else
    {
        //ensure the text box is visible
        document.getElementById('topicMaplbl').style.display = 'block';
        document.getElementById('topicMaptxt').type = "text";
        //hide the multiple select list
        document.getElementById("outputlist").style.display = 'none';
        document.getElementById("selectlistlbl").style.display = 'none';

        document.getElementById('topicMaptxt').value = vars['MapTopic'];
        document.getElementById('TopicTypetxt').value = "MAP";
    }
    document.getElementById('sensorNametxt').value = vars['Name'];

}
          