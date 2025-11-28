#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define DEFAULT_CONFIG_FILE "zabbix_server.conf"

/* Minimal task flags mimicking the server binary */
#define ZBX_TASK_START              0
#define ZBX_TASK_RUNTIME_CONTROL    1
#define ZBX_TASK_TEST_CONFIG        2

#define ZBX_TASK_FLAG_FOREGROUND    0x01

typedef struct
{
    int task;
    int flags;
    char *opts;
} zbx_task;

static const char *usage_message[] = {
    "[-c config-file]",
    "[-c config-file] -R runtime-option",
    "[-c config-file] -T",
    "-h",
    "-V",
    NULL
};

static const char *help_message[] = {
    "The core daemon of Zabbix software (minimal example).",
    "",
    "Options:",
    "  -c --config config-file        Path to the configuration file",
    "                                 (default: \"" DEFAULT_CONFIG_FILE "\")",
    "  -f --foreground                Run Zabbix server in foreground",
    "  -R --runtime-control runtime-option   Perform administrative functions",
    "  -T --test-config               Validate configuration file and exit",
    "  -h --help                      Display this help message",
    "  -V --version                   Display version number",
    NULL
};

static const struct option longopts[] = {
    {"config",           required_argument, NULL, 'c'},
    {"runtime-control",  required_argument, NULL, 'R'},
    {"test-config",      no_argument,       NULL, 'T'},
    {"help",             no_argument,       NULL, 'h'},
    {"version",          no_argument,       NULL, 'V'},
    {"foreground",       no_argument,       NULL, 'f'},
    {0, 0, 0, 0}
};

static char *zbx_strdup(char *dst, const char *src)
{
    if (NULL != dst)
    {
        free(dst);
    }

    if (NULL == src)
    {
        return NULL;
    }

    char *copy = strdup(src);

    if (NULL == copy)
    {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    return copy;
}

static void zbx_print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s ", progname);

    for (int i = 0; NULL != usage_message[i]; i++)
    {
        if (0 == i)
            fprintf(stderr, "%s\n", usage_message[i]);
        else
            fprintf(stderr, "       %s\n", usage_message[i]);
    }
}

static void zbx_print_help(const char *progname)
{
    printf("Usage: %s\n", progname);

    for (int i = 0; NULL != help_message[i]; i++)
        printf("%s\n", help_message[i]);
}

static void zbx_print_version(const char *title)
{
    printf("%s (example)\n", title);
}

int main(int argc, char **argv)
{
    zbx_task t = {ZBX_TASK_START, 0, NULL};
    const char *title_message = "zabbix_server";
    char *config_file = NULL;
    int opt_c = 0, opt_r = 0, opt_t = 0, opt_f = 0;
    int ch;

    while ((ch = getopt_long(argc, argv, ":c:R:ThVf", longopts, NULL)) != -1)
    {
        switch (ch)
        {
            case 'c':
                opt_c++;
                if (NULL == config_file)
                    config_file = zbx_strdup(config_file, optarg);
                break;
            case 'R':
                opt_r++;
                t.opts = zbx_strdup(t.opts, optarg);
                t.task = ZBX_TASK_RUNTIME_CONTROL;
                break;
            case 'T':
                opt_t++;
                t.task = ZBX_TASK_TEST_CONFIG;
                break;
            case 'h':
                zbx_print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 'V':
                zbx_print_version(title_message);
                exit(EXIT_SUCCESS);
            case 'f':
                opt_f++;
                t.flags |= ZBX_TASK_FLAG_FOREGROUND;
                break;
            case ':':
                fprintf(stderr, "option '-%c' requires an argument\n", optopt);
                zbx_print_usage(argv[0]);
                exit(EXIT_FAILURE);
            case '?':
            default:
                zbx_print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (1 < opt_c || 1 < opt_r || 1 < opt_t || 1 < opt_f)
    {
        if (1 < opt_c)
            fprintf(stderr, "option '-c' or '--config' specified multiple times\n");
        if (1 < opt_r)
            fprintf(stderr, "option '-R' or '--runtime-control' specified multiple times\n");
        if (1 < opt_t)
            fprintf(stderr, "option '-T' or '--test-config' specified multiple times\n");
        if (1 < opt_f)
            fprintf(stderr, "option '-f' or '--foreground' specified multiple times\n");

        exit(EXIT_FAILURE);
    }

    if (0 != opt_t && 0 != opt_r)
    {
        fprintf(stderr, "option '-T' or '--test-config' cannot be specified with '-R'\n");
        exit(EXIT_FAILURE);
    }

    if (optind < argc)
    {
        for (int i = optind; i < argc; i++)
            fprintf(stderr, "invalid parameter '%s'\n", argv[i]);

        exit(EXIT_FAILURE);
    }

    if (NULL == config_file)
        config_file = zbx_strdup(NULL, DEFAULT_CONFIG_FILE);

    printf("Config file: %s\n", config_file);
    printf("Foreground: %s\n", (t.flags & ZBX_TASK_FLAG_FOREGROUND) ? "yes" : "no");

    if (t.task == ZBX_TASK_RUNTIME_CONTROL)
        printf("Runtime control option: %s\n", t.opts);
    else if (t.task == ZBX_TASK_TEST_CONFIG)
        printf("Configuration test requested\n");
    else
        printf("Normal start\n");

    free(t.opts);
    free(config_file);

    return EXIT_SUCCESS;
}
