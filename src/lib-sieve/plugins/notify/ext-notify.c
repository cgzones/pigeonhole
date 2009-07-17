/* Copyright (c) 2002-2009 Dovecot Sieve authors, see the included COPYING file
 */

/* Extension notify
 * ----------------
 *
 * Authors: Stephan Bosch
 * Specification: draft-ietf-sieve-notify-00.txt
 * Implementation: deprecated; provided for backwards compatibility
 *                 denotify command is explicitly not supported.
 * Status: deprecated
 * 
 */
	
#include <stdio.h>

#include "sieve-common.h"

#include "sieve-code.h"
#include "sieve-extensions.h"
#include "sieve-actions.h"
#include "sieve-commands.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-interpreter.h"
#include "sieve-result.h"

#include "ext-notify-common.h"

/*
 * Operations
 */

const struct sieve_operation *ext_notify_operations[] = {
	&notify_old_operation,
	&denotify_operation
};

/* 
 * Extension
 */

static bool ext_notify_validator_load(struct sieve_validator *valdtr);

static int ext_notify_my_id = -1;

const struct sieve_extension notify_extension = { 
	"notify", 
	&ext_notify_my_id,
	NULL,
	NULL,
	ext_notify_validator_load, 
	NULL, NULL, NULL, NULL, NULL,
	SIEVE_EXT_DEFINE_OPERATIONS(ext_notify_operations),
	SIEVE_EXT_DEFINE_NO_OPERANDS,
};

/*
 * Extension validation
 */

static bool ext_notify_validator_extension_validate
	(struct sieve_validator *valdtr, void *context, 
		struct sieve_ast_argument *require_arg);

const struct sieve_validator_extension notify_validator_extension = {
	&notify_extension,
	ext_notify_validator_extension_validate,
	NULL
};

static bool ext_notify_validator_load(struct sieve_validator *valdtr)
{
	/* Register validator extension to check for conflict with enotify */
	sieve_validator_extension_register
		(valdtr, &notify_validator_extension, NULL);

	/* Register new commands */
	sieve_validator_register_command(valdtr, &cmd_notify_old);
	sieve_validator_register_command(valdtr, &cmd_denotify);
	
	return TRUE;
}

static bool ext_notify_validator_extension_validate
(struct sieve_validator *valdtr, void *context ATTR_UNUSED,
    struct sieve_ast_argument *require_arg)
{
	const struct sieve_extension *ext;

	if ( (ext=sieve_extension_get_by_name("enotify")) != NULL ) {

		/* Check for conflict with enotify */
		if ( sieve_validator_extension_loaded(valdtr, ext) ) {
			sieve_argument_validate_error(valdtr, require_arg,
				"the (deprecated) notify extension cannot be used "
				"together with the enotify extension");
			return FALSE;
		}
	}

	return TRUE;
}


