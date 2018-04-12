import {Component, OnInit} from '@angular/core';
import {Observable} from 'rxjs/Observable';
import 'rxjs/add/operator/map';
import {AuthService} from '../../services/auth.service';
import {Router} from '@angular/router';


@Component({
  selector: 'app-sensor',
  templateUrl: './sensor.component.html',
  styleUrls: ['./sensor.component.css']
})

export class SensorComponent implements OnInit {
  
  constructor(private authService:AuthService, private router:Router) { }

	sensor: Object;
	page: number; //current page number
	size: number; //number of item per page
	maxpage: number; //maximum page of table view
  
  ngOnInit() {
    this.page = 0;
	this.size = 10;
	this.getSensorData();
  }

  onClickNext() {
    if (this.page < (this.maxpage - 1)) {
      this.page++;
      this.getSensorData();
    }
  }

  onClickSelectedPage(input) {
    if (input > 0 && input < (this.maxpage - 1)) {
      this.page = input - 1;
      this.getSensorData();
    }
  }

  onClickPrevious() {
    if (this.page > 0) {
      this.page--;
      this.getSensorData();
    }
  }
 
  getSensorData() {
	this.authService.getSensor(this.page, this.size).subscribe(data => {      
	  this.maxpage = Math.ceil(data[1].rows / this.size);
      this.sensor = data[0];
    }, error => {
      console.log(error);
      return false;
    });
  }
  
}
