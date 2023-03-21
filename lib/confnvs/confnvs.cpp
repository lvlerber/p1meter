using namespace std;

#include "confnvs.h"
#include <iostream>

    template <typename T>
    ConfigItem::ConfigItem(std::string  key , config_item_type_t type, bool secret, T val ) 
        : key(key)         , type(type)        , secret(secret) ,  val(val)   { }

    template <typename T>
    ConfigItem::ConfigItem(const char  *key, config_item_type_t type, bool secret, T val ) 
        : key(key)         , type(type)        , secret(secret)        , val(val) {}        


    ConfigItem::ConfigItem() :key{std::move("not defined")} , type(CONFIG_ITEM_TYPE_MAX) ,secret(false) {} 

    ConfigItem::ConfigItem(const ConfigItem &x) {
                // std::cout << "before"<<x.key<< " "<<x.type<<" ";
        key=x.key;
        type=x.type;
        secret=x.secret;
        switch (x.type) {
            case CONFIG_ITEM_TYPE_BOOL:
            // std::cout << "bool1";
                val.bool1=x.val.bool1;
                break;
            case CONFIG_ITEM_TYPE_INT8:
            // std::cout << "int8";
                val.int8=x.val.int8;
                break;
    
            case CONFIG_ITEM_TYPE_BLOB:
                //you shouldn't have default blobs
                val.bool1=x.val.bool1;
                            std::cout << "blob";
                break;
            case CONFIG_ITEM_TYPE_COLOR:
            //    std::cout << "color";             
               val.color=x.val.color;
                break;
            case CONFIG_ITEM_TYPE_IP:
            // std::cout << "ip";                
                val.ip=x.val.ip;
                break;
            case CONFIG_ITEM_TYPE_STRING:
            //  std::cout << "string " <<x.val.str;               
             val.str.assign(x.val.str);
            //  std::cout << "string after" <<x.val.str;  
                break;
            default:
            //  std::cout << "default";               // Just copying the 8 bytes that should hold all real values 
                val.int64=x.val.int64;
                // break;
        } 

        // std::cout << "after\n";
    } 

ConfigItem CONFIG_ITEMS[CONFIG_ITEM_SIZE];

void * get_conf_item_by_ref(int item_label) {
    return(&CONFIG_ITEMS[item_label].val);
}

void printValueString(int item_label){
    std::cout<< *((std::string*) get_conf_item_by_ref(CONFIG_WIFI_AP_SSID));
}


        // Admin
void printci( ConfigItem ci) {

    std::string typedesc;
    switch (ci.type) {
        case CONFIG_ITEM_TYPE_BOOL:
        typedesc="BOOL";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<ci.val.bool1<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_INT8:
        typedesc="INT8";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<std::to_string((int)ci.val.int8)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_INT16:
        typedesc="INT16";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.int16)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_INT32:
        typedesc="INT32";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.int32)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_INT64:
        typedesc="INT64";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.int64)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_UINT8:
        typedesc="UINT8";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.uint8)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_UINT16:
        typedesc="UINT16";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.uint16)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_UINT32:
        typedesc="UINT32";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.uint32)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_UINT64:
        typedesc="UINT64";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.uint64)<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_STRING:
        typedesc="STRING";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<ci.val.str<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_BLOB:
        typedesc="BLOB";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<" no blobs please  "<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_COLOR:
        typedesc="COLOR";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<ci.val.color.rgba<<std::endl;
        break;
        case CONFIG_ITEM_TYPE_IP:
        typedesc="IP";
        std::cout  <<  ci.key <<"\t"<<ci.secret <<"\t"<<typedesc<<"\t"<<to_string((int)ci.val.ip.bytes[0])<<"."<<to_string((int)ci.val.ip.bytes[1])\
                                    <<"."<<to_string((int)ci.val.ip.bytes[2]) <<"."<<to_string((int)ci.val.ip.bytes[3]) <<std::endl;
        break;
        default:
        typedesc="undetermined";
        break;
    }
}





void ConfNvs::begin(){

NVS.begin("config");

// Admin
CONFIG_ITEMS[CONFIG_ADMIN_AUTH].key = "adm_auth";
CONFIG_ITEMS[CONFIG_ADMIN_AUTH].type = CONFIG_ITEM_TYPE_INT8;
CONFIG_ITEMS[CONFIG_ADMIN_AUTH].secret = false;
CONFIG_ITEMS[CONFIG_ADMIN_AUTH].val.int8 = 0;

CONFIG_ITEMS[CONFIG_ADMIN_USERNAME].key = "adm_user";
CONFIG_ITEMS[CONFIG_ADMIN_USERNAME].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_ADMIN_USERNAME].secret = false;
CONFIG_ITEMS[CONFIG_ADMIN_USERNAME].val.str = "admin";

CONFIG_ITEMS[CONFIG_ADMIN_PASSWORD].key = "adm_pass";
CONFIG_ITEMS[CONFIG_ADMIN_PASSWORD].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_ADMIN_PASSWORD].secret = true;
CONFIG_ITEMS[CONFIG_ADMIN_PASSWORD].val.str = "";
  	
// Bluetooth,
CONFIG_ITEMS[CONFIG_BLUETOOTH_ACTIVE].key = "bt_active";
CONFIG_ITEMS[CONFIG_BLUETOOTH_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_BLUETOOTH_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_BLUETOOTH_ACTIVE].val.bool1 = false;
  	 
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_NAME].key = "bt_dev_name";
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_NAME].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_NAME].secret = false;
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_NAME].val.str = "";
  	 
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_DISCOVERABLE].key = "bt_dev_vis";
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_DISCOVERABLE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_DISCOVERABLE].secret = false;
CONFIG_ITEMS[CONFIG_BLUETOOTH_DEVICE_DISCOVERABLE].val.bool1 = true;
  	
CONFIG_ITEMS[CONFIG_BLUETOOTH_PIN_CODE].key = "bt_pin_code";
CONFIG_ITEMS[CONFIG_BLUETOOTH_PIN_CODE].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_BLUETOOTH_PIN_CODE].secret = true;
CONFIG_ITEMS[CONFIG_BLUETOOTH_PIN_CODE].val.uint16 = 1234;
  
// NTRIP,
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_ACTIVE].key = "ntr_srv_active";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_ACTIVE].val.bool1 = false;
  	
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_COLOR].key = "ntr_srv_color";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_COLOR].val.color.rgba = 0x12345655u;

CONFIG_ITEMS[CONFIG_NTRIP_SERVER_HOST].key = "ntr_srv_host";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_HOST].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_HOST].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_HOST].val.str = "";

CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PORT].key = "ntr_srv_port";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PORT].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PORT].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PORT].val.uint16 = 2101;

CONFIG_ITEMS[CONFIG_NTRIP_SERVER_MOUNTPOINT].key = "ntr_srv_mp";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_MOUNTPOINT].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_MOUNTPOINT].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_MOUNTPOINT].val.str = "";

CONFIG_ITEMS[CONFIG_NTRIP_SERVER_USERNAME].key = "ntr_srv_user";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_USERNAME].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_USERNAME].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_USERNAME].val.str = "";

CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PASSWORD].key = "ntr_srv_pass";
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PASSWORD].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PASSWORD].secret = true;
CONFIG_ITEMS[CONFIG_NTRIP_SERVER_PASSWORD].val.str = "";

CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_ACTIVE].key = "ntr_cli_active";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_ACTIVE].val.bool1 = true;
  
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_COLOR].key = "ntr_cli_color";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_COLOR].val.color.rgba = 0x00000055u;
  	 
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_HOST].key = "ntr_cli_host";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_HOST].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_HOST].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_HOST].val.str = "";
  	 
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PORT].key = "ntr_cli_port";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PORT].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PORT].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PORT].val.uint16 = 2101; 
  	
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_MOUNTPOINT].key = "ntr_cli_mp";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_MOUNTPOINT].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_MOUNTPOINT].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_MOUNTPOINT].val.str = "";
  	
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_USERNAME].key = "ntr_cli_user";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_USERNAME].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_USERNAME].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_USERNAME].val.str = "";
 
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PASSWORD].key = "ntr_cli_pass";
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PASSWORD].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PASSWORD].secret = true;
CONFIG_ITEMS[CONFIG_NTRIP_CLIENT_PASSWORD].val.str = "";
  	 
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_ACTIVE].key = "ntr_cst_active";
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_ACTIVE].val.bool1 = false;
  
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_COLOR].key = "ntr_cst_color";
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_COLOR].val.color.rgba = 0x00000055u;
  	
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PORT].key = "ntr_cst_port";
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PORT].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PORT].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PORT].val.uint16 = 2101;
  	 
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_MOUNTPOINT].key = "ntr_cst_mp";
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_MOUNTPOINT].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_MOUNTPOINT].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_MOUNTPOINT].val.str = "";

CONFIG_ITEMS[CONFIG_NTRIP_CASTER_USERNAME].key = "ntr_cst_user";
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_USERNAME].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_USERNAME].secret = false;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_USERNAME].val.str = "";

CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PASSWORD].key = "ntr_cst_pass";
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PASSWORD].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PASSWORD].secret = true;
CONFIG_ITEMS[CONFIG_NTRIP_CASTER_PASSWORD].val.str = "";
 
// Socket,
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_ACTIVE].key = "sck_srv_active";
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_ACTIVE].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_COLOR].key = "sck_srv_color";
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_COLOR].val.color.rgba = 0x00000055u;
 
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_TCP_PORT].key = "sck_srv_t_port";
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_TCP_PORT].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_TCP_PORT].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_TCP_PORT].val.uint16 = 23;
 
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_UDP_PORT].key = "sck_srv_u_port";
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_UDP_PORT].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_UDP_PORT].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_SERVER_UDP_PORT].val.uint16 = 23;
 
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_ACTIVE].key = "sck_cli_active";
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_ACTIVE].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_COLOR].key = "sck_cli_color";
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_COLOR].val.color.rgba = 0x00000055u;
 
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_HOST].key = "sck_cli_host";
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_HOST].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_HOST].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_HOST].val.str = "";
 
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_PORT].key = "sck_cli_port";
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_PORT].type = CONFIG_ITEM_TYPE_UINT16;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_PORT].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_PORT].val.uint16 = 23;
 
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_TYPE_TCP_UDP].key = "sck_cli_type";
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_TYPE_TCP_UDP].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_TYPE_TCP_UDP].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_TYPE_TCP_UDP].val.bool1 = true;
 
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_CONNECT_MESSAGE].key = "sck_cli_msg";
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_CONNECT_MESSAGE].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_CONNECT_MESSAGE].secret = false;
CONFIG_ITEMS[CONFIG_SOCKET_CLIENT_CONNECT_MESSAGE].val.str = "\n";
 
// UART,
CONFIG_ITEMS[CONFIG_UART_NUM].key = "uart_num";
CONFIG_ITEMS[CONFIG_UART_NUM].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_NUM].secret = false;
CONFIG_ITEMS[CONFIG_UART_NUM].val.uint8 = 0;
 
CONFIG_ITEMS[CONFIG_UART_TX_PIN].key = "uart_tx_pin";
CONFIG_ITEMS[CONFIG_UART_TX_PIN].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_TX_PIN].secret = false;
CONFIG_ITEMS[CONFIG_UART_TX_PIN].val.uint8 = 1;
 
CONFIG_ITEMS[CONFIG_UART_RX_PIN].key = "uart_rx_pin";
CONFIG_ITEMS[CONFIG_UART_RX_PIN].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_RX_PIN].secret = false;
CONFIG_ITEMS[CONFIG_UART_RX_PIN].val.uint8 = 3;
 
CONFIG_ITEMS[CONFIG_UART_RTS_PIN].key = "uart_rts_pin";
CONFIG_ITEMS[CONFIG_UART_RTS_PIN].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_RTS_PIN].secret = false;
CONFIG_ITEMS[CONFIG_UART_RTS_PIN].val.uint8 = 14;
 
CONFIG_ITEMS[CONFIG_UART_CTS_PIN].key = "uart_cts_pin";
CONFIG_ITEMS[CONFIG_UART_CTS_PIN].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_CTS_PIN].secret = false;
CONFIG_ITEMS[CONFIG_UART_CTS_PIN].val.uint8 = 33;
 
CONFIG_ITEMS[CONFIG_UART_BAUD_RATE].key = "uart_baud_rate";
CONFIG_ITEMS[CONFIG_UART_BAUD_RATE].type = CONFIG_ITEM_TYPE_UINT32;
CONFIG_ITEMS[CONFIG_UART_BAUD_RATE].secret = false;
CONFIG_ITEMS[CONFIG_UART_BAUD_RATE].val.uint32 = 115200;
 
CONFIG_ITEMS[CONFIG_UART_DATA_BITS].key = "uart_data_bits";
CONFIG_ITEMS[CONFIG_UART_DATA_BITS].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_DATA_BITS].secret = false;
CONFIG_ITEMS[CONFIG_UART_DATA_BITS].val.uint8 = 8;
 
CONFIG_ITEMS[CONFIG_UART_STOP_BITS].key = "uart_stop_bits";
CONFIG_ITEMS[CONFIG_UART_STOP_BITS].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_STOP_BITS].secret = false;
CONFIG_ITEMS[CONFIG_UART_STOP_BITS].val.uint8 = 1;
 
CONFIG_ITEMS[CONFIG_UART_PARITY].key = "uart_parity";
CONFIG_ITEMS[CONFIG_UART_PARITY].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_UART_PARITY].secret = false;
CONFIG_ITEMS[CONFIG_UART_PARITY].val.uint8 = 0;
 
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_RTS].key = "uart_fc_rts";
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_RTS].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_RTS].secret = false;
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_RTS].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_CTS].key = "uart_fc_cts";
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_CTS].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_CTS].secret = false;
CONFIG_ITEMS[CONFIG_UART_FLOW_CTRL_CTS].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_UART_LOG_FORWARD].key = "uart_log_fwd";
CONFIG_ITEMS[CONFIG_UART_LOG_FORWARD].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_UART_LOG_FORWARD].secret = false;
CONFIG_ITEMS[CONFIG_UART_LOG_FORWARD].val.bool1 = false;
 
// WiFi,
CONFIG_ITEMS[CONFIG_WIFI_AP_ACTIVE].key = "w_ap_active";
CONFIG_ITEMS[CONFIG_WIFI_AP_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_WIFI_AP_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_ACTIVE].val.bool1 = true;
 
CONFIG_ITEMS[CONFIG_WIFI_AP_COLOR].key = "w_ap_color";
CONFIG_ITEMS[CONFIG_WIFI_AP_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_WIFI_AP_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_COLOR].val.color.rgba = 0x00000055u;
 
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID].key = "w_ap_ssid";
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID].val.str = "Ardumower-AP";
 
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID_HIDDEN].key = "w_ap_ssid_hid";
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID_HIDDEN].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID_HIDDEN].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_SSID_HIDDEN].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_WIFI_AP_AUTH_MODE].key = "w_ap_auth_mode";
CONFIG_ITEMS[CONFIG_WIFI_AP_AUTH_MODE].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_WIFI_AP_AUTH_MODE].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_AUTH_MODE].val.uint8 = 0;//WIFI_AUTH_OPEN;
 
CONFIG_ITEMS[CONFIG_WIFI_AP_PASSWORD].key = "w_ap_pass";
CONFIG_ITEMS[CONFIG_WIFI_AP_PASSWORD].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_WIFI_AP_PASSWORD].secret = true;
CONFIG_ITEMS[CONFIG_WIFI_AP_PASSWORD].val.str = "";

CONFIG_ITEMS[CONFIG_WIFI_AP_GATEWAY].key = "w_ap_gw";
CONFIG_ITEMS[CONFIG_WIFI_AP_GATEWAY].type = CONFIG_ITEM_TYPE_IP;
CONFIG_ITEMS[CONFIG_WIFI_AP_GATEWAY].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_GATEWAY].val.ip = config_ip_addr_t(192,168,0,1);

CONFIG_ITEMS[CONFIG_WIFI_AP_SUBNET].key = "w_ap_subnet";
CONFIG_ITEMS[CONFIG_WIFI_AP_SUBNET].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_WIFI_AP_SUBNET].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_AP_SUBNET].val.uint8 = 24;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_ACTIVE].key = "w_sta_active";
CONFIG_ITEMS[CONFIG_WIFI_STA_ACTIVE].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_WIFI_STA_ACTIVE].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_ACTIVE].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_COLOR].key = "w_sta_color";
CONFIG_ITEMS[CONFIG_WIFI_STA_COLOR].type = CONFIG_ITEM_TYPE_COLOR;
CONFIG_ITEMS[CONFIG_WIFI_STA_COLOR].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_COLOR].val.color.rgba = 0x00000055u;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_SSID].key = "w_sta_ssid";
CONFIG_ITEMS[CONFIG_WIFI_STA_SSID].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_WIFI_STA_SSID].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_SSID].val.str = "";
 
CONFIG_ITEMS[CONFIG_WIFI_STA_PASSWORD].key = "w_sta_pass";
CONFIG_ITEMS[CONFIG_WIFI_STA_PASSWORD].type = CONFIG_ITEM_TYPE_STRING;
CONFIG_ITEMS[CONFIG_WIFI_STA_PASSWORD].secret = true;
CONFIG_ITEMS[CONFIG_WIFI_STA_PASSWORD].val.str = "";
 
CONFIG_ITEMS[CONFIG_WIFI_STA_SCAN_MODE_ALL].key = "w_sta_scan_mode";
CONFIG_ITEMS[CONFIG_WIFI_STA_SCAN_MODE_ALL].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_WIFI_STA_SCAN_MODE_ALL].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_SCAN_MODE_ALL].val.bool1 = true;

CONFIG_ITEMS[CONFIG_WIFI_STA_STATIC].key = "w_sta_static";
CONFIG_ITEMS[CONFIG_WIFI_STA_STATIC].type = CONFIG_ITEM_TYPE_BOOL;
CONFIG_ITEMS[CONFIG_WIFI_STA_STATIC].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_STATIC].val.bool1 = false;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_IP].key = "w_sta_ip";
CONFIG_ITEMS[CONFIG_WIFI_STA_IP].type = CONFIG_ITEM_TYPE_IP;
CONFIG_ITEMS[CONFIG_WIFI_STA_IP].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_IP].val.ip.addr = 0;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_GATEWAY].key = "w_sta_gw";
CONFIG_ITEMS[CONFIG_WIFI_STA_GATEWAY].type = CONFIG_ITEM_TYPE_IP;
CONFIG_ITEMS[CONFIG_WIFI_STA_GATEWAY].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_GATEWAY].val.ip.addr = 0;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_SUBNET].key = "w_sta_subnet";
CONFIG_ITEMS[CONFIG_WIFI_STA_SUBNET].type = CONFIG_ITEM_TYPE_UINT8;
CONFIG_ITEMS[CONFIG_WIFI_STA_SUBNET].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_SUBNET].val.uint8 = 24;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_A].key = "w_sta_dns_a";
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_A].type = CONFIG_ITEM_TYPE_IP;
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_A].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_A].val.ip.addr = 0;
 
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_B].key = "w_sta_dns_b";
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_B].type = CONFIG_ITEM_TYPE_IP;
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_B].secret = false;
CONFIG_ITEMS[CONFIG_WIFI_STA_DNS_B].val.ip.addr = 0;

}
  