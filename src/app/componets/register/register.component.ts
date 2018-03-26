import { Component, OnInit } from '@angular/core';
import {ValidateService} from '../../services/validate.service';
import {AuthService} from '../../services/auth.service';
import {FlashMessagesService} from 'angular2-flash-messages';
import {Router} from '@angular/router';
@Component({
  selector: 'app-register',
  templateUrl: './register.component.html',
  styleUrls: ['./register.component.css']
})
export class RegisterComponent implements OnInit {

  name: String;
  username: String;
  email: String;
  password: String;

  constructor(private validateService: ValidateService ,
              private flashMessage:FlashMessagesService,
              private authService: AuthService,
              private  router:Router) {}

  ngOnInit() {
  }

  onRegisterSubmit (){
    //console.log(this.name);
    const  user = {
      email: this.email,
      username: this.username,
      password: this.password,
      name: this.name
    }

    // Required Fields
    /*
    if(!this.validateService.validateRegister(user)){
        this.flashMessage.show('Please fill in all fields',{cssClass:'alert-danger', timeout:5000});
        return false;
    }
    */

    // Validate Email
    if(!this.validateService.validateEmail(user.email)){
        this.flashMessage.show('Please use valid email', {cssClass: 'alert-danger', timeout: 5000});
        return false;
    }

    // Register User
    this.authService.registerUser(user).subscribe(data =>{
        console.log("Indikator "+data.success);
        if(data.success == true){
          this.flashMessage.show('Congratulations, you are now registered', {cssClass: 'alert-success', timeout:5000});
          //console.log("Berhasil Input data");
          this.router.navigate(['/login']);
        }else {
          this.flashMessage.show('Registration failed', {cssClass: 'alert-danger', timeout:5000});
          this.router.navigate(['/register']);
          //console.log("Gagal Input data");
        }
      }, error => {
          console.log(error);
          this.flashMessage.show('Unknown Error', {cssClass: 'alert-danger', timeout:5000});
          this.router.navigate(['/register']);
      });

  }

}
