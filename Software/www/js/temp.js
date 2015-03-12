/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

 $(document).ready(function (){
                $.getJSON('/get_data.php?view=Rooms', function (json)
                {
                    $.each(json, function(key,value) {
                var row = $("<tr />");
    $("#personDataTable").append(row); //this will append tr element to table... keep its reference for a while since we will add cels into it
    row.append($("<td>" + value['Name'] + "</td>"));
    row.append($("<td>" + value['sensorC'] + "</td>"));
      row.append($("<td>" + "<a href='rooms/edit?id=" + value['Id']+"'>"+"Edit" +"</a>"+ "</td>"));
  //  row.append($("<td><a href=/rooms/edit?id=" + value['Id'] + ">Edit</a></td>"));
    row.append($("<td><a href='/rooms/delete?id=" + value['Id'] + "' >Delete</a></td>"));
                    }); 
                });
});
