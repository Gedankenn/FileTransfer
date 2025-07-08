#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((char *)userp)[0] = '\0';  // Clear buffer
    strncat(userp, contents, size * nmemb);
    return size * nmemb;
}

int find_my_ip(int type, char* ip) {
    CURL *curl;
    CURLcode res;
    int ret = EXIT_FAILURE;
    int ipType = type == 4 ? CURL_IPRESOLVE_V4 : CURL_IPRESOLVE_V6;
    ip[0] = '\0';

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://ifconfig.me");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ip);
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, ipType);

        res = curl_easy_perform(curl);
        if(res == CURLE_OK)
        {
            printf("Public IP: %s\n", ip);
            ret = EXIT_SUCCESS;
        }
        else
        {
            fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
            ret = EXIT_FAILURE;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return ret;
}

