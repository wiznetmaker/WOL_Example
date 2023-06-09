#include <stdio.h>
#include "port_common.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"
#include "wizchip_conf.h"

static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 2, 102},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 2, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

#define WOL_PORT 9
const uint8_t mac_address[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
const uint8_t broadcast_ip[4] = {255, 255, 255, 255};
//const uint8_t broadcast_ip[4] = {192, 168, 2, 100};

void send_WOL_packet(uint8_t *mac_address) {
    uint8_t WOL_packet[102];
    int socket_num;
    int result;

    // Create the magic packet
    for (int i = 0; i < 6; i++) {
        WOL_packet[i] = 0xFF;
    }

    for (int i = 6; i < sizeof(WOL_packet); i++) {
        WOL_packet[i] = mac_address[(i - 6) % 6];
    }

    // Initialize a UDP socket
    socket_num = socket(0, Sn_MR_UDP, WOL_PORT, SF_IO_NONBLOCK);
    if (socket_num < 0) {
        // Handle error
        return;
    }

    //memset(WOL_packet, 'A', sizeof(WOL_packet));
    // Send the WOL packet
    result = sendto(result, WOL_packet, sizeof(WOL_packet), broadcast_ip, WOL_PORT);
    if (result < 0) {
        // Handle error
        return;
    }

    // Close the socket after the packet is sent
    close(socket_num);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);

    printf("Hello WOL_Example\r\n");
    // Initialization of W5100S Ethernet chip
    // Refer to the example provided in the SDK on how to initialize the chip
    wizchip_spi_initialize();
    wizchip_cris_initialize();
    wizchip_reset();
    wizchip_initialize();
    wizchip_check();
    network_initialize(g_net_info);
    print_network_information(g_net_info);
    
    send_WOL_packet(mac_address);

    // Other application code...

    return 0;
}
