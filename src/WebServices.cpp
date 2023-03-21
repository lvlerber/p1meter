/*
 * This file is inspired on the ESP32-XBee distribution (https://github.com/nebkat/esp32-xbee).
 * Copyright (c) 2019 Nebojsa Cvetkovic.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "WebServices.h"
#include <algorithm> // std::min
#include <cJSON.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "confnvs.h"
#include <bits/stdc++.h>
#include <SD.h>
	
#include <base64.h>
#define DEBUG
#define BUFFER_SIZE 2048
#define CONFIG_VALUE_UNCHANGED "\x1a\x1a\x1a\x1a\x1a\x1a\x1a\x1a"
static const char *TAG = "WEB";
enum auth_method
{
    AUTH_METHOD_OPEN = 0,
    AUTH_METHOD_HOTSPOT = 1,
    AUTH_METHOD_BASIC = 2
};
static const char *basic_authentication;
static enum auth_method auth_method;

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

// Set HTTP response content type according to file extension
/// this is probably included in the FS version of beginResponse!!!
static String set_content_type_from_file(AsyncWebServerRequest *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".html"))
    {
        return "text/html";
    }
    else if (IS_FILE_EXT(filename, ".js"))
    {
        return "application/javascript";
    }
    else if (IS_FILE_EXT(filename, ".css"))
    {
        return "text/css";
    }
    else if (IS_FILE_EXT(filename, ".ico"))
    {
        return "image/x-icon";
    }
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return "text/plain";
}

/* Copies the full path into destination buffer and returns
 * pointer to path (skipping the preceding base path) */
static char *get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    const char *quest = strchr(uri, '?');
    if (quest)
    {
        pathlen = std::min((int)pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash)
    {
        pathlen = std::min((int)pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize)
    {
        // Full path string won't fit into destination buffer
        return NULL;
    }

    // Construct full path (base + path)
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri, pathlen + 1);

    // Return pointer to path, skipping the base
    return dest + base_pathlen;
}

static esp_err_t basic_auth(AsyncWebServerRequest *request)
{

    if (request->hasHeader("Authorization"))
    {
        AsyncWebHeader *h = request->getHeader("Authorization");
        //   Serial.printf("MyHeader: %s\n", h->value().c_str());
        bool authenticated = strcasecmp(basic_authentication, h->value().c_str()) == 0;
        if (authenticated)
            return ESP_OK;
    }
    else
    {
        AsyncWebServerResponse *response = request->beginResponse(401, "text/html", "401 Unauthorized - Incorrect or no password provided");
        response->addHeader("WWW-Authenticate", "Basic realm=\"ESP32 Ardumower Config\"");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t hotspot_auth(AsyncWebServerRequest *req)
{
    // int sock = httpd_req_to_sockfd(req);

    // struct sockaddr_in6 client_addr;
    // socklen_t socklen = sizeof(client_addr);
    // getpeername(sock, (struct sockaddr *)&client_addr, &socklen);

    // // TODO: Correctly read IPv4?
    // // ERROR_ACTION(TAG, client_addr.sin6_family != AF_INET, goto _auth_error, "IPv6 connections not supported, IP family %d", client_addr.sin6_family);

    // wifi_sta_list_t *ap_sta_list = wifi_ap_sta_list();
    // esp_netif_sta_list_t esp_netif_ap_sta_list;
    // esp_netif_get_sta_list(ap_sta_list, &esp_netif_ap_sta_list);

    //  TODO: Correctly read IPv4?
    // for (int i = 0; i < esp_netif_ap_sta_list.num; i++) {
    //     if (esp_netif_ap_sta_list.sta[i].ip.addr == client_addr.sin6_addr.un.u32_addr[3]) return ESP_OK;
    // }

    // //_auth_error:
    // httpd_resp_set_status(req, "401"); // Unauthorized
    // char *unauthorized = "401 Unauthorized - Configured to only accept connections from hotspot devices";
    // httpd_resp_send(req, unauthorized, strlen(unauthorized));
    return ESP_FAIL;
}

static esp_err_t check_auth(AsyncWebServerRequest *req)
{
    if (auth_method == AUTH_METHOD_HOTSPOT)
        return hotspot_auth(req);
    if (auth_method == AUTH_METHOD_BASIC)
        return basic_auth(req);
    return ESP_OK;
}
/*
static esp_err_t log_get_handler(httpd_req_t *req) {
    if (check_auth(req) == ESP_FAIL) return ESP_FAIL;

    httpd_resp_set_type(req, "text/plain");

    size_t length;
    void *log_data = log_receive(&length, 1);
    if (log_data == NULL) {
        httpd_resp_sendstr(req, "");

        return ESP_OK;
    }

    httpd_resp_send(req, log_data, length);

    log_return(log_data);

    return ESP_OK;
}

static esp_err_t core_dump_get_handler(httpd_req_t *req) {
    if (check_auth(req) == ESP_FAIL) return ESP_FAIL;

    size_t core_dump_size = core_dump_available();
    if (core_dump_size == 0) {
        httpd_resp_sendstr(req, "No core dump available");
        return ESP_OK;
    }

    httpd_resp_set_type(req, "application/octet-stream");

    const esp_app_desc_t *app_desc = esp_ota_get_app_description();

    char elf_sha256[7];
    esp_ota_get_app_elf_sha256(elf_sha256, sizeof(elf_sha256));

    time_t t = time(NULL);
    char date[20] = "\0";
    if (t > 315360000l) strftime(date, sizeof(date), "_%F_%T", localtime(&t));

    char content_disposition[128];
    snprintf(content_disposition, sizeof(content_disposition),
            "attachment; filename=\"esp32_xbee_%s_core_dump_%s%s.bin\"", app_desc->version, elf_sha256, date);
    httpd_resp_set_hdr(req, "Content-Disposition", content_disposition);

    for (int offset = 0; offset < core_dump_size; offset += BUFFER_SIZE) {
        size_t read = core_dump_size - offset;
        if (read > BUFFER_SIZE) read = BUFFER_SIZE;

        core_dump_read(offset, buffer, read);
        httpd_resp_send_chunk(req, buffer, read);
    }

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}
*/
static esp_err_t heap_info_get_handler(AsyncWebServerRequest *request)
{
    if (check_auth(request) == ESP_FAIL)
        return ESP_FAIL;

    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &root = response->getRoot();
    root["total_free_bytes", info.total_free_bytes];
    root["total_allocated_bytes", info.total_allocated_bytes];
    root["largest_free_block", info.largest_free_block];
    root["minimum_free_bytes", info.minimum_free_bytes];
    root["allocated_blocks", info.allocated_blocks];
    root["free_blocks", info.free_blocks];
    root["total_blocks", info.total_blocks];
    response->setLength();
    request->send(response);
    return ESP_OK;
}

static esp_err_t file_check_etag_hash(AsyncWebServerRequest *req, char *file_hash_path, char *etag, size_t etag_size)
{
    /*    struct stat file_hash_stat;
       if (stat(file_hash_path, &file_hash_stat) == -1) {
           // Hash file not created yet
           return ESP_ERR_NOT_FOUND;
       }

       FILE *fd_hash = fopen(file_hash_path, "r+");

       // Ensure hash file was opened
       ERROR_ACTION(TAG, fd_hash == NULL, return ESP_FAIL,
               "Could not open hash file %s (%lu bytes) for reading/updating: %d %s", file_hash_path,
               file_hash_stat.st_size, errno, strerror(errno));

       // Read existing hash
       uint32_t crc;
       int read = fread(&crc, sizeof(crc), 1, fd_hash);
       fclose(fd_hash);
       ERROR_ACTION(TAG, read != 1, return ESP_FAIL,
               "Could not read hash file %s: %d %s", file_hash_path,
               errno, strerror(errno));

       snprintf(etag, etag_size, "\"%08X\"", crc);

       // Compare to header sent by client
       size_t if_none_match_length = httpd_req_get_hdr_value_len(req, "If-None-Match") + 1;
       if (if_none_match_length > 1) {
           char *if_none_match = malloc(if_none_match_length);
           httpd_req_get_hdr_value_str(req, "If-None-Match", if_none_match, if_none_match_length);

           bool header_match = strcmp(etag, if_none_match) == 0;
           free(if_none_match);

           // Matching ETag, return not modified
           if (header_match) {
               return ESP_OK;
           } else {
               ESP_LOGW(TAG, "ETag for file %s sent by client does not match (%s != %s)", file_hash_path, etag, if_none_match);
               return ESP_ERR_INVALID_CRC;
           }
       }
    */
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t file_get_handler(AsyncWebServerRequest *request)
{
    
    /*
    if (check_auth(req) == ESP_FAIL) return ESP_FAIL;

    char file_path[FILE_PATH_MAX - strlen(FILE_HASH_SUFFIX)];
    char file_hash_path[FILE_PATH_MAX];
    FILE *fd = NULL, *fd_hash = NULL;
    struct stat file_stat;

    // Extract filename from URL
    char *file_name = get_path_from_uri(file_path, WWW_PARTITION_PATH, req->uri, sizeof(file_path));
    ERROR_ACTION(TAG, file_name == NULL, {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }, "Filename too long")

    // If name has trailing '/', respond with index page
    if (file_name[strlen(file_name) - 1] == '/' && strlen(file_name) + strlen("index.html") < FILE_PATH_MAX) {
        strcpy(&file_name[strlen(file_name)], "index.html");

        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    }

    set_content_type_from_file(req, file_name);

    // Check if file exists
    ERROR_ACTION(TAG, stat(file_path, &file_stat) == -1, {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }, "Could not stat file %s", file_path)

    // Check file hash (if matches request, file is not modified)
    strcpy(file_hash_path, file_path);
    strcpy(&file_hash_path[strlen(file_hash_path)], FILE_HASH_SUFFIX);
    char etag[8 + 2 + 1] = ""; // Store CRC32, quotes and \0
    if (file_check_etag_hash(req, file_hash_path, etag, sizeof(etag)) == ESP_OK) {
        httpd_resp_set_status(req, "304 Not Modified");
        httpd_resp_send(req, NULL, 0);
        return ESP_OK;
    }

    if (strlen(etag) > 0) httpd_resp_set_hdr(req, "ETag", etag);

    fd = fopen(file_path, "r");
    ERROR_ACTION(TAG, fd == NULL, {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Could not read file");
        return ESP_FAIL;
    }, "Could not read file %s", file_path)

    ESP_LOGI(TAG, "Sending file %s (%ld bytes)...", file_name, file_stat.st_size);

    // Retrieve the pointer to scratch buffer for temporary storage
    size_t length;
    uint32_t crc = 0;
    do {
        // Read file in chunks into the scratch buffer
        length = fread(buffer, 1, BUFFER_SIZE, fd);

        // Send the buffer contents as HTTP response chunk
        if (httpd_resp_send_chunk(req, buffer, length) != ESP_OK) {
            ESP_LOGE(TAG, "Failed sending file %s", file_name);
            httpd_resp_sendstr_chunk(req, NULL);

            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");

            fclose(fd);
            return ESP_FAIL;
        }

        // Update checksum
        crc = crc32_le(crc, (const uint8_t *)buffer, length);
    } while (length != 0);

    // Close file after sending complete
    fclose(fd);

    // Store CRC hash
    fd_hash = fopen(file_hash_path, "w");
    if (fd_hash != NULL) {
        fwrite(&crc, sizeof(crc), 1, fd_hash);
        fclose(fd_hash);
    } else {
        ESP_LOGW(TAG, "Could not open hash file %s for writing: %d %s", file_hash_path, errno, strerror(errno));
    }

 */ return ESP_OK;
};

static esp_err_t config_get_handler(AsyncWebServerRequest *request)
{
    if (check_auth(request) == ESP_FAIL)
        return ESP_FAIL;

    AsyncJsonResponse *response = new AsyncJsonResponse();

    JsonVariant &root = response->getRoot();

    // const esp_app_desc_t *app_desc = esp_ota_get_app_description();
    // root[ "version", app_desc->version];

    for (ConfigItem item : CONFIG_ITEMS)
    {

        int64_t int64 = 0;
        uint64_t uint64 = 0;

        size_t length = 0;
        char *string = NULL;
        String value;
        config_color_t color;
        config_ip_addr_t ip(0, 0, 0, 0);
        uint8_t *blobvalue = NULL;
        if (item.type == CONFIG_ITEM_TYPE_STRING)
        {
            value = NVS.getString(item.key);
        }
        else if (item.type == CONFIG_ITEM_TYPE_BLOB)
        {
            // Get length
            length = NVS.getBlobSize(item.key);
            // Get value
            string = (char *)calloc(1, length + 1);
            NVS.getBlob(item.key, (uint8_t *)string, length);
            string[length] = '\0';

            // this is most likely not going to work, as a blob may contain characters
            // unsuited for json.
            // this should be base64 encoded
        }
        else if (item.type == CONFIG_ITEM_TYPE_COLOR)
        {
            color.rgba = (uint32_t)NVS.getInt(item.key);
            // Convert to hex
            asprintf(&string, "#%02x%02x%02x", color.values.red, color.values.green, color.values.blue);
        }
        else if (item.type == CONFIG_ITEM_TYPE_IP)
        {
            ip.addr = (uint32_t)NVS.getInt(item.key);
            JsonVariant ipjson = root.createNestedArray(item.key);
            for (int b = 0; b < 4; b++)
            {
                ipjson.add(ip.bytes[b]);
            }
        }
        else if (item.type == CONFIG_ITEM_TYPE_UINT8 || item.type == CONFIG_ITEM_TYPE_UINT16 || item.type == CONFIG_ITEM_TYPE_UINT32 || item.type == CONFIG_ITEM_TYPE_UINT64)
        {
            NVS.getInt(item.key);
            asprintf(&string, "%llu", uint64);
        }
        else if (item.type == CONFIG_ITEM_TYPE_BOOL || item.type == CONFIG_ITEM_TYPE_INT8 || item.type == CONFIG_ITEM_TYPE_INT16 || item.type == CONFIG_ITEM_TYPE_INT32 || item.type == CONFIG_ITEM_TYPE_INT64)
        {
            NVS.getInt(item.key);
            asprintf(&string, "%lld", int64);
        }
        else
        {
            string = (char *)calloc(1, 1);
        }

        if (string != NULL)
        {
            value = string;
            free(string);
        }
        if (value != "")
        {
            // Hide secret values that aren't empty
            if (item.secret)
            {
                value = CONFIG_VALUE_UNCHANGED;
            }
            root[item.key] = value;
        }
    }
    response->setLength();
    request->send(response);
    return ESP_OK;
}

ArJsonRequestHandlerFunction config_post_handler(AsyncWebServerRequest *request, JsonVariant &json)
{
    for (ConfigItem item : CONFIG_ITEMS)
    {
        String strval = "";
        switch (item.type)
        {
        case CONFIG_ITEM_TYPE_STRING:
        case CONFIG_ITEM_TYPE_BLOB:
            strval = json[item.key].as<String>();
            if ((strval != "") && (strval != CONFIG_VALUE_UNCHANGED))
            {
                if (item.type == CONFIG_ITEM_TYPE_STRING)
                {
                    NVS.setString(item.key, strval);
                }
                else
                {
                    NVS.setBlob(item.key, (uint8_t *)strval.c_str(), strval.length());
                }
            }
            break;
        case CONFIG_ITEM_TYPE_COLOR:
        {
            strval = json[item.key].as<String>();
            config_color_t color;
            if (strval == "#000000")
            {
                color.rgba = 0;
            }
            else
            {
                strval = strval.substring(2) + "55";
                color.rgba = std::stoul(strval.c_str(), 0, 16);
            }
            NVS.setInt(item.key, color.rgba);
            break;
        }
        case CONFIG_ITEM_TYPE_IP:
        {
            config_ip_addr_t ip(0, 0, 0, 0);
            for (int i = 0; i < 4; i++)
            {
                ip.bytes[i] = json[item.key][i];
            }
            NVS.setInt(item.key, ip.addr);
            break;
        }
        case CONFIG_ITEM_TYPE_BOOL:
            NVS.setInt(item.key, json[item.key].as<int8_t>());
            break;
        case CONFIG_ITEM_TYPE_INT8:
            NVS.setInt(item.key, json[item.key].as<int8_t>());
            break;
        case CONFIG_ITEM_TYPE_INT16:
            NVS.setInt(item.key, json[item.key].as<int16_t>());
            break;
        case CONFIG_ITEM_TYPE_INT32:
            NVS.setInt(item.key, json[item.key].as<int32_t>());
            break;
        case CONFIG_ITEM_TYPE_INT64:
            NVS.setInt(item.key, json[item.key].as<int64_t>());
            break;
        case CONFIG_ITEM_TYPE_UINT8:
            NVS.setInt(item.key, json[item.key].as<uint8_t>());
            break;
        case CONFIG_ITEM_TYPE_UINT16:
            NVS.setInt(item.key, json[item.key].as<uint16_t>());
            break;
        case CONFIG_ITEM_TYPE_UINT32:
            NVS.setInt(item.key, json[item.key].as<uint32_t>());
            break;
        case CONFIG_ITEM_TYPE_UINT64:
            NVS.setInt(item.key, json[item.key].as<uint64_t>());
            break;

        default:
            break;
        }
    }
    NVS.commit();
    // config_restart();
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &root = response->getRoot();
    root["success"] = true;

    response->setLength();
    request->send(response);
    return ESP_OK;
};


static esp_err_t status_get_handler(AsyncWebServerRequest *request)
{
    if (check_auth(request) == ESP_FAIL){
        return ESP_FAIL;        
    }

    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &root = response->getRoot();
 
    // Uptime
    root[ "uptime"]= (int)((double)esp_timer_get_time() / 1000000);
    // Heap

    root["heap"]["total"]= heap_caps_get_total_size(MALLOC_CAP_8BIT);
    root["heap"]["free"], heap_caps_get_free_size(MALLOC_CAP_8BIT);

   /*  // Streams
    cJSON *streams = cJSON_AddObjectToObject(root, "streams");
    stream_stats_values_t values;
    for (stream_stats_handle_t stats = stream_stats_first(); stats != NULL; stats = stream_stats_next(stats))
    {
        stream_stats_values(stats, &values);

        cJSON *stream = cJSON_AddObjectToObject(streams, values.name);
        cJSON *total = cJSON_AddObjectToObject(stream, "total");
        cJSON_AddNumberToObject(total, "in", values.total_in);
        cJSON_AddNumberToObject(total, "out", values.total_out);
        cJSON *rate = cJSON_AddObjectToObject(stream, "rate");
        cJSON_AddNumberToObject(rate, "in", values.rate_in);
        cJSON_AddNumberToObject(rate, "out", values.rate_out);
    }

    // WiFi
    wifi_ap_status_t ap_status;
    wifi_sta_status_t sta_status;

    wifi_ap_status(&ap_status);
    wifi_sta_status(&sta_status);

    cJSON *wifi = cJSON_AddObjectToObject(root, "wifi");

    cJSON *ap = cJSON_AddObjectToObject(wifi, "ap");
    cJSON_AddBoolToObject(ap, "active", ap_status.active);
    if (ap_status.active)
    {
        cJSON_AddStringToObject(ap, "ssid", (char *)ap_status.ssid);
        cJSON_AddStringToObject(ap, "authmode", wifi_auth_mode_name(ap_status.authmode));
        cJSON_AddNumberToObject(ap, "devices", ap_status.devices);

        char ip[40];
        snprintf(ip, sizeof(ip), IPSTR, IP2STR(&ap_status.ip4_addr));
        cJSON_AddStringToObject(ap, "ip4", ip);
        snprintf(ip, sizeof(ip), IPV6STR, IPV62STR(ap_status.ip6_addr));
        cJSON_AddStringToObject(ap, "ip6", ip);
    }

    cJSON *sta = cJSON_AddObjectToObject(wifi, "sta");
    cJSON_AddBoolToObject(sta, "active", ap_status.active);
    if (sta_status.active)
    {
        cJSON_AddBoolToObject(sta, "connected", sta_status.connected);
        if (sta_status.connected)
        {
            cJSON_AddStringToObject(sta, "ssid", (char *)sta_status.ssid);
            cJSON_AddStringToObject(sta, "authmode", wifi_auth_mode_name(sta_status.authmode));
            cJSON_AddNumberToObject(sta, "rssi", sta_status.rssi);

            char ip[40];
            snprintf(ip, sizeof(ip), IPSTR, IP2STR(&sta_status.ip4_addr));
            cJSON_AddStringToObject(sta, "ip4", ip);
            snprintf(ip, sizeof(ip), IPV6STR, IPV62STR(sta_status.ip6_addr));
            cJSON_AddStringToObject(sta, "ip6", ip);
        }
    }
 */
    response->setLength();
    request->send(response);
    return ESP_OK;

}

/*
static esp_err_t wifi_scan_get_handler(httpd_req_t *req)
{
    if (check_auth(req) == ESP_FAIL)
        return ESP_FAIL;

    uint16_t ap_count;
    wifi_ap_record_t *ap_records = wifi_scan(&ap_count);

    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < ap_count; i++)
    {
        wifi_ap_record_t *ap_record = &ap_records[i];
        cJSON *ap = cJSON_CreateObject();
        cJSON_AddItemToArray(root, ap);
        cJSON_AddStringToObject(ap, "ssid", (char *)ap_record->ssid);
        cJSON_AddNumberToObject(ap, "rssi", ap_record->rssi);
        cJSON_AddStringToObject(ap, "authmode", wifi_auth_mode_name(ap_record->authmode));
    }

    free(ap_records);

    return json_response(req, root);
}
*/



 
void WebServices::begin(AsyncWebServer *server)
{
    /// make sure to start webserial first, because otherwise the last catch all here will probably block the webserial handler
    _server = server;

    if (NVS.getInt(CONFIG_ITEMS[CONFIG_ADMIN_AUTH].key) == AUTH_METHOD_BASIC){}; 
    {
        String dummy="Basic " + base64::encode(NVS.getString(CONFIG_ITEMS[CONFIG_ADMIN_USERNAME].key)+":"+NVS.getString(CONFIG_ITEMS[CONFIG_ADMIN_PASSWORD].key));
        basic_authentication = (char *) calloc(1,dummy.length() + 1);
        basic_authentication = dummy.c_str();
    }


    _server->on("/config", HTTP_GET, config_get_handler);

    AsyncCallbackJsonWebHandler *config_post_handler_json = new AsyncCallbackJsonWebHandler("/config", config_post_handler);
    _server->addHandler(config_post_handler_json);

    _server->on("/status", HTTP_GET, status_get_handler);

        // _server->on("/log", HTTP_GET, log_get_handler);
        // _server->on("/core_dump", HTTP_GET, core_dump_get_handler);
    _server->on("/heap_info", HTTP_GET, heap_info_get_handler);

        // _server->on("/wifi/scan", HTTP_GET, wifi_scan_get_handler);
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SD, "/www/index.html", "text/html");
});
    _server->serveStatic("/sd/", SD, "/");
    _server->serveStatic("/", SD, "/www/");    
    _server->on("/*", HTTP_GET, file_get_handler);

}
