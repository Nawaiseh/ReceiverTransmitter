/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../downloads/DSRC_R36_Source.ASN"
 * 	`asn1c -fcompound-names`
 */

#ifndef    _SignalControlZone_H_
#define    _SignalControlZone_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DescriptiveName.h"
#include "SignalReqScheme.h"
#include "LaneNumber.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include "LaneWidth.h"
#include "NodeList.h"
#include <constr_SEQUENCE.h>
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SignalControlZone__data_PR {
    SignalControlZone__data_PR_NOTHING, /* No components present */
            SignalControlZone__data_PR_laneSet,
    SignalControlZone__data_PR_zones
} SignalControlZone__data_PR;

/* SignalControlZone */
typedef struct SignalControlZone {
    DescriptiveName_t *name    /* OPTIONAL */;
    SignalReqScheme_t pValue;
    struct SignalControlZone__data {
        SignalControlZone__data_PR present;
        union SignalControlZone__data_u {
            struct SignalControlZone__data__laneSet {
                A_SEQUENCE_OF(LaneNumber_t)
                list;

                /* Context for parsing across buffer boundaries */
                asn_struct_ctx_t _asn_ctx;
            } laneSet;

            struct SignalControlZone__data__zones {
                A_SEQUENCE_OF(struct SignalControlZone__data__zones__Member {
                                  struct enclosed {
                                      A_SEQUENCE_OF(LaneNumber_t)
                                      list;

                                      /* Context for parsing across buffer boundaries */
                                      asn_struct_ctx_t _asn_ctx;
                                  } *enclosed;
                                  LaneWidth_t *laneWidth    /* OPTIONAL */;
                                  NodeList_t nodeList;
                                  /*
                                   * This type is extensible,
                                   * possible extensions are below.
                                   */

                                  /* Context for parsing across buffer boundaries */
                                  asn_struct_ctx_t _asn_ctx;
                              })

                list;

                /* Context for parsing across buffer boundaries */
                asn_struct_ctx_t _asn_ctx;
            } zones;
        } choice;

        /* Context for parsing across buffer boundaries */
        asn_struct_ctx_t _asn_ctx;
    } data;
    /*
     * This type is extensible,
     * possible extensions are below.
     */

    /* Context for parsing across buffer boundaries */
    asn_struct_ctx_t _asn_ctx;
} SignalControlZone_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SignalControlZone;

#ifdef __cplusplus
}
#endif

#endif	/* _SignalControlZone_H_ */
