/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _Extent_H_
#define    _Extent_H_


#include <asn_application.h>

/* Including external dependencies */
#include <ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Extent {
    Extent_useInstantlyOnly = 0,
    Extent_useFor3meters = 1,
    Extent_useFor10meters = 2,
    Extent_useFor50meters = 3,
    Extent_useFor100meters = 4,
    Extent_useFor500meters = 5,
    Extent_useFor1000meters = 6,
    Extent_useFor5000meters = 7,
    Extent_useFor10000meters = 8,
    Extent_useFor50000meters = 9,
    Extent_useFor100000meters = 10,
    Extent_forever = 127
} e_Extent;

/* Extent */
typedef ENUMERATED_t Extent_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Extent;
asn_struct_free_f Extent_free;
asn_struct_print_f Extent_print;
asn_constr_check_f Extent_constraint;
ber_type_decoder_f Extent_decode_ber;
der_type_encoder_f Extent_encode_der;
xer_type_decoder_f Extent_decode_xer;
xer_type_encoder_f Extent_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _Extent_H_ */
