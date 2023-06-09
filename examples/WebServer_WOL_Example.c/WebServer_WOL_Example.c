#include <stdio.h>
#include "port_common.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "html.h"

static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 2, 102},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 2, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

#define WOL_SOCKET 0
#define HTTP_SOCKET 1

#define WOL_PORT 9
#define HTTP_PORT 80
#define DATA_BUF_SIZE 2048

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

uint8_t recv_buf[DATA_BUF_SIZE];
uint8_t send_buf[DATA_BUF_SIZE];

const uint8_t broadcast_ip[4] = {255, 255, 255, 255};

static uint8_t get_hex(uint8_t b0, uint8_t b1);
static char atonum(char ch);
static int check_hexChar(char ch);
static int parse_macAddr(char *macStr, char *macArray);

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
    int ret;
    int size;
    int data_len;
    uint8_t mac_addr[6];
    char *inputStr;
    char *inputStrEnd;
    uint8_t destip[4];
    uint8_t hexStrTemp[3];
    uint16_t destport;

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
    
    while(1)
    {
        switch(getSn_SR(HTTP_SOCKET))
        {
            case SOCK_ESTABLISHED :
                if(getSn_IR(HTTP_SOCKET) & Sn_IR_CON)
                {
                    getSn_DIPR(HTTP_SOCKET, destip);
                    destport = getSn_DPORT(HTTP_SOCKET);
                    printf("%d:Connected - %d.%d.%d.%d : %d\r\n", HTTP_SOCKET, destip[0], destip[1], destip[2], destip[3], destport);
                    setSn_IR(HTTP_SOCKET,Sn_IR_CON);
                }
                if((size = getSn_RX_RSR(HTTP_SOCKET)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
                {
                    if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                    ret = recv(HTTP_SOCKET, recv_buf, size);

                    if(ret <= 0) while(1);      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
                    size = (uint16_t) ret;
                    printf("recv data : %.*s\r\n", size, recv_buf);

                    if (strstr(recv_buf, "POST") != NULL) {
                        inputStr = strstr(recv_buf, "inputStr=");
                        if (inputStr != NULL) {
                            inputStr += strlen("inputStr=");
                            inputStrEnd = strstr(inputStr, "\r\n");
                            if (inputStrEnd != NULL) {
                                *inputStrEnd = '\0';  // Null-terminate the string
                            }
                            if (parse_macAddr(inputStr, mac_addr) < 0)
                                sprintf(send_buf, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nmac address value is wrong %s\r\n", inputStr);
                            else
                            {
                                sprintf(send_buf, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nmac address value is correct: %s\r\n", inputStr);
                                send_WOL_packet(mac_addr);
                            }
                            send(HTTP_SOCKET, send_buf, strlen(send_buf));
                        }
                    }
                    else
                    {
                        send(HTTP_SOCKET, html_str, strlen(html_str));
                    }
                    disconnect(HTTP_SOCKET);
                }
                break;
            case SOCK_CLOSE_WAIT :
                if((ret = disconnect(HTTP_SOCKET)) != SOCK_OK) while(1);
                printf("%d:Socket Closed\r\n", HTTP_SOCKET);
                break;
            case SOCK_INIT :
                printf("%d:Listen, HTTP server, port [%d]\r\n", HTTP_SOCKET, HTTP_PORT);
                if( (ret = listen(HTTP_SOCKET)) != SOCK_OK) while(1);
                break;
            case SOCK_CLOSED:
                if((ret = socket(HTTP_SOCKET, Sn_MR_TCP, HTTP_PORT, 0x00)) != HTTP_SOCKET) return ret;
                break;
            default:
                break;

        }
    }
}

static uint8_t get_hex(uint8_t b0, uint8_t b1)
{
    uint8_t buf[2];

    buf[0]   = b0;
    buf[1]   = b1;
    buf[0]   = atonum(buf[0]);
    buf[0] <<= 4;
    buf[0]  += atonum(buf[1]);
    return(buf[0]);
}

static char atonum(char ch)
{
    ch -= '0';
    if (ch > 9) ch -= 7;
    if (ch > 15) ch -= 0x20;
    return(ch);
}

static int check_hexChar(char ch)
{
    if ( ((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')) || ((ch >= 'a') && (ch <= 'f')))
        return 0;
    else
        return -1;
}

static int parse_macAddr(char *macStr, char *macArray)
{
    int data_len;

    data_len = strlen(macStr);
    printf("macStr len = %d\r\n", data_len);
    if (data_len == 12)
    {
        for(int i=0; i<6; i++)
        {                
            if (check_hexChar(macStr[i*2]) < 0)
                return -1;
            if (check_hexChar(macStr[i*2+1]) < 0)
                return -1;

            macArray[i] = get_hex(macStr[i*2], macStr[i*2+1]);
        }
        printf("\""MACSTR"\"\r\n", MAC2STR(macArray));
        return 0;
    }
    else
        return -1;
}
