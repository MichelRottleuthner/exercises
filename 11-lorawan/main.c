/*
 * Copyright (C) 2025 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

#include "net/netdev.h"
#include "net/netif.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif/hdr.h"

#include "saul_reg.h"
#include "phydat.h"

#include "container.h"

#include "ztimer.h"

#define SEND_INTERVAL_SEC 5

int send_lorawan_packet(const netif_t *netif, const phydat_t *data)
{
    int result;
    gnrc_pktsnip_t *packet;
    gnrc_pktsnip_t *header;
    gnrc_netif_hdr_t *netif_header;
    uint8_t address = 1;

    /* [TASK 2.3] implement function to send data via lorawan */
}

netif_t *find_lorawan_network_interface(void)
{
    /* [TASK 2.1: implement function to identify lorawan interface here]*/
}

void join_lorawan_network(const netif_t *netif)
{
    /* [TASK 2.2: implement join function here ]*/
}

int main(void)
{
    int result;
    netif_t *netif = NULL;

    /* Sleep so that we do not miss this message while connecting */
    ztimer_sleep(ZTIMER_SEC, 3);

    /* get the on-board temperature sensor */
    saul_reg_t *btn = saul_reg_find_type(SAUL_SENSE_BTN);
    if (!btn) {
        puts("No button present");
        return 1;
    }

    /* find the LoRaWAN network interface and connect */
    netif = find_lorawan_network_interface();
    if (netif == NULL) {
        puts("No LoRaWAN network interface found");
        return -1;
    }

    join_lorawan_network(netif);

    /* record the starting time */
    ztimer_now_t last_wakeup = ztimer_now(ZTIMER_SEC);

    while (1) {
        /* read button state from saul */
        phydat_t data;
        int dimensions = saul_reg_read(btn, &data);
        if (dimensions < 1) {
            puts("Error reading a value from the device");
            break;
        }

        /* dump the read value to STDIO */
        phydat_dump(&data, dimensions);

        /* send data via LoRaWAN */
        puts("Sending data via LoRaWAN...");
        result = send_lorawan_packet(netif, &data);
        if (result != 0) {
            puts("Failed to send LoRaWAN packet");
        } else {
            printf("Sent LoRaWAN packet successfully\n");
        }

        printf("%d\n", data.val[0]);

        /* wait a bit */
        printf("Waiting for %d seconds...\n", SEND_INTERVAL_SEC);
        ztimer_periodic_wakeup(ZTIMER_SEC, &last_wakeup, SEND_INTERVAL_SEC);
    }

    return 0;
}
