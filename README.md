# Radio Nodes 
The Radio Node is an ATtiny84-based module equipped with an NRF24L01+ radio.  
It serves as the "end node" in the [MQTT-Radio-Gateway](https://github.com/Peppson/MQTT-Radio-Gateway) network.   
Can run directly on 2-7V, a small solarpanel or approximately 50 days with a small battery (3000mAh).  
Or why not all three? >:)

- [Pcb](#Pcb)
- [Schematic](#Schematic)
- [Parts used](#Parts)



&nbsp;
## Features

- **Low powered**: Nodes use about 12 mW in active mode (at 3.3V with 3.5 mA).
- **Deepsleep**: Power consumption reduced to 29.7 μW (at 3.3V with 9 μA) while asleep.
- **Flexible config**: Utilizes the "Config.h" file for easy customization of "Node tasks".
- **Real-time clock sync**: Enables timekeeping across nodes by syncing their clocks.
- **EEPROM usage**: Stores data in the EEPROM, ensuring data persistence in the event of power loss or self reset.
- **Logging**: Nodes continuously monitor and transmit their chosen sensor values.






&nbsp;
## Pcb
&nbsp;
![img](https://github.com/Peppson/Radio-Node/blob/main/Images/Node_Pcb.png)



&nbsp;
## Schematic
&nbsp;
![img](https://github.com/Peppson/Vattna_Bara/blob/main/Node_Schematic.png)
&nbsp;




## Parts used
- Generic Solar Panel 5-6V
- SPDT Switch
- TP4056 Module [Link](https://www.amazon.se/ZkeeShop-laddningsmodul-litiumbatteri-laddningskort-skyddsfunktion/dp/B08BZP283B/ref=sr_1_5?crid=26YZ4CVG0ZU1C&keywords=TP4056&qid=1684667989&sprefix=tp4056%2Caps%2C87&sr=8-5)
- 3.3V LDO MCP1700-3302E 
- PN2222A (NPN) 
- 2N3904 (NPN) 
- 2N3906 (PNP)
- Ceramic Capacitors 1uf, 100nf
- Electrolytic Capacitor 22uf, 10uf 
- Resistors 
- Terminal Blocks 
- Waterpump 3-5V [Link](https://www.amazon.se/-/en/ICQUANZX-Submersible-1-2-1-6L-0-3-0-8m-Aquarium/dp/B088LQ4C1Z/ref=sr_1_6?crid=1BOGNJLGL3Y88&keywords=vattenpump+3v&qid=1684668502&sprefix=vattenpump+3v%2Caps%2C115&sr=8-6)
- ATtiny84A MCU
- Dil Socket 14 
- NRF24L01 [Link](https://www.amazon.se/-/en/AZDelivery-Compatible-NRF24L01-Wireless-Raspberry/dp/B075DBDS3J/ref=sr_1_5?keywords=nrf24l01&qid=1684668788&sprefix=nrf24%2Caps%2C91&sr=8-5&th=1)
&nbsp;




