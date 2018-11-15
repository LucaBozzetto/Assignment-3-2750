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

        let num = $("#messagesBadge").text();
        num = parseInt(num);

        if(num > 0)
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

        $('#container').toggleClass("running");
        $('#container').toggleClass("noClick");



        uploadFile(form, selectedFile.name).then((resp) => {
            $('#container').toggleClass("running");
            $('#container').toggleClass("noClick");

            console.log("success");
            console.log(resp);

            $(".alert").alert('close');
            if(resp.fileAlreadyExists === false){
                updateUI();
                createMessage(`<strong>Uploaded:</strong> ${selectedFile.name} was uploaded!`,"primary");
            }
        }).catch((e) => {
            // console.log("error");
            // console.log(e);
            $("#container").toggleClass("running");
            $("#container").toggleClass("noClick");
            $(".alert").alert('close');
            if(e.status === 409){
                createMessage(`<strong>Error:</strong> ${selectedFile.name} was <strong>NOT</strong> uploaded, a file with the same name already exists!`,"primary");
                updateUI();
            }else{
                failedRequest(e);
                updateUI();
            }
        });
    });

    /***
     * AJAX calls
     */

    function uploadFile(form, name){
        return new Promise((resolve, reject) => {
            $.ajax({
                type: 'post',            //Request type
                data: form,
                global: true,
                processData: false,
                contentType: false,
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/upload',   //The server endpoint we are connecting to
                timeout: 5000,
                success: resolve,
                error: reject,
            });
        });
    }

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

            let requestes = [];

            for (let i in names){
                requestes[i] = getSummaryFromFile(names[i]);
            }

            
            // Wait for all summary to be received and the update the UI based on which files were valid
            Promise.all(requestes).then((values) => {
                let valids = [];
                let j = 0;

                // console.log(values);

                for (let i in values){
                    // console.log(values[data]);
                    if(i == 0 )
                        $("#fileLogPanelTableBody").html("");

                    if(updateFileLog(values[i]))
                        valids[j++] = values[i].name;
                }

                loadDropdown(valids);

                if(valids[0] !== undefined){
                    getCardDetails(valids[0])
                    .then((data) => {
                        updateCardView(data);
                    }).catch((e) => {
                        failedRequest(e);
                    });
                }
            }).catch((e) => {
                failedRequest(e);
            });
        }).catch((e) => {
            failedRequest(e);
        });
    }

    updateUI();

    function failedRequest(e){
        console.log(e);
        createMessage(`<strong>Something went wrong!</strong> A request generated an unexpected error(${e.status}). Please try again.`,"danger");
    }

    function updateCardView(data){
        // console.log(data);

        if(data.details.status != 0){
            return false;
        }

        $("#cardViewFileName").text(data.name);
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

        return true;
    }    

    function loadDropdown(names){
        let dropdown = $("#dropdownBtn");
        dropdown.html("");

        for(let i in names){
            dropdown.append(`<a class="dropdown-item cardViewListElement" href="#">${names[i]}</a>`);
            if(i == 0 )
                dropdown.children(":first").addClass("active");
        }
    }

    function updateFileLog(data){
        // console.log(data);

        if(data.summary.status != 0){
            // console.log("error:"+data.summary.status);

            //NOTE: an invalid file should not be displayed on the fileLogPanel
            // see TA post here https://moodle.socs.uoguelph.ca/mod/forum/discuss.php?d=1998
            handleErrorCode(data.summary.status,data.name);
            return false;
        }

        if($("#fileLogPanelEmptyPlaceholder").length > 0)
            $("#fileLogPanelTableBody").html("");

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
        let hidden = "";
        let num = $("#messagesBadge").text();
        num = parseInt(num) + 1;
        $("#messagesBadge").text(num);

        if((msgType === "success" && !($("#successFilter").hasClass("active")))
        || (msgType === "danger" && !($("#errorFilter").hasClass("active")))
        || (msgType === "warning" && !($("#warningFilter").hasClass("active")))
        || (msgType === "primary" && !($("#uploadFilter").hasClass("active")))){
            hidden = "hidden";
        }

        let alertDiv = `<div class="alert alert-${msgType} alert-dismissible fade show ${hidden}" role="alert">
                            <div class="text-center">${msg}</div>
                            <button type="button" class="close" data-dismiss="alert" aria-label="Close">
                                <span aria-hidden="true">&times;</span>
                            </button>
                        </div>`;
        
        if(num -1 == 0)
            $("#errorBoard").html("");

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

    $(document).on("click", ".cardViewListElement", function (e){
        e.preventDefault();
        let prevElem = $(this).parent().children(".active");

        $(this).parent().children().removeClass("active");
        $(this).toggleClass("active");

        if(prevElem.text() == $(this).text())
            return;

        getCardDetails($(this).text())
        .then((data) => {
            updateCardView(data);
        });
    });

    $(document).on("click", ".filterListElement", function (e){
        e.stopPropagation();
        e.preventDefault();

        $(this).toggleClass("active");
        $(this).children(".far").toggleClass("hidden");

        //hide or show
        let id = $(this).attr('id');

        if(id == "successFilter"){
            $(".alert-success").toggleClass("hidden");
        }
        if(id == "errorFilter"){
            $(".alert-danger").toggleClass("hidden");
        }
        if(id == "warningFilter"){
            $(".alert-warning").toggleClass("hidden");
        }
        if(id == "uploadFilter"){
            $(".alert-primary").toggleClass("hidden");
        }
    });

    $(document).on("close.bs.alert",".alert",function(e){
        if($("#emptyMessage").length > 0){
            return;
        }
        let num = $("#messagesBadge").text();
        num = parseInt(num) - 1;
        $("#messagesBadge").text(num);

        if(num == 0){
            let alertDiv = `<div id="emptyMessage" class="alert alert-secondary alert-dismissible fade show" role="alert">
                                <div class="text-center"><strong> Such emptiness!</strong> your status messages will be shown here</div>
                            </div>`;

            $("#errorBoard").append(alertDiv);
        }
    });

});

function isEmpty(obj){
    return Object.keys(obj).length === 0 && obj.constructor === Object;
}