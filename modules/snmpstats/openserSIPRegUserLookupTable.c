/*
 * SNMPStats Module
 * Copyright (C) 2006 SOMA Networks, INC.
 * Written by: Jeffrey Magder (jmagder@somanetworks.com)
 *
 * This file is part of opensips, a free SIP server.
 *
 * opensips is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * opensips is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 * USA
 *
 * History:
 * --------
 * 2006-11-23 initial version (jmagder)
 *
 * Note: this file originally auto-generated by mib2c using
 * mib2c.array-user.conf
 *
 * This file implements the openserSIPRegUserLookupTable.  For a full
 * description of the table, please see the OPENSER-SIP-SERVER-MIB.
 *
 * This file is much larger and more complicated than the files for other
 * tables.  This is because the table is settable, bringing a lot of SNMP
 * overhead.  Most of the file consists of the original auto-generated
 * code (aside from white space and comment changes).
 *
 * The functions that have been modified to implement this table are the
 * following:
 *
 * 1) openserSIPRegUserLookupTable_extract_index()
 *
 *    - Modified to fail if the index is invalid.  The index is invalid if it
 *      does not match up with the global userLookupCounter.
 *
 * 2) openserSIPRegUserLookupTable_can_[activate|deactivate|delete]()
 *
 *    - Simplified to always allow activation/deactivation/deletion.
 *
 * 3) openserSIPRegUserLookupTable_set_reserve1()
 *
 *    - The reserve1 phase passes if the row is new, and the rowStatus column
 *      is being set to 'createAndGo'
 *    - The reserve1 phase passes if the row is not new, and the rowStatus
 *      column is being set to 'destroy'
 *
 * 4) openserSIPRegUserLookupTable_set_action()
 *
 *    - The function was modified to populate the row with the userIndex of the
 *      supplied URI if that URI was found on the system, and set the rowStatus
 *      to 'active'.  If the URI was not found, the rowStatus is set to
 *      'notInService'
 *
 * You can safely ignore the other functions.
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>

#include "openserSIPRegUserLookupTable.h"
#include "snmpstats_globals.h"
#include "hashTable.h"
#include "interprocess_buffer.h"

static netsnmp_handler_registration *my_handler = NULL;
static netsnmp_table_array_callbacks cb;

oid openserSIPRegUserLookupTable_oid[] = {
	openserSIPRegUserLookupTable_TABLE_OID };

size_t openserSIPRegUserLookupTable_oid_len =
	OID_LENGTH(openserSIPRegUserLookupTable_oid);


/*
 * Initializes the openserSIPRegUserLookupTable table.  This step is easier
 * than in the other tables because there is no table population.  All table
 * population takes place during run time.
 */
void init_openserSIPRegUserLookupTable(void)
{
	initialize_table_openserSIPRegUserLookupTable();
}

/* the *_row_copy routine */
static int openserSIPRegUserLookupTable_row_copy(
		openserSIPRegUserLookupTable_context * dst,
		openserSIPRegUserLookupTable_context * src)
{
	if(!dst||!src)
	{
		return 1;
	}


	/* copy index, if provided */
	if(dst->index.oids)
	{
		free(dst->index.oids);
	}

	if(snmp_clone_mem( (void*)&dst->index.oids, src->index.oids,
				src->index.len * sizeof(oid) )) {
		dst->index.oids = NULL;
		return 1;
	}

	dst->index.len = src->index.len;

	/* Copy out almost all components of the structure.  We don't copy out
	 * the openserSIPRegUSerLookupURI (or its length).  */
	dst->openserSIPRegUserLookupIndex     = src->openserSIPRegUserLookupIndex;
	dst->openserSIPRegUserIndex           = src->openserSIPRegUserIndex;
	dst->openserSIPRegUserLookupRowStatus =
		src->openserSIPRegUserLookupRowStatus;

	return 0;
}

/*
 * the *_extract_index routine. (Mostly auto-generated)
 *
 * This routine is called when a set request is received for an index
 * that was not found in the table container. Here, we parse the oid
 * in the individual index components and copy those indexes to the
 * context. Then we make sure the indexes for the new row are valid.
 *
 * It has been modified from its original form in that if the indexes are
 * invalid, then they aren't returned.  An index is invalid if:
 *
 *   1) It is < 1
 *   2) It doesn't match the global userLookupIndex. (As per MIB specs)
 *
 */
int openserSIPRegUserLookupTable_extract_index(
		openserSIPRegUserLookupTable_context * ctx,
		netsnmp_index * hdr)
{
	/*
	 * temporary local storage for extracting oid index
	 *
	 * extract index uses varbinds (netsnmp_variable_list) to parse
	 * the index OID into the individual components for each index part.
	 */
	netsnmp_variable_list var_openserSIPRegUserLookupIndex;
	int err;

	/* copy index, if provided */
	if(hdr) {
		netsnmp_assert(ctx->index.oids == NULL);
		if((hdr->len > MAX_OID_LEN) ||
		   snmp_clone_mem( (void*)&ctx->index.oids, hdr->oids,
						   hdr->len * sizeof(oid) )) {
			return -1;
		}
		ctx->index.len = hdr->len;
	} else
		return -1;

	/* Set up the index */
	memset(&var_openserSIPRegUserLookupIndex, 0x00,
			sizeof(var_openserSIPRegUserLookupIndex));

	var_openserSIPRegUserLookupIndex.type = ASN_UNSIGNED;
	var_openserSIPRegUserLookupIndex.next_variable = NULL;


	/* parse the oid into the individual index components */
	err = parse_oid_indexes( hdr->oids, hdr->len,
			&var_openserSIPRegUserLookupIndex );

	if (err == SNMP_ERR_NOERROR) {

		/* copy index components into the context structure */
		ctx->openserSIPRegUserLookupIndex =
			*var_openserSIPRegUserLookupIndex.val.integer;

		/*
		 * Check to make sure that the index corresponds to the
		 * global_userLookupCounter, as per the MIB specifications.
		 */
		if (*var_openserSIPRegUserLookupIndex.val.integer !=
				global_UserLookupCounter ||
		    *var_openserSIPRegUserLookupIndex.val.integer < 1) {
			err = -1;
		}

	}

	/* parsing may have allocated memory. free it. */
	snmp_reset_var_buffers( &var_openserSIPRegUserLookupIndex );

	return err;
}

/*
 * This is an auto-generated function.  In general the *_can_activate routine
 * is called when a row is changed to determine if all the values set are
 * consistent with the row's rules for a row status of ACTIVE.  If not, then 0
 * can be returned to prevent the row status from becomming final.
 *
 * For our purposes, we have no need for this check, so we always return 1.
 */
int openserSIPRegUserLookupTable_can_activate(
		openserSIPRegUserLookupTable_context *undo_ctx,
		openserSIPRegUserLookupTable_context *row_ctx,
		netsnmp_request_group * rg)
{
	return 1;
}

/*
 * This is an auto-generated function.  In general the *_can_deactivate routine
 * is called when a row that is currently ACTIVE is set to a state other than
 * ACTIVE. If there are conditions in which a row should not be allowed to
 * transition out of the ACTIVE state (such as the row being referred to by
 * another row or table), check for them here.
 *
 * Since this table has no reason why this shouldn't be allowed, we always
 * return 1;
 */
int openserSIPRegUserLookupTable_can_deactivate(
		openserSIPRegUserLookupTable_context *undo_ctx,
		openserSIPRegUserLookupTable_context *row_ctx,
		netsnmp_request_group * rg)
{
	return 1;
}

/*
 * This is an auto-generated function.  In general the *_can_delete routine is
 * called to determine if a row can be deleted.  This usually involved checking
 * if it can be deactivated, and if it can be, then checking for other
 * conditions.
 *
 * Since this table ha no reason why row deletion shouldn't be allowed, we
 * always return 1, unless we can't deactivate.
 */
int openserSIPRegUserLookupTable_can_delete(
		openserSIPRegUserLookupTable_context *undo_ctx,
		openserSIPRegUserLookupTable_context *row_ctx,
		netsnmp_request_group * rg)
{
	if(openserSIPRegUserLookupTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
		return 0;

	return 1;
}

/*
 * This is an auto-generated function.
 *
 * The *_create_row routine is called by the table handler to create a new row
 * for a given index. This is the first stage of the row creation process.  The
 * *_set_reserve_* functions can be used to prevent the row from being inserted
 * into the table even if the row passes any preliminary checks set here.
 *
 * Returns a newly allocated openserSIPRegUserLookupTable_context structure (a
 * row in the table) if the indexes are legal.  NULL will be returned otherwise.
 */
openserSIPRegUserLookupTable_context *
openserSIPRegUserLookupTable_create_row( netsnmp_index* hdr)
{
	openserSIPRegUserLookupTable_context * ctx =
		SNMP_MALLOC_TYPEDEF(openserSIPRegUserLookupTable_context);

	if (!ctx)
	{
		return NULL;
	}

	/*
	 * Extract the index.  The function has been modified from its original
	 * auto-generated version in that the function will fail if index is
	 * somehow invalid.
	 */
	if(openserSIPRegUserLookupTable_extract_index( ctx, hdr ))
	{

		if (NULL != ctx->index.oids)
		{
			free(ctx->index.oids);
		}

		free(ctx);

		return NULL;
	}

	ctx->openserSIPRegUserLookupURI       = NULL;
	ctx->openserSIPRegUserLookupURI_len   = 0;
	ctx->openserSIPRegUserIndex           = 0;
	ctx->openserSIPRegUserLookupRowStatus = 0;

	return ctx;
}

/*
 * Auto-generated function.  The *_duplicate row routine
 */
openserSIPRegUserLookupTable_context *
openserSIPRegUserLookupTable_duplicate_row(
		openserSIPRegUserLookupTable_context * row_ctx)
{
	openserSIPRegUserLookupTable_context * dup;

	if(!row_ctx)
		return NULL;

	dup = SNMP_MALLOC_TYPEDEF(openserSIPRegUserLookupTable_context);
	if(!dup)
		return NULL;

	if(openserSIPRegUserLookupTable_row_copy(dup,row_ctx)) {
		free(dup);
		dup = NULL;
	}

	return dup;
}

/*
 * The *_delete_row method is auto-generated, and is called to delete a row.
 *
 * This will not be called if earlier checks said that this row can't be
 * deleted.  However, in our implementation there is never a reason why this
 * function can't be called.
 */
netsnmp_index * openserSIPRegUserLookupTable_delete_row(
		openserSIPRegUserLookupTable_context * ctx )
{
	if(ctx->index.oids)
	{
		free(ctx->index.oids);
	}


	if (ctx->openserSIPRegUserLookupURI != NULL)
	{
		pkg_free(ctx->openserSIPRegUserLookupURI);
	}

	free( ctx );

	return NULL;
}


/*
 * Large parts of this function have been auto-generated.  The functions purpose
 * is to check to make sure all SNMP set values for the given row, have been
 * valid.  If not, then the process is supposed to be aborted.  Otherwise, we
 * pass on to the *_reserve2 function.
 *
 * For our purposes, our only check is to make sure that either of the following
 * conditions are true:
 *
 * 1) If this row already exists, then the SET request is setting the rowStatus
 *    column to 'destroy'.
 *
 * 2) If this row does not already exist, then the SET request is setting the
 *    rowStatus to 'createAndGo'.
 *
 * Since the MIB specified there are to be no other modifications to the row,
 * any other condition is considered illegal, and will result in an SNMP error
 * being returned.
 */
void openserSIPRegUserLookupTable_set_reserve1( netsnmp_request_group *rg )
{
	openserSIPRegUserLookupTable_context *row_ctx =
		(openserSIPRegUserLookupTable_context *)rg->existing_row;

	netsnmp_variable_list *var;

	netsnmp_request_group_item *current;

	int rc;

	for( current = rg->list; current; current = current->next ) {

		var = current->ri->requestvb;
		rc = SNMP_ERR_NOERROR;

		switch(current->tri->colnum)
		{

			case COLUMN_OPENSERSIPREGUSERLOOKUPURI:

				if (row_ctx->openserSIPRegUserLookupRowStatus == 0 ||
				    row_ctx->openserSIPRegUserLookupRowStatus ==
				    TC_ROWSTATUS_NOTREADY) {

				} else {
					rc = SNMP_ERR_BADVALUE;
				}

				break;

			case COLUMN_OPENSERSIPREGUSERLOOKUPROWSTATUS:

				/** RowStatus = ASN_INTEGER */
				rc = netsnmp_check_vb_type_and_size(var, ASN_INTEGER,
						sizeof(
						row_ctx->openserSIPRegUserLookupRowStatus));

				/* Want to make sure that if it already exists that it
				 * is setting it to 'destroy', or if it doesn't exist,
				 * that it is setting it to 'createAndGo' */
				if (row_ctx->openserSIPRegUserLookupRowStatus == 0 &&
				    *var->val.integer != TC_ROWSTATUS_CREATEANDGO)
				{
					rc = SNMP_ERR_BADVALUE;
				}

				else if (row_ctx->openserSIPRegUserLookupRowStatus ==
						TC_ROWSTATUS_ACTIVE &&
						*var->val.integer !=
						TC_ROWSTATUS_DESTROY)
				{
					rc = SNMP_ERR_BADVALUE;
				}

			break;

			default: /** We shouldn't get here */
				rc = SNMP_ERR_GENERR;
				snmp_log(LOG_ERR, "unknown column in openserSIPReg"
						  "UserLookupTable_set_reserve1\n");
		}

		if (rc)
		{
			netsnmp_set_mode_request_error(
					MODE_SET_BEGIN, current->ri, rc );
		}

		rg->status = SNMP_MAX( rg->status, current->ri->status );
	}

}

/*
 * Auto-generated function.  The function is supposed to check for any
 * last-minute conditions not being met.  However, we don't have any such
 * conditions, so we leave the default function as is.
 */
void openserSIPRegUserLookupTable_set_reserve2( netsnmp_request_group *rg )
{
	openserSIPRegUserLookupTable_context *undo_ctx =
		(openserSIPRegUserLookupTable_context *)rg->undo_info;

	netsnmp_request_group_item *current;

	int rc;

	rg->rg_void = rg->list->ri;

	for( current = rg->list; current; current = current->next ) {

		rc = SNMP_ERR_NOERROR;

		switch(current->tri->colnum)
		{
			case COLUMN_OPENSERSIPREGUSERLOOKUPURI:
				break;

			case COLUMN_OPENSERSIPREGUSERLOOKUPROWSTATUS:

				/** RowStatus = ASN_INTEGER */
				rc = netsnmp_check_vb_rowstatus(current->ri->requestvb,
					undo_ctx ?
					undo_ctx->openserSIPRegUserLookupRowStatus:0);

				rg->rg_void = current->ri;

				break;

			default: /** We shouldn't get here */
				netsnmp_assert(0); /** why wasn't this caught in reserve1? */
		}

		if (rc)
		{
			netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc);
		}
	}

}

/*
 * This function is called only when all the *_reserve[1|2] functions were
 * succeful.  Its purpose is to make any changes to the row before it is
 * inserted into the table.
 *
 * In the case of this table, this involves looking up the index of the
 * requested user in the URI to userIndex mapping hash table.  If the result is
 * found, the index will be copied to the row, and the rowStatus set to
 * 'active'.  Otherwise, the row status will be set to 'notInService'
 *
 * All other handling is auto-generated.
 */
void openserSIPRegUserLookupTable_set_action( netsnmp_request_group *rg )
{
	/* First things first, we need to consume the interprocess buffer, in
	 * case something has changed. We want to return the freshest data. */
	consumeInterprocessBuffer();

	aorToIndexStruct_t *hashRecord;

	netsnmp_variable_list *var;

	openserSIPRegUserLookupTable_context *row_ctx =
		(openserSIPRegUserLookupTable_context *)rg->existing_row;

	openserSIPRegUserLookupTable_context *undo_ctx =
		(openserSIPRegUserLookupTable_context *)rg->undo_info;

	netsnmp_request_group_item *current;

	int			row_err = 0;

	/* Copy the actual data to the row. */
	for( current = rg->list; current; current = current->next ) {

		var = current->ri->requestvb;

		switch(current->tri->colnum)
		{

			case COLUMN_OPENSERSIPREGUSERLOOKUPURI:

				row_ctx->openserSIPRegUserLookupURI =
					pkg_malloc(sizeof(char)*(var->val_len + 1));

				memcpy(row_ctx->openserSIPRegUserLookupURI,
						var->val.string,
						var->val_len);

				/* Usually NetSNMP won't terminate strings with '\0'.
				 * The hash function expect them to be terminated
				 * though, so we have to add this on to the end.  The +1
				 * in the malloc makes sure of the extra space for us.
				 */
				row_ctx->openserSIPRegUserLookupURI[var->val_len] = '\0';

				row_ctx->openserSIPRegUserLookupURI_len = var->val_len;

				/* Do the lookup.  If we could find the record, then set
				 * the index and the row status to active.  Otherwise,
				 * set the row to notInService */
				hashRecord =
					findHashRecord(hashTable,
						(char *)
						row_ctx->openserSIPRegUserLookupURI,
						HASH_SIZE);

				if (hashRecord == NULL)
				{
					row_ctx->openserSIPRegUserIndex = 0;
					row_ctx->openserSIPRegUserLookupRowStatus =
						TC_ROWSTATUS_NOTINSERVICE;
				}
				else
				{
					row_ctx->openserSIPRegUserIndex =
						hashRecord->userIndex;
					row_ctx->openserSIPRegUserLookupRowStatus =
						TC_ROWSTATUS_ACTIVE;
				}

				break;

			case COLUMN_OPENSERSIPREGUSERLOOKUPROWSTATUS:

				row_ctx->openserSIPRegUserLookupRowStatus =
					*var->val.integer;

				if (*var->val.integer == TC_ROWSTATUS_CREATEANDGO)
				{
					rg->row_created = 1;
					/* Set to NOT READY until the lookup URI has
					 * been supplied. */
					row_ctx->openserSIPRegUserLookupRowStatus =
						TC_ROWSTATUS_NOTREADY;
				}
				else if (*var->val.integer == TC_ROWSTATUS_DESTROY)
				{
					rg->row_deleted = 1;
				}
				else
				{
					/* We should never be here, because the RESERVE
					 * functions should have taken care of all other
					 * values. */
				LM_ERR("invalid RowStatus in openserSIPStatusCodesTable\n");
				}

				break;

			default: /** We shouldn't get here */
				netsnmp_assert(0); /** why wasn't this caught in reserve1? */
		}
	}

	/*
	 * done with all the columns. Could check row related
	 * requirements here.
	 */
#ifndef openserSIPRegUserLookupTable_CAN_MODIFY_ACTIVE_ROW
	if( undo_ctx && RS_IS_ACTIVE(undo_ctx->openserSIPRegUserLookupRowStatus) &&
		row_ctx && RS_IS_ACTIVE(row_ctx->openserSIPRegUserLookupRowStatus) ) {
		LM_DBG("row error!\n");
	}
#endif

	/*
	 * check activation/deactivation
	 */
	row_err = netsnmp_table_array_check_row_status(&cb, rg,
			row_ctx ? &row_ctx->openserSIPRegUserLookupRowStatus : NULL,
			undo_ctx ? &undo_ctx->openserSIPRegUserLookupRowStatus : NULL);

	if(row_err) {
		netsnmp_set_mode_request_error(MODE_SET_BEGIN,
				(netsnmp_request_info*)rg->rg_void, row_err);
		return;
	}

}

/*
 * The COMMIT phase is used to do any extra processing after the ACTION phase.
 * In our table, there is nothing to do, so the function body is empty.
 */
void openserSIPRegUserLookupTable_set_commit( netsnmp_request_group *rg )
{

}


/*
 * This function is called if the *_reserve[1|2] calls failed.  Its supposed to
 * free up any resources allocated earlier.  However, we already take care of
 * all these resources in earlier functions.  So for our purposes, the function
 * body is empty.
 */
void openserSIPRegUserLookupTable_set_free( netsnmp_request_group *rg )
{

}


/*
 * This function is called if an ACTION phase fails, to do extra clean-up work.
 * We don't have anything complicated enough to warrant putting anything in this
 * function.  Therefore, its just left with an empty function body.
 */
void openserSIPRegUserLookupTable_set_undo( netsnmp_request_group *rg )
{

}


/*
 * Initialize the openserSIPRegUserLookupTable table by defining how it is
 * structured.
 *
 * This function is mostly auto-generated.
 */
void initialize_table_openserSIPRegUserLookupTable(void)
{
	netsnmp_table_registration_info *table_info;

	if(my_handler) {
		snmp_log(LOG_ERR, "initialize_table_openserSIPRegUserLookup"
				"Table_handler called again\n");
		return;
	}

	memset(&cb, 0x00, sizeof(cb));

	/** create the table structure itself */
	table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

	my_handler = netsnmp_create_handler_registration(
			"openserSIPRegUserLookupTable",
			netsnmp_table_array_helper_handler,
			openserSIPRegUserLookupTable_oid,
			openserSIPRegUserLookupTable_oid_len,
			HANDLER_CAN_RWRITE);

	if (!my_handler || !table_info) {
		snmp_log(LOG_ERR, "malloc failed in "
				 "initialize_table_openserSIPRegUserLookup"
				 "Table_handler\n");
		if (table_info)
			SNMP_FREE(table_info);
		return; /** mallocs failed */
	}

	/*
	 * Setting up the table's definition
	 */

	netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

	table_info->min_column = openserSIPRegUserLookupTable_COL_MIN;
	table_info->max_column = openserSIPRegUserLookupTable_COL_MAX;

	/*
	 * registering the table with the master agent
	 */
	cb.get_value = openserSIPRegUserLookupTable_get_value;
	cb.container = netsnmp_container_find(
			"openserSIPRegUserLookupTable_primary:"
			"openserSIPRegUserLookupTable:"
			"table_container");

	cb.can_set = 1;
	cb.create_row    =
		(UserRowMethod*)openserSIPRegUserLookupTable_create_row;
	cb.duplicate_row =
		(UserRowMethod*)openserSIPRegUserLookupTable_duplicate_row;
	cb.delete_row    =
		(UserRowMethod*)openserSIPRegUserLookupTable_delete_row;
	cb.row_copy      =
		(Netsnmp_User_Row_Operation *)
			openserSIPRegUserLookupTable_row_copy;

	cb.can_activate   = (Netsnmp_User_Row_Action *)
		openserSIPRegUserLookupTable_can_activate;
	cb.can_deactivate = (Netsnmp_User_Row_Action *)
		openserSIPRegUserLookupTable_can_deactivate;
	cb.can_delete     = (Netsnmp_User_Row_Action *)
		openserSIPRegUserLookupTable_can_delete;

	cb.set_reserve1 = openserSIPRegUserLookupTable_set_reserve1;
	cb.set_reserve2 = openserSIPRegUserLookupTable_set_reserve2;
	cb.set_action   = openserSIPRegUserLookupTable_set_action;
	cb.set_commit   = openserSIPRegUserLookupTable_set_commit;
	cb.set_free     = openserSIPRegUserLookupTable_set_free;
	cb.set_undo     = openserSIPRegUserLookupTable_set_undo;

	DEBUGMSGTL(("initialize_table_openserSIPRegUserLookupTable",
				"Registering table openserSIPRegUserLookupTable "
				"as a table array\n"));

	netsnmp_table_container_register(my_handler, table_info, &cb,
			cb.container, 1);
}

/*
 * This function was auto-generated and didn't need modifications from its
 * auto-generation.  It is called to handle an SNMP GET request.
 */
int openserSIPRegUserLookupTable_get_value(
			netsnmp_request_info *request,
			netsnmp_index *item,
			netsnmp_table_request_info *table_info )
{
	netsnmp_variable_list *var = request->requestvb;

	openserSIPRegUserLookupTable_context *context =
		(openserSIPRegUserLookupTable_context *)item;

	switch(table_info->colnum)
	{
		case COLUMN_OPENSERSIPREGUSERLOOKUPURI:
			/** SnmpAdminString = ASN_OCTET_STR */
			snmp_set_var_typed_value(var, ASN_OCTET_STR,
					(unsigned char*)
					context->openserSIPRegUserLookupURI,
					context->openserSIPRegUserLookupURI_len);
		break;

		case COLUMN_OPENSERSIPREGUSERINDEX:
			/** UNSIGNED32 = ASN_UNSIGNED */
			snmp_set_var_typed_value(var, ASN_UNSIGNED,
					 (unsigned char*)
					 &context->openserSIPRegUserIndex,
					 sizeof(context->openserSIPRegUserIndex));
		break;

		case COLUMN_OPENSERSIPREGUSERLOOKUPROWSTATUS:
			/** RowStatus = ASN_INTEGER */
			snmp_set_var_typed_value(var, ASN_INTEGER,
					(unsigned char*)
					&context->openserSIPRegUserLookupRowStatus,
					sizeof(
					context->openserSIPRegUserLookupRowStatus));
		break;

	default: /** We shouldn't get here */
		snmp_log(LOG_ERR, "unknown column in "
				 "openserSIPRegUserLookupTable_get_value\n");
		return SNMP_ERR_GENERR;
	}

	return SNMP_ERR_NOERROR;
}

/*
 * openserSIPRegUserLookupTable_get_by_idx
 */
const openserSIPRegUserLookupTable_context *
openserSIPRegUserLookupTable_get_by_idx(netsnmp_index * hdr)
{
	return (const openserSIPRegUserLookupTable_context *)
		CONTAINER_FIND(cb.container, hdr );
}


