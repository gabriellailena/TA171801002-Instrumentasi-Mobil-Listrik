const express = require('express');
const router = express.Router();
const passport = require('passport');
const jwt = require('jsonwebtoken');

const User = require('../models/user');
const config = require('../config/database');

//.Register
router.post('/register', function (req, res, next) {
    //res.send('REGISTER');
    //console.log("ini loh  "+req.body.password);
    let newUser = new User({
        name: req.body.name,
        email: req.body.email,
        username: req.body.username,
        password: req.body.password
    });

    User.addUser(newUser, (err, user) => {
        if(err){
            res.json({success: false, msg: 'Failed to register'});
        } else {
            res.json({success: true, msg: 'registered'});
        }
    })
});

//.Authenticate
router.post('/authenticate', function (req, res, next) {
    //res.send('AUTHENTICATE');
    const username = req.body.username;
    const password = req.body.password;

    User.getUserByUsername(username, (err, user)=>{
        if(err) throw err;
        if (!user) {
            return res.json({success: false, msg: 'User not found'});
        }

        User.comparePassword(password, user.password, (err, isMatch)=>{
                if(err) throw err;
                if(isMatch){
                    const token = jwt.sign({user}, config.secret,{
                        expiresIn: 604800 // 1 week
                    });

                    res.json({
                        success:true ,
                        token : 'Bearer '+token,
                        user: {
                            id: user._id,
                            name: user.name,
                            username: user.username,
                            email: user.email
                        }
                    });
                }
                else {
                    return res.json({success: false, msg: 'Wrong Credentials'});
                }
        });
    });
});

//.Profile with authenticate
router.get('/profile', passport.authenticate('jwt',{session:false}) ,function (req, res, next) {
    //res.send('PROFILE');
    res.json({user:req.user});
});

// RETURNS ALL THE USERS IN THE DATABASE
router.get('/profiles', passport.authenticate('jwt',{session:false}), function (req, res, next) {
    User.find({}, function (err, users) {
        if (err) return res.status(500).send("There was a problem finding the users.");
        res.status(200).send(users);
    });
});

// GETS A SINGLE USER FROM THE DATABASE
router.get('/profile/:id', passport.authenticate('jwt',{session:false}) , function (req, res, next) {
    User.findById(req.params.id, function (err, user) {
        if (err) return res.status(500).send("There was a problem finding the user.");
        if (!user) return res.status(404).send("No user found.");
        res.status(200).send(user);
    });
});

// DELETES A USER FROM THE DATABASE
router.delete('/profile/:id', passport.authenticate('jwt',{session:false}) , function (req, res, next) {
    User.findByIdAndRemove(req.params.id, function (err, user) {
        if (err) return res.status(500).send("There was a problem deleting the user.");
        res.status(200).send("User: "+ user.name +" was deleted.");
    });
});

// UPDATES A SINGLE USER IN THE DATABASE
router.put('/profile/:id', passport.authenticate('jwt',{session:false}) , function (req, res, next) {
    User.findByIdAndUpdate(req.params.id, req.body, {new: true}, function (err, user) {
        if (err) return res.status(500).send("There was a problem updating the user.");
        res.status(200).send(user);
    });
});


//.Validate
router.get('/validate', function (req, res, next) {
    res.send('VALIDATE');
});


module.exports = router;
