import { Component, OnInit } from '@angular/core';
import {AuthService} from '../../services/auth.service';
import {Router} from '@angular/router';
import {FlashMessagesService} from 'angular2-flash-messages';
import 'rxjs/add/operator/catch';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})

export class LoginComponent implements OnInit {

  username: String;
  password: String;

  constructor(
    private authService: AuthService,
    private router:Router,
    private flashMessage:FlashMessagesService) { }

  ngOnInit() {
  }

  onLoginSubmit() {
    //console.log(this.username);
    const user = {
      username: this.username,
      password: this.password
    }


    this.authService.authenticateUser(user).subscribe(data =>  {
      console.log(data);
      this.authService.storeUserData(data.token, data.user);
      this.flashMessage.show('Login Successful', {cssClass: 'alert-success', timeout: 5000});
      this.router.navigate(['/dashboard']);
    }, error => {
      //console.log(error.status);
      if(error.status=="401"){
        this.flashMessage.show('Login Failed / Incorrect Username or Password', {cssClass: 'alert-danger', timeout: 5000});
        this.router.navigate(['/login']);
      }
    });
  }
}
