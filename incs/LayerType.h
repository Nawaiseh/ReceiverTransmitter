/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _LayerType_H_
#define    _LayerType_H_


#include <asn_application.h>

/* Including external dependencies */
#include <ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum LayerType {
    LayerType_none = 0,
    LayerType_mixedContent = 1,
    LayerType_generalMapData = 2,
    LayerType_intersectionData = 3,
    LayerType_curveData = 4,
    LayerType_roadwaySectionData = 5,
    LayerType_parkingAreaData = 6,
    LayerType_sharedLaneData = 7
    /*
     * Enumeration is extensible
     */
} e_LayerType;

/* LayerType */
typedef ENUMERATED_t LayerType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_LayerType;
asn_struct_free_f LayerType_free;
asn_struct_print_f LayerType_print;
asn_constr_check_f LayerType_constraint;
ber_type_decoder_f LayerType_decode_ber;
der_type_encoder_f LayerType_encode_der;
xer_type_decoder_f LayerType_decode_xer;
xer_type_encoder_f LayerType_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _LayerType_H_ */
