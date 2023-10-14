

![flow_diagram](test2.png)

<div align="center">
    <h1>
        Radio Nodes         
        <a href="https://www.gnu.org/licenses/gpl-3.0"> 
            <img src="https://img.shields.io/badge/License-GPLv3-blue.svg" alt="GPL-3.0 License">
        </a>     
    </h1>   
</div>

Solar/USB/Battery powered NRF24L01+ transceiver modules,  based on the Attiny84 microcontroller.  
Serves as the end-Node of [MQTT-Radio-Gateway](https://github.com/Peppson/MQTT-Radio-Gateway).     
Written in C++ with Arduino/PlatformIO framework.


&nbsp; 
## Project Overview
- Primarily made for personal "needs" and as a learning experience.   
  However if there's anything here that can benefit someone else's project,   
  feel free to use it!
- Easily configurable for different jobs, such as moving servos, starting pumps etc.
- Most configuration can be found in `include/config.h`.
- The `helpers` folder includes a few .ino files for calibration/setup etc.


&nbsp; 
## Hardware - PCB for Attiny84 MCU
Designed a custom PCB to fit all components more easily,  in a compact format.   
Kicad files can be found here: 
[📂 kicad - radio nodes/](./.kicad%20-%20ESP32%20shield)  
More project images in: 
[📂 images/](./images/)  

&nbsp;
![Kicad PCB img](./.images/kicad_PCB_schematic.png)   

### Finished hardware:
![Finished hardware](./.images/finished_hardware.jpg)   


&nbsp; 
## Nodes

<details>
    <summary>
        Nodes 1-5: Solar-powered self watering plants
    </summary>

Uses a 3-5V waterpump and DIY capacative fluid-level sensor.  
[Liquid Level Sensing Using Capacitive-to-Digital Converters](https://www.analog.com/en/analog-dialogue/articles/liquid-level-sensing-using-cdcs.html)
<br>   
</details>


<details>
    <summary>
        Nodes 6-10: USB-powered self watering plants
    </summary>
    Same as above but skips the TP4056 charging board and battery.   

<br> 
</details>

<details>
    <summary>
        Node 11: Mocca Master controller 
    </summary>
    Uses a simple SG90 servo and a hall-effect current sensor.     
<br>
</details>


## License 
See the [LICENSE](./LICENSE) file for more information.  
&nbsp; 

