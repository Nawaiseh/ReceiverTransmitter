/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _URL_Short_H_
#define    _URL_Short_H_


#include <asn_application.h>

/* Including external dependencies */
#include <IA5String.h>

#ifdef __cplusplus
extern "C" {
#endif

/* URL-Short */
typedef IA5String_t URL_Short_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_URL_Short;
asn_struct_free_f URL_Short_free;
asn_struct_print_f URL_Short_print;
asn_constr_check_f URL_Short_constraint;
ber_type_decoder_f URL_Short_decode_ber;
der_type_encoder_f URL_Short_encode_der;
xer_type_decoder_f URL_Short_decode_xer;
xer_type_encoder_f URL_Short_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _URL_Short_H_ */
