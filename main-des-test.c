/*
 * des test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "des.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* cipher_name = "DES";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void des_init_dummy(const void* key, uint16_t keysize_b, void* ctx){
	memcpy(ctx, key, 8);
}

void des_enc_dummy(void* buffer, void* ctx){
	des_encrypt(buffer, buffer, ctx);
} 

void des_dec_dummy(void* buffer, void* ctx){
	des_decrypt(buffer, buffer, ctx);
} 

void testrun_nessie_des(void){
	nessie_bc_init();
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   =  64;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(8);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)des_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)des_dec_dummy;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)des_init_dummy;
	
	nessie_bc_run();
}


void testrun_performance_des(void){
	uint16_t i,c;
	uint64_t t;
	char str[16];
	uint8_t key[8], data[8];
	
	
	calibrateTimer();
	getOverhead(&c, &i);
	uart_putstr_P(PSTR("\r\n\r\n=== benchmark ==="));
	utoa(c, str, 10);
	uart_putstr_P(PSTR("\r\n\tconst overhead:     "));
	uart_putstr(str);
	utoa(i, str, 10);
	uart_putstr_P(PSTR("\r\n\tinterrupt overhead: "));
	uart_putstr(str);
	
	memset(key,  0, 8);
	memset(data, 0, 8);
	
	startTimer(1);
	des_encrypt(data, data, key);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	startTimer(1);
	des_decrypt(data, data, key);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	uart_putstr_P(PSTR("\r\n"));
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

typedef void(*void_fpt)(void);

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

	PGM_P    u   = PSTR("nessie\0test\0performance\0");
	void_fpt v[] = {testrun_nessie_des, testrun_nessie_des, testrun_performance_des};

	while(1){ 
		if (!getnextwordn(str,20)){DEBUG_S("DBG: W1\r\n"); goto error;}
		if(execcommand_d0_P(str, u, v)<0){
			uart_putstr_P(PSTR("\r\nunknown command\r\n"));
		}
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
}
