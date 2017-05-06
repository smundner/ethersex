#include "smartGarden.h"

#include "protocols/uip/uip.h"
#include "protocols/uip/uip_router.h"

#include <avr/pgmspace.h>
#include <avr/io.h>

#define BUF ((struct uip_udpip_hdr *)(uip_appdata - UIP_IPUDPH_LEN))

struct inOut {
	uint8_t dI;
	uint8_t dO;
	uint16_t aI[4];
}out_buff;

const PROGMEM char howAmI[] = {"{\"DI\":4,\"AI\":4,\"DO\":8}"};

uint8_t adc_status = 0;
uip_ipaddr_t host_addr;

void smartGardenInit(){
    uip_ipaddr_t ip;
    uip_ipaddr_copy(&ip,all_ones_addr);
    
    uip_udp_conn_t *udp_ans_conn = uip_udp_new(&ip, 0, udp_ans);
    
    if(!udp_ans_conn) return;
    
    uip_udp_bind(udp_ans_conn, HTONS(5998));
    
    uip_listen(HTONS(5999),smartGardenTcpCon);
    
    DDRA = 0x00;
    PORTA = 0x0f;
    PORTC = 0xff;
    DDRC = 0xff;

    
}

void smartGardenTcpCon(){
    if(!uip_connected() && !uip_newdata()) return;

    PORTC = 255 ^ ((char *) uip_appdata)[0];
    //uip_send(PORTC,1);
	
}

void udp_ans(){
	
	if(!uip_newdata()) return;
	
	int len = strlen_P(howAmI);
	
	for(int i = 0 ;i<len;i++ ){
		((char *) uip_appdata)[i] = pgm_read_byte_near(howAmI + i);
	}.ripaddr, BUF->srcipaddr);
	echo_conn.rport = BUF->srcport;
	echo_

	
	uip_udp_conn_t echo_conn;
	uip_ipaddr_copy(echo_connconn.lport = HTONS(5998);
	
	uip_udp_conn = &echo_conn;
	
	uip_slen = len;
	uip_process(UIP_UDP_SEND_CONN);
	router_output();
	
	uip_slen=0;
}

void sendInputs(){
	
	//if(! *host_addr) return;
	
	out_buff.dI = PINA & 0x0f;
	out_buff.dO = PORTC;
	char *out[sizeof(out_buff)];
	memcpy(out, &out_buff,sizeof(out_buff));
	
	uint8_t len = sizeof(out_buff);
	uip_ipaddr(&host_addr,192,168,0,187);
	uip_conn_t *conn = uip_connect(&host_addr,6000,NULL);
	if(uip_connected()||uip_rexmit()||uip_poll()){
		uip_send(out,len);
		uip_close();
	}
}

void convertAI(){
	static uint8_t adc_status = 0;
	if(!(ADCSRA && (1<<ADSC))){
	    ADMUX = 0;
	    switch(adc_status){
	        case 0: ADMUX |= (1<<REFS0)|(1<<MUX2);
				break;
	        case 1: ADMUX |= (1<<REFS0)|(1<<MUX2)|(1<<MUX0);
				break;
			case 2: ADMUX |= (1<<REFS0)|(1<<MUX2)|(1<<MUX1);
				break;
			case 3: ADMUX |= (1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);
				break;
			default:
				adc_status = 0;
				break;
			}
	    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
	    ADCSRA |= (1<<ADSC);
    }else{
	    out_buff.aI[adc_status++] = ADC;
	    if(3<=adc_status){
			adc_status = 0;
		}
	    	
	}
}

/*
 - Ethersex META -
 header(services/SmartGarden/smartGarden.h)
 net_init(smartGardenInit)
 timer(10, sendInputs)
 timer(1, convertAI)
*/
