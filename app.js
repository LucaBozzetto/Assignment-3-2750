'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

let isMacOs = process.platform === "darwin";

let VCardLibrary = null;

if (!isMacOs) {
	VCardLibrary = ffi.Library('./VCardLib.so', {
  	'getSummaryFromFile': [ 'string', [ 'string' ] ],
  	'getCardDetails': [ 'string', [ 'string' ] ]
	});	
} else {
	VCardLibrary = ffi.Library('./VCardLib.dylib', {
  	'getSummaryFromFile': [ 'string', [ 'string' ] ],
  	'getCardDetails': [ 'string', [ 'string' ] ]
	});	
}


// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// send about page
app.get('/about',function(req,res){
	res.sendFile(path.join(__dirname+'/public/about.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
	const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
	res.contentType('application/javascript');
	res.send(minimizedContents._obfuscatedCode);
	// res.send(contents);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

	let alreadyExists = false;
	
  	if(!req.files) {
		return res.status(400).send({fileAlreadyExists:false});
  	}
 
  	let uploadFile = req.files.uploadFile;

  	fs.access('uploads/' + uploadFile.name, fs.constants.F_OK, (err) => {
		if(!err)
			alreadyExists = true;
	});
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
	if(err) {
	  return res.status(500).send({fileAlreadyExists:alreadyExists});
	}


	// res.redirect('/');
	if(!alreadyExists)
		res.send({fileAlreadyExists: false});
	else
		res.status(409).send({fileAlreadyExists: true});
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
	// console.log(err);
	if(err == null) {
	  res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
	} else {
	  res.send('');
	}
  });
});

//******************** Your code goes here ******************** 

// Add external css
app.get('/loading.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/loading.css'));
});

app.get('/loadingbutton.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/loadingbutton.css'));
});

// Return list of all vcard filenames in uploads/
app.get('/upload', function(req , res){
  let dirPath = "uploads/";
  let files = [];

  fs.readdir(dirPath, function(err,list){
	  if(err) throw err;
	  // console.log(list);
	  for(var i=0; i<list.length; i++){
		  if(path.extname(list[i])===".vcard" || path.extname(list[i])===".vcf"){
			//   console.log(list[i]); //print the file
			  // let name = list[i].split('.').slice(0, -1).join('.');
			  let name = list[i];
			  files.push(name); //store the file name into the array files
		  }
	  }

	  res.send(files);
  });
});

app.get('/summary/:fileName',function(req,res){
	let result = VCardLibrary.getSummaryFromFile(req.params.fileName);
	
	// console.log(result);
	res.send(JSON.parse(result));
	// console.log("summary");

});

app.get('/details/:fileName',function(req,res){
	let result = VCardLibrary.getCardDetails(req.params.fileName);
	
	// console.log(result);
	res.send(JSON.parse(result));
	// console.log("details");

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);