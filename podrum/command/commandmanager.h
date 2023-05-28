

#ifndef PODRUM_COMMAND_COMMANDMANAGER_H
#define PODRUM_COMMAND_COMMANDMANAGER_H

#include <stdlib.h>
#include <stdint.h>

typedef void (*command_executor_t)(int argc, char **argv);

typedef struct {
    char *name;
    char *description;
    char *usage;
    char *prefix;
    uint8_t flags; /* 1 = Error flag */
    command_executor_t executor;
} command_t;

typedef struct {
	command_t *commands;
	size_t commands_count;
} command_manager_t;

void register_command(command_t command, command_manager_t *manager);

command_t get_command(char *name, command_manager_t *manager);

void delete_command(char *name, command_manager_t *manager);

void execute(char *name, int argc, char **argv, command_manager_t *manager);

#endif