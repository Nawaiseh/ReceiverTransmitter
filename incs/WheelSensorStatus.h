/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _WheelSensorStatus_H_
#define    _WheelSensorStatus_H_


#include <asn_application.h>

/* Including external dependencies */
#include <ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum WheelSensorStatus {
    WheelSensorStatus_off = 0,
    WheelSensorStatus_on = 1,
    WheelSensorStatus_notDefined = 2,
    WheelSensorStatus_notSupoprted = 3
} e_WheelSensorStatus;

/* WheelSensorStatus */
typedef ENUMERATED_t WheelSensorStatus_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_WheelSensorStatus;
asn_struct_free_f WheelSensorStatus_free;
asn_struct_print_f WheelSensorStatus_print;
asn_constr_check_f WheelSensorStatus_constraint;
ber_type_decoder_f WheelSensorStatus_decode_ber;
der_type_encoder_f WheelSensorStatus_encode_der;
xer_type_decoder_f WheelSensorStatus_decode_xer;
xer_type_encoder_f WheelSensorStatus_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _WheelSensorStatus_H_ */
