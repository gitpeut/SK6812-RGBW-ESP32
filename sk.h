#ifndef SK_H
#define SK_H 1


#include "driver/rmt.h"


//Use RMT for a strip of SK6812 RGBW leds



class sk{
public:
sk(void){}
~sk(void){
   
   free( _skstrip);
   rmt_driver_uninstall( skconfig.channel );
}


/*-------------------------------------------------------------------------*/
int begin( int pin, int count){

_bitcount = count*32;
_ledpin = pin;
_ledcount = count;
_brightness = 100;

int rc;

    _skstrip = (rmt_item32_t *)calloc(  _bitcount, sizeof( rmt_item32_t ) );

     skconfig.rmt_mode = RMT_MODE_TX;
     skconfig.channel = RMT_CHANNEL_7;
     skconfig.gpio_num = (gpio_num_t)_ledpin;
     skconfig.mem_block_num = 1;
     skconfig.tx_config.loop_en = 0;
     skconfig.tx_config.carrier_en = 0;
     skconfig.tx_config.idle_output_en = 1;
     skconfig.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
     skconfig.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
     skconfig.clk_div = 8; // 80MHx / 8 = 10MHz translates to  0,1 us = 100 ns per count
 
     rmt_config( &skconfig);
     rc = rmt_driver_install( skconfig.channel, 0, 0);  //  rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int rmt_intr_num)    
 
 return(rc);
}

/*-------------------------------------------------------------------------*/
uint32_t getcolor( uint8_t r,uint8_t g,uint8_t b,uint8_t w ){

uint32_t  kleur=0;

kleur |= ((uint32_t)g<<24);
kleur |= ((uint32_t)r<<16);
kleur |= ((uint32_t)b<<8);
kleur |= (uint32_t)w; 

return( kleur );
}

/*-------------------------------------------------------------------------*/

uint32_t breakcolor( uint32_t kleur, uint8_t *r,uint8_t *g,uint8_t *b,uint8_t *w ){

*g = (kleur >> 24 )&0xff;
*r = (kleur >> 16 )&0xff;
*b = (kleur >> 8 )&0xff;
*w = kleur&0xff;

return( kleur );
}


/*-------------------------------------------------------------------------*/
void color32( int led, uint32_t kleur, int brightness = -1  ){
uint8_t r,g,b,w;

breakcolor( kleur,&r,&g,&b,&w);
color( led, r,g,b,w, brightness);
  
}

/*-------------------------------------------------------------------------*/
void color( int led, uint8_t red,uint8_t green,uint8_t blue,uint8_t white, int brightness = -1  ){
uint32_t  kleur=0, bright;
uint8_t   r = red, g = green, b = blue, w = white; 
int       i,bit;

if ( led < 0 || led >= ledcount() ) return;

//Serial.printf( "---\nrgbw = %d.%d.%d.%d\n", red,green,blue,white);
// brightness is a percentage 0...100

bright = brightness;
if ( brightness == -1 ) bright = 100; 

bright = (bright * _brightness) /100;
    
r = bright*red/100;
g = bright*green/100;
b = bright*blue/100;
w = bright*white/100;

//Serial.printf("_brightness = %d, brightness %d, bright %d\n", _brightness, brightness, bright);
//Serial.printf( "rgbw = %d.%d.%d.%d\n---\n", r,g,b,w);

kleur |= ((uint32_t)g<<24);
kleur |= ((uint32_t)r<<16);
kleur |= ((uint32_t)b<<8);
kleur |= (uint32_t)w; 

//Serial.printf("Set color of led %d kleur %08X\n", led, kleur);
// sk6812 has around 600us/600us 1, 300/900us 0

for (i=(led*32),bit=0; bit<32; bit++){
     
     if ( (kleur & (1<<(31-bit)))  ) {                                                
           _skstrip[i].level0 = 1;
           _skstrip[i].duration0 = 6;
           _skstrip[i].level1 = 0;
           _skstrip[i].duration1 = 6;
      } else {
           _skstrip[i].level0 = 1;
           _skstrip[i].duration0 = 3;
           _skstrip[i].level1 = 0;
           _skstrip[i].duration1 = 9;
      }
      //if ( bit == 31 )  _skstrip[i].duration1 += 60;
      ++i;    
}  

return;
}


/*-------------------------------------------------------------------------*/
void show(){

    int rc;
    // esp_err_t rmt_write_items(rmt_channel_t channel, rmt_item32_t *rmt_item, int item_num, bool wait_tx_done)
    
    rc = rmt_write_items(skconfig.channel, _skstrip, _bitcount, 1);
    
    
    delay(2);

}

/*-------------------------------------------------------------------------*/

void clear( void ){


for ( int i =0; i < _ledcount;++i ) {
  color(i,0,0,0,0);
}

show();
delay(10);

}

/*-------------------------------------------------------------------------*/
uint8_t   setbrightness(uint8_t newbrightness ){
  _brightness = newbrightness;
  return( _brightness );
}
/*-------------------------------------------------------------------------*/
uint8_t   getbrightness(){
  return( _brightness );
}
/*-------------------------------------------------------------------------*/

int ledcount(){
return( _ledcount);
}

/*-------------------------------------------------------------------------*/

private:
int   _ledcount;
int   _bitcount;
int   _ledpin;
int   _brightness;
rmt_item32_t* _skstrip = NULL;
rmt_config_t skconfig;
float _realtick;

};

#endif
