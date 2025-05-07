/*
 * Copyright (C) 2025 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */
#include <stdio.h>
#include "ztimer.h"
#include "shell.h"
#include "host/util/util.h"
#include "net/bluetil/ad.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"

static uint8_t id_addr_type;

static const char adv_name[] = "Sense-M8";

/* The first two bytes of the manufacturer specific data type contain
 * a company ID code which for a final product must be requested from
 * the Bluetooth SIG. 
 * For testing purposes we use 0xFFFF as an unassigned identifier code.
 */
static const uint8_t _company_id_code[] = { 0xFF, 0xFF };

/* We use the manufacturer specific data type to transmit arbitrary data.
 * After the company ID we always put the same marker pattern to be able
 * to easily filter for these packets.
 */
static const uint8_t _custom_msd_marker_pattern[] = {
        0xf0, 0x9f, 0x93, 0x9f
};

#define TX_POWER_UNDEF (127)

#define ADV_PKT_BUFFER_SIZE (300)
#define NIMBLE_INSTANCE (0)

static uint8_t _payload_buf[ADV_PKT_BUFFER_SIZE];
static unsigned _pl_len = 0;

static void _ad_append(bluetil_ad_t *ad, const uint8_t *data, unsigned len)
{
    if ((ad->pos + len) <= ad->size) {
        memcpy(&ad->buf[ad->pos], data, len);
        ad->pos += len;
    }
}

/* hand-craft a manufacturer specific data type with a custom marker
 * at the start of the data */
static void _ad_append_marked_msd_payload(bluetil_ad_t *ad, const uint8_t *payload, unsigned len)
{
    uint8_t msd_len = sizeof(_company_id_code) + 1 +
                      sizeof(_custom_msd_marker_pattern) + len;
    uint8_t data_type = BLE_GAP_AD_VENDOR;

    /* set the size field */
    _ad_append(ad, &msd_len, sizeof(msd_len));

    /* set the data type */
    _ad_append(ad, &data_type, sizeof(data_type));

    /* set the company id code */
    _ad_append(ad, _company_id_code, sizeof(_company_id_code));

    /* set the marker */
    _ad_append(ad, _custom_msd_marker_pattern, sizeof(_custom_msd_marker_pattern));

    /* set the payload */
    _ad_append(ad, payload, len);
}

static void start_adv(uint8_t *payload, unsigned payload_len)
{
    /* buffer for the advertisement */
    static uint8_t adv_buf[ADV_PKT_BUFFER_SIZE];
    struct os_mbuf *data;
    int rc;
    struct ble_gap_ext_adv_params params;

    /* advertising data struct */
    static bluetil_ad_t ad;

    /* use defaults for non-set params */
    memset (&params, 0, sizeof(params));

    /* advertise using ID addr */
    params.own_addr_type = id_addr_type;

    params.primary_phy = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power = TX_POWER_UNDEF;
    params.sid = 0;

    /* configure the nimble instance */
    rc = ble_gap_ext_adv_configure(NIMBLE_INSTANCE, &params, NULL, NULL, NULL);
    assert (rc == 0);

    /* get mbuf for adv data */
    data = os_msys_get_pkthdr(ADV_PKT_BUFFER_SIZE, 0);
    assert(data);

    /* build advertising data with flags to specifiy that:
     * - the device is a BLE device (instead of BR/EDR a.k.a. bluetooth classic)
     * - the device is not discoverable */
    rc = bluetil_ad_init_with_flags(&ad, adv_buf, sizeof(adv_buf),
                                    BLE_GAP_FLAG_BREDR_NOTSUP);
    assert(rc == BLUETIL_AD_OK);
    
    /* give the device a name that is included in the advertisements */
    rc = bluetil_ad_add_name(&ad, adv_name);
    assert(rc == BLUETIL_AD_OK);
 
    /* Add a manufacturer spcific data entry with custom marker. */
    _ad_append_marked_msd_payload(&ad, payload, payload_len);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, ad.buf, ad.pos);
    assert(rc == 0);

    rc = ble_gap_ext_adv_set_data(NIMBLE_INSTANCE, data);
    assert (rc == 0);

    /* start advertising */
    rc = ble_gap_ext_adv_start(NIMBLE_INSTANCE, 0, 0);
    assert (rc == 0);

    printf("instance %u started (non-con non-scan)\n", NIMBLE_INSTANCE);
}

int adv_cmd(int argc, char **argv)
{
    /* check that the command is called correctly */
    if (argc != 2) {
        puts("usage: adv <message>");
        return 1;
    }

    puts(argv[1]);
 
    if (ble_gap_ext_adv_active(NIMBLE_INSTANCE)) {
        ble_gap_ext_adv_stop(NIMBLE_INSTANCE);
    }
    
    _pl_len = strlen(argv[1]);

    /* update the payload with the given message */
    memcpy(_payload_buf, argv[1], _pl_len);
    
    start_adv(_payload_buf, _pl_len);

    return 0;
}

SHELL_COMMAND(adv,"set advertised message",adv_cmd);

int main(void)
{
    /* Sleep so that we do not miss this message while connecting */
    ztimer_sleep(ZTIMER_SEC, 3);
    puts("BLE extended advertisement app!");
    
    int rc;
    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);
    /* configure global address */
    rc = ble_hs_id_infer_auto(0, &id_addr_type);
    assert(rc == 0);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
/** @} */
