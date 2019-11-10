/*
 * Copyright (c) 2019, University of Bristol - www.bristol.ac.uk
 * Copyright (c) 2019, DTU - www.dtu.dk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "sys/etimer.h"
#include "dev/batmon-sensor.h"
#include "dev/button-hal.h"
#include "net/ipv6/simple-udp.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
/* Here we configure the logging sub-system */
#include "sys/log.h"
#define LOG_MODULE "DTU lab"
#define LOG_LEVEL LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
PROCESS(dtu_lab_process, "DTU lab process");
AUTOSTART_PROCESSES(&dtu_lab_process);
/*---------------------------------------------------------------------------*/
/* Our periodic timer */
static struct etimer et;

/* Our UDP endpoint configuration. Local and Remote */
#define UDP_PORT 32768
static struct simple_udp_connection udp_conn;

/*
 * This is the remote IPv6 address.
 * You will need your friend to tell you what to put here!
 */
static uip_ipaddr_t dest_ipaddr;

/* And this is our outgoing data buffer */
#define TX_BUF_LEN 8
static uint8_t tx_buf[TX_BUF_LEN];
/*---------------------------------------------------------------------------*/
/* This function gets called when we received a UDP message */
static void
udp_rx_callback(struct simple_udp_connection *c,
                const uip_ipaddr_t *sender_addr,
                uint16_t sender_port,
                const uip_ipaddr_t *receiver_addr,
                uint16_t receiver_port,
                const uint8_t *data,
                uint16_t datalen)
{
  int vdd_in_mv;
  int temp_in_c;
  int i;

  /*
   * Temperature in the packet is already in degrees C.
   * Just read out the respective bytes. Remember, this was in network byte
   * order.
   */
  temp_in_c = data[0] << 8 | data[1];

  /*
   * VDD in the packet was a raw value and it needs converted first.
   * Read out the respective bytes and perform the conversion.
   * Remember, this too was in network byte order.
   *
   * This conversion comes from the chip's datasheet.
   */
  vdd_in_mv = ((data[2] << 8 | data[3]) * 125) >> 5;

  LOG_INFO("RX %u bytes from ", datalen);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_(" T=%dC, VDD=%dmV, Raw payload: 0x", temp_in_c, vdd_in_mv);

  /* Print out the raw payload */
  for(i = 0; i < datalen; i++) {
    LOG_INFO_("%02X", data[i]);
  }
  LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
/* Send a message to our neighbour */
static void
udp_tx(void)
{
  int vdd;
  int temp;
  /*
   * Send a message.
   * Let's e.g. report our chip temp and input voltage.
   *
   * Each of those readings is an int. We will pack them inside the message
   * in network byte order. Temp then Voltage.
   */
  memset(&tx_buf, 0, TX_BUF_LEN);

  temp = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);
  vdd = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);

  /* First the temperature. MSB first */
  tx_buf[0] = temp >> 8;
  tx_buf[1] = temp & 0xFF;

  /* Ditto voltage */
  tx_buf[2] = vdd >> 8;
  tx_buf[3] = vdd & 0xFF;

  simple_udp_sendto(&udp_conn, tx_buf, TX_BUF_LEN, &dest_ipaddr);

  LOG_INFO("TX T=0x%02x%02x, VDD=0x%02x%02x, dest=", tx_buf[0], tx_buf[1],
           tx_buf[2], tx_buf[3]);
  LOG_INFO_6ADDR(&dest_ipaddr);
  LOG_INFO_("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(dtu_lab_process, ev, data)
{
  button_hal_button_t *btn;

  PROCESS_BEGIN();

  LOG_INFO("DTU lab process starting\n");

  /* Initialise the battery monitor sensor */
  SENSORS_ACTIVATE(batmon_sensor);

  /* Configure our periodic timer */
  etimer_set(&et, CLOCK_SECOND * 5);

  /*
   * Initialise a UDP endpoint.
   *
   * At some point we will need to figure out the remote IP and put it in here.
   *
   * Leave this alone for now until you have successfully run the example once
   * Then get your friend to give the IP of their launchpad by using
   * `ip-addr`
   * in the shell. Give them yours too.
   */
  uip_ip6addr(&dest_ipaddr, 0xfe80, 0x0000, 0x0000, 0x0000,
              0x0212, 0x4b00, 0x07b5, 0xa183);

  simple_udp_register(&udp_conn, UDP_PORT, NULL,
                      UDP_PORT, udp_rx_callback);

  while(1) {

    if(ev == PROCESS_EVENT_TIMER && etimer_expired(&et)) {
      LOG_DBG("Chip temp %dC\n",
              batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP));
      etimer_reset(&et);
    } else if(ev == button_hal_release_event) {
      btn = (button_hal_button_t *)data;
      LOG_INFO("Release event (%s)\n", BUTTON_HAL_GET_DESCRIPTION(btn));
      LOG_INFO("This was button %u, on pin %u\n", btn->unique_id, btn->pin);

      if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ZERO)) {
        /* Send a message */
        udp_tx();
      }
    }

    PROCESS_YIELD();
  }

  PROCESS_END();
}
