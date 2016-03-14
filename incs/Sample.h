/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _Sample_H_
#define    _Sample_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Sample */
typedef struct Sample {
    long sampleStart;
    long sampleEnd;

    /* Context for parsing across buffer boundaries */
    asn_struct_ctx_t _asn_ctx;
} Sample_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Sample;

#ifdef __cplusplus
}
#endif

#endif	/* _Sample_H_ */
