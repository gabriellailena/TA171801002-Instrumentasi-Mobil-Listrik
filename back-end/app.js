const express = require('express');
const path = require('path');
const bodyParser = require('body-parser');
const cors = require('cors');
const passport = require('passport');
const mongoose = require('mongoose');
const config = require('./config/database');

// mongoose.createConnection(config.database);
mongoose.Promise = global.Promise;
mongoose.connect(config.database, { useMongoClient: true }).then(function (){
    console.log('Connected to MongoDB at ', config.database);
});

const app = express();
const users = require('./routers/users');
const sensors = require('./routers/sensors');

// Port
const  port = 3000;

// CORS Middleware
app.use(cors());

// Set Static Folder
app.use(express.static(path.join(__dirname,'public')));

// Body Parser Middleware
app.use(bodyParser.json());

// Passport Middleware
app.use(passport.initialize());
app.use(passport.session());

require('./config/passport')(passport);

app.use('/users', users);
app.use('/sensors', sensors);

// Index route
app.get('/', function (req, res) {
    res.send('Invalid Endpoint');
});

app.get('*', (req,res)=>{
    res.sendfile(path.join(__dirname,'public/index.html'));
});

app.listen(port, function (){
    console.log('server started on port '+ port);
});
