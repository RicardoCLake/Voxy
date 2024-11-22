#include "connexion.h"

static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;
static constexpr size_t REMOTE_PORT = 80; // standard HTTP port

NetworkInterface *net;
SocketAddress address;
TCPSocket socket;


#define thingspeak_APIkey_write "SEKFR11XGZPOQRJQ"    
#define thingspeak_APIkey_read "KL70Z84TENH9UTXL"       

nsapi_error_t response;
nsapi_size_t size;
char sbuffer[256];
char message[64];

void wifi_scan(bool verbose) 
{ 
    WiFiInterface *wifi = net->wifiInterface(); 
    WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS]; 

    int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS); 
    if (result <= 0) { 
        printf("WiFiInterface::scan() failed with return value: %d\r\n", result); 
        return; 
    } 

    if (verbose) printf("%d networks available:\n", result); 
    for (int i = 0; i < result; i++) { 
        if (verbose) printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n", 
            ap[i].get_ssid(), get_security_string(ap[i].get_security()), 
            ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2], 
            ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], 
            ap[i].get_rssi(), ap[i].get_channel()); 
    } 
    if (verbose) printf("\n"); 
}

bool resolve_hostname(char *hostname, bool verbose) 
{ 
    if (verbose) printf("Resolve hostname %s\n", hostname); 
    nsapi_size_or_error_t result = net->gethostbyname(hostname, &address); 
    if (result != 0) { 
        printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result); 
        return false; 
    } 
    if (verbose) printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") ); 
    return true; 
}

void print_network_info() 
{ 
    SocketAddress a; 
    net->get_ip_address(&a); 
    printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None"); 
    net->get_netmask(&a); 
    printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None"); 
    net->get_gateway(&a); 
    printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None"); 
} 


void init_my_connexion(bool verbose) {
    if (verbose) printf("Starting socket demo...\n"); 

    net = NetworkInterface::get_default_instance(); 
    if (!net) { 
        printf("Error! No network interface found.\r\n"); 
        return; 
    } 

    if (net->wifiInterface()) { 
        wifi_scan(verbose); 
    } 

    if (verbose) printf("Connecting to the network...\r\n"); 
    nsapi_size_or_error_t result = net->connect(); 
    if (result != 0) { 
        printf("Error! net->connect() returned: %d\r\n", result); 
        return; 
    } 

    if (verbose) print_network_info(); 

    char hostname[] = "api.thingspeak.com"; 
    if (!resolve_hostname(hostname, verbose)) { 
        return; 
    } 
    address.set_port(REMOTE_PORT);

}

void send_led_state (int switch_state, bool verbose) {
    // Préparation du message à transmettre
        sprintf(sbuffer, "GET /update?api_key=%s&field1=%d HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n", thingspeak_APIkey_write, switch_state);
        size = strlen(sbuffer);
        response = 0;

        nsapi_size_or_error_t result = socket.open(net); 
        if (result != 0) { 
            printf("Error! socket.open() returned: %d\r\n", result); 
            return; 
        } 

        result = socket.connect(address); 
        if (result != 0) { 
            printf("Error! socket.connect() returned: %d\r\n", result); 
            socket.close();
            return; 
        }

        while(size) { 
            response = socket.send(sbuffer + response, size); 
            if (response < 0) { 
                printf("Error sending data: %d\n", response); 
                socket.close(); 
                return; 
            } else { 
                size -= response; 
            } 
        } 
        if (verbose) printf("Request sent to thingspeak.com...\n");

        char rbuffer[64]; 
        response = socket.recv(rbuffer, sizeof rbuffer); 
        if (response < 0) { 
            printf("Error receiving data: %d\n", response); 
        } else { 
            if (verbose) printf("recv %d [%.*s]\n", response, strstr(rbuffer, "\r\n") - rbuffer, rbuffer); 
        } 

        ThisThread::sleep_for(100ms);
        // Fermer le socket
        socket.close();


}



