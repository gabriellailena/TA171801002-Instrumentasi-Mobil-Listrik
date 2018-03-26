const mongoose = require('mongoose');
const config = require('../config/database');

// Sensor Schema
const SensorSchema = mongoose.Schema({
    time:{
        type : String
    },
    date:{
        type:String,
        required: true
    },
    lon:{
        type: String,
        required: true
    },
    lat: {
        type : String,
        required : true
    },
	suhu_bat1: {
        type : String,
        required : true
    },
	suhu_bat2: {
        type : String,
        required : true
    },
	suhu_motor: {
        type : String,
        required : true
    },
	v_batt: {
        type : String,
        required : true
    },
	soc: {
        type : String,
        required : true
    },
	arus_dc: {
        type : String,
        required : true
    },
	arus_ac: {
        type : String,
        required : true
    },
	rpm_motor: {
        type : String,
        required : true
    },
	torsi_motor: {
        type : String,
        required : true
    }
});

const Sensor = module.exports = mongoose.model('Sensor', SensorSchema);

module.exports.getSensorById = function(id, callback){
    Sensor.findById(id, callback);
}

