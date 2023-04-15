/* This doesn't work. Can't figure out how to get the HMAC to match.  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define STORAGE_ACCOUNT "glockteststorage"
#define CONTAINER_NAME "glocktestscratch"
#define STORAGE_KEY ""

static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    return nitems * size;
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    printf("%.*s", size * nmemb, ptr);
    return size * nmemb;
}

char* hmac_base64(const char* key, const char* data) {
    unsigned char* result = NULL;
    unsigned int result_len;
    char* encoded_result = NULL;

    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)data, strlen(data), result, &result_len);
    result = malloc(result_len);

    HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)data, strlen(data), result, &result_len);
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, result, result_len);
    BIO_flush(b64);
    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    encoded_result = malloc(bptr->length);
    memcpy(encoded_result, bptr->data, bptr->length - 1);
    encoded_result[bptr->length-1] = 0;

    BIO_free_all(b64);
    free(result);

    return encoded_result;
}


int main() {
    // Get the current time in GMT format
    time_t now = time(NULL);
    struct tm *gmt_time = gmtime(&now);
    char x_ms_date[30];
    strftime(x_ms_date, sizeof(x_ms_date), "%a, %d %b %Y %H:%M:%S GMT", gmt_time);

    // Generate the authorization header
    char string_to_sign[4096];
    snprintf(string_to_sign,
	sizeof(string_to_sign),
	"GET\n\n\n\n\napplication/xml\n\n\n\n\n\n\nx-ms-date:%s\nx-ms-version:2022-11-02\n/%s/%s\nrestype:container",
	x_ms_date,
	STORAGE_ACCOUNT,
	CONTAINER_NAME);
    printf(string_to_sign);
    printf("\n----------\n");

    char* signature = hmac_base64(STORAGE_KEY, string_to_sign);
    printf("%s\n", signature);

    // Set the request headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "x-ms-version: 2022-11-02");
    char buf[2048];
    sprintf(buf, "x-ms-date: %s", x_ms_date);
    headers = curl_slist_append(headers, buf);
    sprintf(buf, "Authorization: SharedKey %s:%s",STORAGE_ACCOUNT, signature);
    free(signature);
    headers = curl_slist_append(headers, buf);

    headers = curl_slist_append(headers, "Content-Type: application/xml");

    // Set the URL
    char url[1024];
    snprintf(url, sizeof(url), "https://%s.blob.core.windows.net/%s?restype=container", STORAGE_ACCOUNT, CONTAINER_NAME);

    // Initialize libcurl
    CURL *curl = curl_easy_init();
    if (curl) {
        // Set the options
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        return (int)res;
    } else {
        return 1;
    }
}
