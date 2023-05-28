

#ifndef PODRUM_MISC_LOGGER_H
#define PODRUM_MISC_LOGGER_H

void log_generic(char *message, char *type_name, char *type_color);

void log_info(char *message);

void log_warning(char *message);

void log_error(char *message);

void log_success(char *message);

void log_emergency(char *message);

void log_notice(char *message);

void log_critical(char *message);

void log_debug(char *message);

#endif