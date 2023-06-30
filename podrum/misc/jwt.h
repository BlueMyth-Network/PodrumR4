

#ifndef PODRUM_MISC_JWT_H
#define PODRUM_MISC_JWT_H

#include <podrum/misc/json.h>

typedef struct {
  json_root_t header;
  json_root_t payload;
  char *signature;
  uint8_t status;
  char *raw_header;
  char *raw_payload;
} jwt_data_t;

jwt_data_t jwt_decode(char *token);
#endif
