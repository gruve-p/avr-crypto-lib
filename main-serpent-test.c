/*
 * serpent test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "serpent.h"
#include "nessie_bc_test.h"

#include <stdint.h>
#include <string.h>

char* cipher_name = "Serpent";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void serpent_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	serpent_genctx(key, keysize&0xff, ctx);
}

void testrun_nessie_serpent(void){
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 128;
	nessie_bc_ctx.name        = cipher_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(serpent_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)serpent_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)serpent_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)serpent_genctx_dummy;
	
	nessie_bc_run();
	
	nessie_bc_ctx.keysize_b   = 192;
	nessie_bc_run();
	
	nessie_bc_ctx.keysize_b   = 256;
	nessie_bc_run();
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

int main (void){
	char  str[20];
	DEBUG_INIT();
	uart_putstr("\r\n");

	uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
	uart_putstr(cipher_name);
	uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));

restart:
	while(1){ 
		if (!getnextwordn(str,20))  {DEBUG_S("DBG: W1\r\n"); goto error;}
		if (strcmp(str, "nessie")) {DEBUG_S("DBG: 1b\r\n"); goto error;}
			testrun_nessie_serpent();
		goto restart;		
		continue;
	error:
		uart_putstr("ERROR\r\n");
	}
	
}
