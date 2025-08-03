#include "utils.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP_BUFFER_SIZE 128

static size_t write_callback(void *contents, size_t size, size_t nmemb,
                             void *userp)
{
    size_t total_size = size * nmemb;
    size_t current_len = strlen((char *)userp);
    size_t max_append = IP_BUFFER_SIZE - current_len - 1;

    if (max_append > 0) {
        strncat((char *)userp, (char *)contents,
                max_append < total_size ? max_append : total_size);
    }

    return total_size;
}

int find_my_ip(int type, char *ip)
{
    CURL *curl;
    CURLcode res;
    int ret = EXIT_FAILURE;
    int ipType = (type == 6) ? CURL_IPRESOLVE_V6 : CURL_IPRESOLVE_V4;

    if (!ip)
        return ret;
    ip[0] = '\0';

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://ifconfig.me");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ip);
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, ipType);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Safety timeout
        curl_easy_setopt(curl, CURLOPT_USERAGENT,
                         "curl/7.81.0"); // Required by ifconfig.me

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            ret = EXIT_SUCCESS;
        } else {
            fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    return ret;
}

