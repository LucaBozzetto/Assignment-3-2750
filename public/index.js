'use strict'

// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

    // Change text on hide/show buttons
    $(".hideBtn").click(function(e){
        let $this = $(this);
        $this.toggleClass("hideBtn");

        if($this.hasClass("hideBtn")){
            $this.text("Hide Panel");
        }else{
            $this.text("Show Panel");
        }
    });

    // clear all messages in status panel
    $("#clearBtn").click(function(e){
        e.preventDefault();
        let $this = $(this);

        $(".alert").alert('close');
    });

    // Upload the selected file to the server. I could have used a form and submitted it but this way
    // i can control what to do in case i get an invalid response or change my app behavior to a 
    // SPA instead of reloading.
    $("#uploadVCard").click(function(e){
        e.preventDefault();

        let selectedFile = document.getElementById("inputFile").files[0];
        if(!selectedFile)
            return;

        let extension = selectedFile.name.split('.').pop();
        extension = extension.toLowerCase();

        if(extension != "vcf" && extension != "vcard"){
            // console.log("invalid file extension");
            return;
        }

        let form = new FormData();

        form.append('uploadFile',selectedFile, selectedFile.name);
        // $("#formVCard").submit();

        $(this).toggleClass("running");
        $(this).toggleClass("noClick");

        $.ajax({
            type: 'post',            //Request type
            data: form,
            processData: false,
            contentType: false,
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/upload',   //The server endpoint we are connecting to
            success: function (data) {
                /*  Do something with returned object
                    Note that what we get is an object, not a string, 
                    so we do not need to parse it on the server.
                    JavaScript really does handle JSONs seamlessly
                */
                // $('#blah').html("On page load, Received string '"+JSON.stringify(data)+"' from server");
                //We write the object to the console to show that the request was successful
                // console.log("success");
                // window.location.replace("/"); 
                $("#uploadVCard").toggleClass("running");
                $("#uploadVCard").toggleClass("noClick");

                updateUI();

            },
            fail: function(error) {
                $(this).toggleClass("running");
                $(this).toggleClass("noClick");
                // Non-200 return, do something with error
                // console.log("Error"); 
            }
        });
    });

    /***
     * AJAX calls
     */

    function getFileNames(){
        return new Promise((resolve, reject) => {
            $.getJSON({
                url: '/upload',   //The server endpoint we are connecting to
                success: resolve,
                error: reject,
            });
        });
    }

    function getSummaryFromFile(name){
        // console.log(name);
        return new Promise((resolve, reject) => {
            $.getJSON({
                url: '/summary/'+name,   //The server endpoint we are connecting to
                success(summary) {
                    resolve({"name":name, "summary": summary})
                },
                error: reject,
            });
        });
    }

    function getCardDetails(name){
        return new Promise((resolve, reject) => {
            $.getJSON({
                url: '/details/'+name,   //The server endpoint we are connecting to
                success(details) {
                    resolve({"name":name, "details": details})
                },
                error: reject,
            });
        });
    }

    function updateUI(){

        getFileNames()
        .then((names) => {
            for (let i in names){
                if(i == 0)
                    $("#fileLogPanelTableBody").html("");
                getSummaryFromFile(names[i])
                .then((data) => {
                    updateFileLog(data);
                });
            }

            loadDropdown(names);
            console.log(names[0]);

            getCardDetails(names[0])
            .then((data) => {
                updateCardView(data);
            });

        });

    }

    updateUI();

    function updateCardView(data){
        //sostituisci contenuto
        console.log(data);

        if(data.details.status != 0){
            //TODO: add erro handling
            //gestisci errore
            handleErrorCode(data.details.status,data.name);
            return false;
        }

        let tableBody = $("#cardViewPanelTableBody");

        let cardObj = data.details.card;
        
        //emtpy the table from previous rows
        tableBody.html("");

        //add fn and datetime properties
        let fnGroup = cardObj.fn.group;
        let fnPropertyName = cardObj.fn.name;
        let fnValue = cardObj.fn.values;

        let fnRow = `
        <tr>
            <td scope="row"></td>
            <td>${fnGroup}</td>
            <td>${fnPropertyName}</td>
            <td>${fnValue}</td>
        </tr>`;

        tableBody.append(fnRow);

        let anniversary = cardObj.anniversary;
        if(!isEmpty(anniversary)){
            let values;

            if(anniversary.isText)
                values = anniversary.text;
            else{
                //datetime !text
                values = `Date: ${anniversary.date}, Time: ${anniversary.time} UTC:`;
                values += anniversary.isUTC?"yes":"no";
            }

            let row = `
            <tr>
                <td scope="row"></td>
                <td></td>
                <td>Anniversary</td>
                <td>${values}</td>
            </tr>`;

            tableBody.append(row);
        }

        let birthday = cardObj.birthday;
        if(!isEmpty(birthday)){
            let values;

            if(birthday.isText)
                values = birthday.text;
            else{
                //datetime !text
                values = `Date: ${birthday.date}, Time: ${birthday.time} UTC:`;
                values += birthday.isUTC?"yes":"no";
            }

            let row = `
            <tr>
                <td scope="row"></td>
                <td></td>
                <td>Birthday</td>
                <td>${values}</td>
            </tr>`;

            tableBody.append(row);
        }


        //add optional prop
        let optProperties = cardObj.optionalProperties;
        for(let i in optProperties){
            let group = optProperties[i].group;
            let propertyName = optProperties[i].name;
            let values = optProperties[i].values;

            let row = `
            <tr>
                <td scope="row"></td>
                <td>${group}</td>
                <td>${propertyName}</td>
                <td>${values}</td>
            </tr>`;

            tableBody.append(row);
        }

        handleErrorCode(data.summary.status,data.name);

        return true;
    }    

    function loadDropdown(names){
        let dropdown = $("#dropdownBtn");

        for(let i in names){
            dropdown.append(`<a class="dropdown-item" href="#">${names[i]}</a>`);
            if(i == 0 )
                dropdown.children(":first").addClass("active");
        }
    }

    function updateFileLog(data){
        // console.log(data);

        if(data.summary.status != 0){
            //TODO: add erro handling
            // console.log("error:"+data.summary.status);
            //gestisci errore
            handleErrorCode(data.summary.status,data.name);
            return false;
        }

        let tableBody = $("#fileLogPanelTableBody");
        let row = `<tr>
            <td><a href="/uploads/${data.name}"><u>${data.name}</u></td>
            <td>${data.summary.individual.values[0]}</td>
            <td>${data.summary.additional}</td>
        </tr>`;

        tableBody.append(row);
        handleErrorCode(data.summary.status,data.name);

        return true;

    }

    function createMessage(msg, msgType){
        //increment the notification badge number
        let num = $("#messagesBadge").text();
        num = parseInt(num) + 1;
        $("#messagesBadge").text(num);

        let alertDiv = `<div class="alert alert-${msgType} alert-dismissible fade show" role="alert">
                            <div class="text-center">${msg}</div>
                            <button type="button" class="close" data-dismiss="alert" aria-label="Close">
                                <span aria-hidden="true">&times;</span>
                            </button>
                        </div>`;
        
        if(num -1 == 0){
            $("#errorBoard").html("");
        }
        $("#errorBoard").append(alertDiv);
    }

    function handleErrorCode(error,fileName){
        let errorCode = parseInt(error);
        // console.log("called "+errorCode +" name: "+fileName);

        if(errorCode == 0)
            createMessage(`<strong>Completed:</strong> ${fileName} was succesfully loaded`,"success");
        if(errorCode == 1)
            createMessage(`<strong>Error:</strong> I was not able to open ${fileName}`,"danger");
        if (errorCode == 2)
            createMessage(`<strong>Error:</strong> ${fileName} is not a valid card!`,"danger");
        if (errorCode == 3)
            createMessage(`<strong>Warning:</strong> ${fileName} contains an invalid property!`,"warning");
        if (errorCode == 4)
            createMessage(`<strong>Warning:</strong> ${fileName} contains an invalid date!`,"warning");
        if (errorCode == 5)
            createMessage(`<strong>Warning:</strong> ${fileName} was not correctly written!`,"warning");
        if (errorCode == 6)
            createMessage(`<strong>Something went wrong!</strong> ${fileName} generated an unexpected error!`,"danger");
    }

    $(document).on("click", ".dropdown-item", function (e){
        e.preventDefault();
        $(this).parent().children().removeClass("active");
        $(this).toggleClass("active");

        getCardDetails($(this).text())
        .then((data) => {
            updateCardView(data);
        });
    });

    $(document).on("close.bs.alert",".alert",function(e){
        // console.log("closed");
        let num = $("#messagesBadge").text();
        num = parseInt(num) - 1;
        $("#messagesBadge").text(num);

        if(num == 0){
            let alertDiv = `<div class="alert alert-secondary alert-dismissible fade show" role="alert">
                                <div class="text-center"><strong>Such emptiness!</strong> your status messages will be shown here</div>
                            </div>`;

            $("#errorBoard").append(alertDiv);
        }
    });

});

function isEmpty(obj){
    return Object.keys(obj).length === 0 && obj.constructor === Object;
}


/***
 * Cimitero
 */

// On page-load AJAX Example
//  $.ajax({
//      type: 'get',            //Request type
//      dataType: 'json',       //Data type - we will use JSON for almost everything 
//      url: '/someendpoint',   //The server endpoint we are connecting to
//      success: function (data) {
//          /*  Do something with returned object
//              Note that what we get is an object, not a string, 
//              so we do not need to parse it on the server.
//              JavaScript really does handle JSONs seamlessly
//          */
//         $('#blah').html("On page load, Received string '"+JSON.stringify(data)+"' from server");
//         //We write the object to the console to show that the request was successful
//         // console.log(data); 

//     },
//     fail: function(error) {
//         // Non-200 return, do something with error
//         // console.log(error); 
//     }
// });

// function viewCard(fileName){
//     console.log(fileName);
// }



// loadDropDown();

// // Load the dropdown list requesting all vcard file names in uploads/ folder
// function loadDropDown(){

//     let ajaxRes = [];

//     $("#dropdownMenuButton").toggleClass("running");
//     $("#dropdownMenuButton").toggleClass("noClick");

//     let req = $.ajax({
//         type: 'get',            //Request type
//         dataType: 'json',       //Data type - we will use JSON for almost everything 
//         url: '/upload',   //The server endpoint we are connecting to
//         // success: myCallback,
//         // fail: function(error) {
//         //     // Non-200 return, do something with error
//         //     console.log(error); 
//         // }
//     });

//     let success = function(response){
//         // console.log("response" + response);
//         $("#dropdownMenuButton").toggleClass("running");
//         $("#dropdownMenuButton").toggleClass("noClick");

//         let dropdown = $("#dropdownBtn");

//         // console.log("loaded "+response);

//         for(let name in response){
//             dropdown.append(`<a class="dropdown-item" href="#">${response[name]}</a>`);
//             ajaxRes.push(response[name]);
//         }

//     };

//     let err = function(response){
//         console.log(response);
//         $("#dropdownMenuButton").toggleClass("running");
//         $("#dropdownMenuButton").toggleClass("noClick");
//     };

//     req.then(success, err);
//     console.log(ajaxRes);

// }

// getFileNames()
//     .then(getSummaryFromFile)
//     .then((data) => {
//         //data contains name and summary properties
//         updateUI(data)
//     });
//     .catch(showError);
//     TODO: add error handling

// let success = function(response){

//     let dropdown = $("#dropdownBtn");

//     for(let name in response){
//         dropdown.append(`<a class="dropdown-item" href="#">${response[name]}</a>`);
//         $.ajax({
//             type: 'get',
//             dataType: 'json',       //Data type - we will use JSON for almost everything 
//             url: '/summary/'+response[name],   //The server endpoint we are connecting to
//             success: function (data) {
//                 console.log(data); 

//             },
//             fail: function(error) {
//                 // Non-200 return, do something with error
//                 console.log(error); 
//             }
//         });
//     }

//     populateFileLogTable(response);

// };

// let err = function(response){
//     console.log(response);
// };

// reqFileNames.then(success,err);

// // Event listener form replacement example, building a Single-Page-App, no redirects if possible
// $('#someform').submit(function(e){
//     $('#blah').html("Callback from the form");
//     e.preventDefault();
//     //Pass data to the Ajax call, so it gets passed to the 
//     $.ajax({});
// });