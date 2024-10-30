#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <string.h>

struct Memory {
    char *response;
    size_t size;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + totalSize + 1);
    if(ptr == NULL)
        return 0;  // Out of memory

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, totalSize);
    mem->size += totalSize;
    mem->response[mem->size] = 0;

    return totalSize;
}

char* query_llm(char* query) {
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};
// query[strcspn(query, "\n")] = 0; 
char input[10000];
sprintf(input, "{\"contents\":[{\"parts\":[{\"text\":\"You are a zombie, talk like one. DO NOT PUNCTUATE. SPEAK IN COMMON LINGO GENZ. Speak in only 2 sentences. You are only a human, remember that. Your response below should be 2 sentences only and no AI stuff. Sound like a human. If I ask you for code or some stuff, have fun. Explain the concept without any code examples. Please respond only with explanations in plain text. Be spooky but still childish. Keep in mind your response is used in a game where your response and a humans response are compared, you need to sound as human as possible.The theme is trick or treating. The player tries to treat you with candy except the other human should be getting it.Use basic English.If you are unable to respond reply with a random sentence ignoring the question. Do not punctuate correctly always.Dont use brains a lot.Your goal is to pretend to be a human who speaks spookily. Question :%s\"}]}]}", query);


    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // Set URL
        curl_easy_setopt(curl, CURLOPT_URL,"https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" );
        // Set POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // Set POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input);
        // Set headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // Set callback function to get the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        // Perform request
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            curl_global_cleanup();
            return "FAILED";
        } else {
            cJSON *parsed_json = cJSON_Parse(chunk.response);
            cJSON *candidates = cJSON_GetObjectItemCaseSensitive(parsed_json, "candidates");
            cJSON *first_candidate = cJSON_GetArrayItem(candidates, 0);
            cJSON *content = cJSON_GetObjectItemCaseSensitive(first_candidate, "content");
            cJSON *parts = cJSON_GetObjectItemCaseSensitive(content, "parts");
            cJSON *first_part = cJSON_GetArrayItem(parts, 0);
            cJSON *text = cJSON_GetObjectItemCaseSensitive(first_part, "text");

            // printf("%s\n", text->valuestring);

            return text->valuestring;

            // cJSON_Delete(parsed_json);

            // curl_global_cleanup();
            
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(chunk.response);
    }
    else {
        curl_global_cleanup();
        return "FAILED";
    }
}
