const express = require('express');
const router = express.Router();
const passport = require('passport');
const jwt = require('jsonwebtoken');

const Sensor = require('../models/sensor');
const config = require('../config/database');

/*// ADDS NEW SENSOR DATA
router.post('/add', function (req, res) {
    Sensor.create({
            time : req.body.time,
            date : req.body.date,
			lon  : req.body.lon,
			lat  : req.body.lat,
			suhu_bat1 : req.body.suhu_bat1,
			suhu_bat2 : req.body.suhu_bat2,
			suhu_motor : req.body.suhu_motor,
			v_batt : req.body.v_batt,
			soc : req.body.soc,
			arus_dc: req.body.arus_dc,
			arus_ac: req.body.arus_ac,
			rpm_motor: req.body.rpm_motor,
			torsi_motor: req.body.torsi_motor
			
			
			// you can add field as you wish here.
        }, 
        function (err, sensor) {
            res.status(200).send(sensor);
            if (err) return res.status(500).send("There was a problem adding the information to the database.");
        });
});
*/

// RETURNS ALL THE SENSOR IN THE DATABASE
router.get('/getAllData', passport.authenticate('jwt',{session:false}), function (req, res, next) {  
	/*var perPage = 5;
	var page = req.params.page || 1;
	var pages;
	
	Sensor.find({}).sort({_id: -1}).skip((perPage*page) - perPage).limit(perPage).exec(function (err, sensors) {
         Sensor.count().exec(function(err, count) {
            pages = Math.ceil(count / perPage);
			if (err) return res.status(500).send("Error calculating pages.");
			res.status(200).send(sensors);
		});

	*/	
	
	Sensor.find({}).sort({_id: -1}).exec(function (err, sensors) {
		if (err) return res.status(500).send("There was a problem finding the sensors.");
		Sensor.count().exec(function(err, count) {
				var arr = [];
				var rowob = {};
				var fld = "rows";
				var num = count;
				rowob[fld] = num;
				arr.push(rowob);
				
				var data = sensors;
				arr = arr.concat(data);
	
				res.status(200).send(arr);
		});
    });
});

// GETS A SINGLE SENSOR FROM THE DATABASE
router.get('/getDataById/:id', passport.authenticate('jwt',{session:false}) , function (req, res, next) {
    Sensor.findById(req.params.id, function (err, sensor) {
        if (err) return res.status(500).send("There was a problem finding the sensor.");
        if (!sensor) return res.status(404).send("No sensor found.");
        res.status(200).send(sensor);
    });
});

// DELETES A SENSOR FROM THE DATABASE
router.delete('/deleteDataById/:id', passport.authenticate('jwt',{session:false}) , function (req, res, next) {
    Sensor.findByIdAndRemove(req.params.id, function (err, sensor) {
        if (err) return res.status(500).send("There was a problem deleting the sensor.");
        res.status(200).send("Sensor: "+ sensor.name +" was deleted.");
    });
});

// UPDATES A SINGLE SENSOR IN THE DATABASE
router.put('/updateDataById/:id', passport.authenticate('jwt',{session:false}) , function (req, res, next) {
    Sensor.findByIdAndUpdate(req.params.id, req.body, {new: true}, function (err, sensor) {
        if (err) return res.status(500).send("There was a problem updating the sensor.");
        res.status(200).send(sensor);
    });
});

module.exports = router;
