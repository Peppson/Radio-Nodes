
/* 
#######################  Attiny84A Pinout  #########################
#                                                                  #
#                        VCC  1|o   |14 GND                        #
#      (Tiny_serial) --- 10   2|    |13 A0/0 --- PUMP_PIN          #
#                        9    3|    |12 A1/1 --- ADC_Enable_Pin    #
#       Pullup R 10K --- RST  4|    |11 A2/2 --- RF24 CE           #
# PUMP_WATERTANK_PIN --- PB2  5|    |10 A3/3 --- RF24 CSN          #
#   ADC_Measure_PIN --- A7/7  6|    |9  A4/4 --- RF24 SCK          #
#         RF24 MISO --- A6/6  7|    |8  A5/5 --- RF24 MOSI         #
#                                                                  #
# - Pico zero pinout                                               #
# - gp0 = CE, gp1 = CSN, gp2 = SCK, gp3 = MOSI, gp4 = MISO         #
#                                                                  #
####################################################################



######################  RF24 message package  ######################
#                                                                  #
# - ATtiny84A have 2 bytes for Tx and Rx buffers...                #
# - splitting up package into small chunks of data                 #
#                                                                  #
# -- uint16_t RF24_package[6] --                                   #
#                                                                  #
# - RF24_package[0] = to_who                                       #
# - RF24_package[1] = from_who                                     #
# - RF24_package[2] = "int"                                        #
# - RF24_package[3] = "float"                                      #
# - RF24_package[4] = "bool"                                       #
# - RF24_package[5] = "time"                                       #
#                                                                  #
####################################################################



#######################  Uploding parameter  #######################
#                                                                  #
# - B.O.D                     Disabled                             #
# - Clock source              1Mhz internal                        #
# - LTO                       Disabled                             #
# - millis()/micros()         Enabled                              #
# - Pin Mapping               Counterclockwise                     #
#                                                                  #
####################################################################
*/ 


