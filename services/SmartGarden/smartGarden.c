#include "smartGarden.h"

#include "protocols/uip/uip.h"
#include "protocols/uip/uip_router.h"

#include <avr/pgmspace.h>
#include <avr/io.h>

#define BUF ((struct uip_udpip_hdr *)(uip_appdata - UIP_IPUDPH_LEN))

typedef struct {
	uint8_t dI;
	uint8_t dO;
	uint16_t aI[4];
}inOut;
inOut out_buff;
const PROGMEM char howAmI[] = {"{\"DI\":4,\"AI\":4,\"DO\":8}"};

uip_ipaddr_t host_addr;
uip_udp_conn_t *rec_conn;

uint8_t host = 0;
uint8_t adc_status = 0;

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

}

void udp_ans(){

	if(!uip_newdata()) return;

	int len = strlen_P(howAmI);

	for(int i = 0 ;i<len;i++ ){
		((char *) uip_appdata)[i] = pgm_read_byte_near(howAmI + i);
	}

	uip_udp_conn_t echo_conn;
	uip_ipaddr_copy(echo_conn.ripaddr, BUF->srcipaddr);
	echo_conn.rport = BUF->srcport;
	echo_conn.lport = HTONS(5998);
	uip_ipaddr_copy(host_addr, BUF->srcipaddr);

	uip_udp_conn = &echo_conn;

	uip_slen = len;
	uip_process(UIP_UDP_SEND_CONN);
	router_output();

	uip_slen=0;
	host = 1;
}

void sendInputs(){

	if(host==0) return;

	out_buff.dI = PINA & 0x0f;
	out_buff.dO = PORTC ^ 255;

	rec_conn = uip_udp_new(host_addr,HTONS(6000),NULL);
	uip_udp_bind(rec_conn, HTONS(6000));
	
	inOut *pkt = (void *) &uip_buf[UIP_LLH_LEN+UIP_IPUDPH_LEN];
	*pkt = out_buff;
	//memcpy(uip_appdata,&out_buff,sizeof(inOut));
	uip_slen = sizeof(inOut);
	//memset(pkt, 0, uip_slen);

	rec_conn->rport = HTONS(6000);
	


	uip_udp_conn = rec_conn;

	uip_process(UIP_UDP_SEND_CONN);
	router_output();
	uip_slen=0;
	uip_udp_remove(rec_conn);
}

void convertAI(){
	//out_buff.aI[0]=230;
	if(!(ADCSRA & (1<<ADSC))){
		out_buff.aI[adc_status++] = ADC;
	    if(4==adc_status){
			adc_status = 0;
		}
		ADMUX = 0;
	    switch(adc_status){
	        case 0: ADMUX |= (1<<REFS0)|(1<<MUX2);
				//PORTC = 255 ^ 1;
				break;
	        case 1: ADMUX |= (1<<REFS0)|(1<<MUX2)|(1<<MUX0);
				//PORTC = 255 ^ 2;
				break;
			case 2: ADMUX |= (1<<REFS0)|(1<<MUX2)|(1<<MUX1);
				//PORTC = 255 ^ 3;
				break;
			case 3: ADMUX |= (1<<REFS0)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0);
				//PORTC = 255 ^ 4;
				break;
			
			}
	    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
	    ADCSRA |= (1<<ADSC);

	}
}

/*
 - Ethersex META -
 header(services/SmartGarden/smartGarden.h)
 net_init(smartGardenInit)
 timer(20, sendInputs())
 timer(1, convertAI())
*/
