#include <stdio.h>

#include "sieve-commands.h"
#include "sieve-commands-private.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-interpreter.h"

/* Exists test
 *  
 * Syntax:
 *    exists <header-names: string-list>
 */

static bool tst_exists_validate
  (struct sieve_validator *validator, struct sieve_command_context *tst);
static bool tst_exists_generate
	(struct sieve_generator *generator, struct sieve_command_context *ctx);

const struct sieve_command tst_exists = { 
	"exists", 
	SCT_TEST, 
	1, 0, FALSE, FALSE,
	NULL, 
	NULL,
	tst_exists_validate, 
	tst_exists_generate, 
	NULL 
};

/* Opcodes */

static bool tst_exists_opcode_dump
	(const struct sieve_opcode *opcode, 
		const struct sieve_dumptime_env *denv, sieve_size_t *address);
static bool tst_exists_opcode_execute
	(const struct sieve_opcode *opcode, 
		const struct sieve_runtime_env *renv, sieve_size_t *address);

const struct sieve_opcode tst_exists_opcode = { 
	"EXISTS",
	SIEVE_OPCODE_EXISTS,
	NULL, 0, 
	tst_exists_opcode_dump, 
	tst_exists_opcode_execute 
};

/* Test validation */

static bool tst_exists_validate
  (struct sieve_validator *validator, struct sieve_command_context *tst) 
{
	struct sieve_ast_argument *arg = tst->first_positional;
		
	if ( !sieve_validate_positional_argument
		(validator, tst, arg, "header names", 1, SAAT_STRING_LIST) ) {
		return FALSE;
	}
	sieve_validator_argument_activate(validator, arg);
	
	tst->data = arg;
	
	return TRUE;
}

/* Test generation */

static bool tst_exists_generate
	(struct sieve_generator *generator, struct sieve_command_context *ctx) 
{
	sieve_generator_emit_opcode(generator, &tst_exists_opcode);

 	/* Generate arguments */
    if ( !sieve_generate_arguments(generator, ctx, NULL) )
        return FALSE;	
	
	return TRUE;
}

/* Code dump */

static bool tst_exists_opcode_dump
(const struct sieve_opcode *opcode ATTR_UNUSED, 
	const struct sieve_dumptime_env *denv, sieve_size_t *address)
{
    printf("EXISTS\n");

	return
    	sieve_opr_stringlist_dump(denv, address);
}

/* Code execution */

static bool tst_exists_opcode_execute
(const struct sieve_opcode *opcode ATTR_UNUSED, 
	const struct sieve_runtime_env *renv, sieve_size_t *address)
{
	struct sieve_coded_stringlist *hdr_list;
	string_t *hdr_item;
	bool matched;
	
	printf("?? EXISTS\n");

	t_push();
		
	/* Read header-list */
	if ( (hdr_list=sieve_opr_stringlist_read(renv->sbin, address)) == NULL ) {
		t_pop();
		return FALSE;
	}
		
	/* Iterate through all requested headers to match */
	hdr_item = NULL;
	matched = FALSE;
	while ( !matched && sieve_coded_stringlist_next_item(hdr_list, &hdr_item) && 
		hdr_item != NULL ) {
		const char *const *headers;
			
		if ( mail_get_headers_utf8
			(renv->msgdata->mail, str_c(hdr_item), &headers) >= 0 && 
			headers[0] != NULL ) {	
			matched = TRUE;				 
		}
	}
	
	t_pop();
	
	sieve_interpreter_set_test_result(renv->interp, matched);
	
	return TRUE;
}
