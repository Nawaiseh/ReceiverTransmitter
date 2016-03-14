/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _BSMblob_H_
#define    _BSMblob_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BSMblob */
typedef OCTET_STRING_t BSMblob_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BSMblob;
asn_struct_free_f BSMblob_free;
asn_struct_print_f BSMblob_print;
asn_constr_check_f BSMblob_constraint;
ber_type_decoder_f BSMblob_decode_ber;
der_type_encoder_f BSMblob_encode_der;
xer_type_decoder_f BSMblob_decode_xer;
xer_type_encoder_f BSMblob_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _BSMblob_H_ */
